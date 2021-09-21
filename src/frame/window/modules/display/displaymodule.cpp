/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displaymodule.h"
#include "displaywidget.h"
#include "brightnesswidget.h"
#include "scalingwidget.h"
#include "resolutionwidget.h"
#include "refreshratewidget.h"
#include "rotatewidget.h"
#include "secondaryscreendialog.h"
#include "multiscreenwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/timeoutdialog.h"
#include "modules/display/displaymodel.h"
#include "modules/display/displayworker.h"
#include "modules/display/recognizewidget.h"

#include <QApplication>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_displayModel(nullptr)
    , m_displayWorker(nullptr)
    , m_displayWidget(nullptr)
{
    // 用于传入MultiScreenWidget销毁副屏窗口
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();

    GSettingWatcher::instance()->erase("displayResolution");
    GSettingWatcher::instance()->erase("displayRefreshRate");
    GSettingWatcher::instance()->erase("displayRotate");
}

void DisplayModule::windowUpdate()
{
    if (m_pMainWindow)
        m_pMainWindow->updateWinsize();
}

void DisplayModule::initialize()
{
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

const QString DisplayModule::displayName() const
{
    return tr("Display");
}

void DisplayModule::active()
{
    m_displayWidget = new DisplayWidget(m_pMainWindow);
    m_displayWidget->setVisible(false);
    pushScreenWidget();
    m_frameProxy->pushWidget(this, m_displayWidget);
    QTimer::singleShot(0, this, [=] {
        if (m_frameProxy->currModule()->name() != name())
            return;
        m_displayWidget->setVisible(true);
    });
}

int DisplayModule::load(const QString &path)
{
    if (!m_displayWidget) {
        active();
    }

    return m_displayWidget->showPath(path);
}

void DisplayModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    if (m_displayModel) {
        delete m_displayModel;
    }
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel, nullptr, sync);

    m_displayModel->moveToThread(qApp->thread());
    m_displayWorker->moveToThread(qApp->thread());

    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [=] {
        //当前从显示模块切到了其他模块, 会导致m_displayWidget不为空,但是也有些数据被delete, 容易访问空指针
        //加不在当前模块内就不处理显示器插拔的信号
        if (m_frameProxy->currModule() && m_frameProxy->currModule()->name() != name())
            return;
        if (m_displayWidget != nullptr) {
            pushScreenWidget();
        }
    });

    QTimer::singleShot(0, m_displayWorker, [=] {
        m_displayWorker->active();
    });
}

QStringList DisplayModule::availPage() const
{
    QStringList sl;
    sl << "Resolution"
       << "Refresh Rate"
       << "Brightness";

    if (m_displayModel && m_displayModel->monitorList().size() > 1) {
        sl << "Multiple Displays";
    }

    if (IsServerSystem) {
        sl << "Display Scaling";
    }

    if (m_displayModel && !m_displayModel->touchscreenList().isEmpty()) {
        sl << "Touch Screen";
    }

    return sl;
}

void DisplayModule::showSingleScreenWidget()
{
    QWidget *singleScreenWidget = new QWidget(m_displayWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(56, 20, 56, 0);

    BrightnessWidget *brightnessWidget = new BrightnessWidget(singleScreenWidget);
    brightnessWidget->setMode(m_displayModel);
    contentLayout->addWidget(brightnessWidget);
    brightnessWidget->setVisible(m_displayModel->brightnessEnable());
    connect(brightnessWidget, &BrightnessWidget::requestSetColorTemperature, m_displayWorker, &DisplayWorker::setColorTemperature);
    connect(brightnessWidget, &BrightnessWidget::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestAmbientLightAdjustBrightness, m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(brightnessWidget, &BrightnessWidget::requestSetMethodAdjustCCT, m_displayWorker, &DisplayWorker::SetMethodAdjustCCT);

    QSpacerItem *scalingSpacerItem = new QSpacerItem(0, m_displayModel->brightnessEnable() ? 20 : 0);
    contentLayout->addSpacerItem(scalingSpacerItem);

    ScalingWidget *scalingWidget = new ScalingWidget(singleScreenWidget);
    scalingWidget->setModel(m_displayModel);
    contentLayout->addWidget(scalingWidget);
    connect(scalingWidget, &ScalingWidget::requestUiScaleChange, m_displayWorker, &DisplayWorker::setUiScale);

    QSpacerItem *resolutionSpacerItem = new QSpacerItem(0, 30);
    contentLayout->addSpacerItem(resolutionSpacerItem);

    ResolutionWidget *resolutionWidget = new ResolutionWidget(300, singleScreenWidget);
    resolutionWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayResolution", resolutionWidget);  // 使用GSettings来控制显示状态
    contentLayout->addWidget(resolutionWidget);
    connect(resolutionWidget, &ResolutionWidget::requestSetResolution, this, [=](dcc::display::Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        windowUpdate();
    }, Qt::QueuedConnection);

    RefreshRateWidget *refreshRateWidget = new RefreshRateWidget(300, singleScreenWidget);
    refreshRateWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayRefreshRate", refreshRateWidget);  // 使用GSettings来控制显示状态
    contentLayout->addSpacing(20);
    contentLayout->addWidget(refreshRateWidget);
    connect(refreshRateWidget, &RefreshRateWidget::requestSetResolution, this, &DisplayModule::onRequestSetResolution, Qt::QueuedConnection);

    RotateWidget *rotateWidget = new RotateWidget(300, singleScreenWidget);
    rotateWidget->setModel(m_displayModel, m_displayModel->monitorList().count() ? m_displayModel->monitorList().first() : nullptr);
    GSettingWatcher::instance()->bind("displayRotate", rotateWidget);  // 使用GSettings来控制显示状态
    contentLayout->addSpacing(20);
    contentLayout->addWidget(rotateWidget);
    connect(rotateWidget, &RotateWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);

    contentLayout->addStretch();

    singleScreenWidget->setLayout(contentLayout);
    m_displayWidget->setContent(singleScreenWidget);

    connect(m_displayModel, &DisplayModel::brightnessEnableChanged, this, [brightnessWidget, scalingSpacerItem](const bool enable) {
        scalingSpacerItem->changeSize(0, enable ? 20 : 0);
        brightnessWidget->setVisible(enable);
    });
}

void DisplayModule::showMultiScreenWidget()
{
    MultiScreenWidget *multiScreenWidget = new MultiScreenWidget(m_pMainWindow);
    multiScreenWidget->setModel(m_displayModel);
    connect(multiScreenWidget, &MultiScreenWidget::requestRecognize, this, &DisplayModule::showDisplayRecognize);
    connect(multiScreenWidget, &MultiScreenWidget::requestSwitchMode, m_displayWorker, &DisplayWorker::switchMode);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorPosition, m_displayWorker, &DisplayWorker::setMonitorPosition);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetPrimary, m_displayWorker, &DisplayWorker::setPrimary);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetColorTemperature, m_displayWorker, &DisplayWorker::setColorTemperature);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestAmbientLightAdjustBrightness, m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMethodAdjustCCT, m_displayWorker, &DisplayWorker::SetMethodAdjustCCT);
    connect(multiScreenWidget, &MultiScreenWidget::requestUiScaleChange, m_displayWorker, &DisplayWorker::setUiScale);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetResolution, this, [=](dcc::display::Monitor *monitor, const int mode) {
        onRequestSetResolution(monitor, mode);
        windowUpdate();
    }, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetRotate, this, &DisplayModule::onRequestSetRotate, Qt::QueuedConnection);
    connect(multiScreenWidget, &MultiScreenWidget::requestSetMainwindowRect, this, [=](Monitor *moi,  bool isInit) {
        bool stateChanged = false;
        //窗口初始化且窗口最大化的时候不需要移动窗口
        if (m_pMainWindow->isMaximized()) {
            if(isInit){
                return;
            }
            m_pMainWindow->setNeedRememberLastSize(false);
            m_pMainWindow->showNormal();

            QSize lastsize = m_pMainWindow->getLastSize();
            if (!lastsize.isValid() || lastsize == m_pMainWindow->maximumSize()) {
                lastsize.setWidth(m_pMainWindow->minimumWidth());
                lastsize.setHeight(m_pMainWindow->minimumHeight());
            }
            m_pMainWindow->resize(lastsize);
            stateChanged = true;
        }

        double scale = m_displayModel->monitorScale(moi);
        m_pMainWindow->move(int(moi->x() + moi->w() / scale / 2 - m_pMainWindow->width() / 2), int(moi->y() + moi->h() / scale / 2 - m_pMainWindow->height() / 2));
        if (stateChanged) {
            m_pMainWindow->showMaximized();
        }
    });

    m_displayWidget->setContent(multiScreenWidget);
}

void DisplayModule::onRequestSetResolution(Monitor *monitor, const int mode)
{
    Resolution lastRes = monitor->currentMode();
    Resolution firstRes;

    for (auto res : monitor->modeList()) {
        if (res.id() == mode) {
            firstRes = res;
            break;
        }
    }

    auto tfunc = [this](Monitor *tmon, Resolution tmode) {
        if (m_displayModel->displayMode() == MERGE_MODE) {
            for (auto monitor : m_displayModel->monitorList()) {
                bool bFind = false;
                for (auto res : monitor->modeList()) {
                    if (res == tmode) {
                        m_displayWorker->setMonitorResolution(monitor, res.id());
                        bFind = true;
                        break;
                    }
                }
                if (!bFind) {
                    m_displayWorker->setMonitorResolutionBySize(monitor, tmode.width(), tmode.height());
                }
            }
        } else {
            m_displayWorker->setMonitorResolution(tmon, tmode.id());
        }
        m_displayWorker->applyChanges();
    };

    tfunc(monitor, firstRes);

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, tfunc, monitor, lastRes]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_displayWorker->saveChanges();
        } else {
            tfunc(monitor, lastRes);
        }
    });
}

void DisplayModule::onRequestSetRotate(Monitor *monitor, const int rotate)
{
    auto lastRotate = monitor->rotate();
    m_displayWorker->setMonitorRotate(monitor, rotate);
    m_displayWorker->applyChanges();

    //此处处理调用applyChanges的200ms延时, TimeoutDialog提前弹出的问题
    QTimer::singleShot(300, monitor, [this, monitor, lastRotate]{
        if (showTimeoutDialog(monitor) == QDialog::Accepted) {
            m_displayWorker->saveChanges();
        } else {
            m_displayWorker->setMonitorRotate(monitor, lastRotate);
            m_displayWorker->applyChanges();
        }
    });
}

void DisplayModule::pushScreenWidget()
{
    if (m_displayModel->monitorList().size() > 1) {
        showMultiScreenWidget();
    } else {
        showSingleScreenWidget();
    }
}

int DisplayModule::showTimeoutDialog(Monitor *monitor)
{
    TimeoutDialog *timeoutDialog = new TimeoutDialog(15);
    qreal radio = qApp->devicePixelRatio();
    QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
    QTimer::singleShot(1, this, [=] { timeoutDialog->moveToCenterByRect(rt.toRect()); });
    connect(monitor, &Monitor::geometryChanged, timeoutDialog, [=] {
        if (timeoutDialog) {
            QRectF rt(monitor->x(), monitor->y(), monitor->w() / radio, monitor->h() / radio);
            timeoutDialog->moveToCenterByRect(rt.toRect());
        }
    },
            Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::monitorListChanged, timeoutDialog, &TimeoutDialog::deleteLater);

    return timeoutDialog->exec();
}

void DisplayModule::showDisplayRecognize()
{
    // 复制模式
    if (m_displayModel->displayMode() == MERGE_MODE) {
        QString text = m_displayModel->monitorList().first()->name();
        for (int idx = 1; idx < m_displayModel->monitorList().size(); idx++) {
            text += QString(" = %1").arg(m_displayModel->monitorList()[idx]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeWidget.value(text) == nullptr) {
            RecognizeWidget *widget = new RecognizeWidget(m_displayModel->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=] {
                widget->deleteLater();
                m_recognizeWidget.remove(text);
            });
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_displayModel->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeWidget.value(monitor->name()) == nullptr) {
                RecognizeWidget *widget = new RecognizeWidget(monitor, monitor->name());
                m_recognizeWidget[monitor->name()] = widget;
                QTimer::singleShot(5000, this, [=] {
                    widget->deleteLater();
                    m_recognizeWidget.remove(monitor->name());
                });
                m_recognizeWidget[monitor->name()] = widget;
            }
        }
    }
}
