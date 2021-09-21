/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commoninfomodule.h"
#include "widgets/multiselectlistview.h"
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/modules/commoninfo/commoninfowork.h"
#include "window/modules/commoninfo/commoninfowidget.h"
#include "window/modules/commoninfo/bootwidget.h"
#include "window/modules/commoninfo/userexperienceprogramwidget.h"
#include "window/modules/commoninfo/developermodewidget.h"
#include "window/utils.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::commoninfo;

CommonInfoModule::CommonInfoModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_commonWidget(nullptr)
    , m_bootWidget(nullptr)
    , m_ueProgramWidget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
#ifdef DCC_DISABLE_GRUB
    //如果服务器版本，且不显示GRUB(arm),则整个模块不显示
    if (IsServerSystem)
        m_frameProxy->setModuleVisible(this, false);
#endif
    GSettingWatcher::instance()->insertState("bootMenu");
    GSettingWatcher::instance()->insertState("developerMode");
    GSettingWatcher::instance()->insertState("userExperienceProgram");
}

CommonInfoModule::~CommonInfoModule()
{
    if (m_commonWork) {
//        m_commonWork->deleteLater();
        delete m_commonWork;
        m_commonWork = nullptr;
    }

    if (m_commonModel)
        m_commonModel->deleteLater();
    m_commonModel = nullptr;
}

void CommonInfoModule::preInitialize(bool sync , FrameProxyInterface::PushType pushtype)
{   
    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
#ifdef DCC_DISABLE_GRUB
    m_frameProxy->setRemoveableDeviceStatus(tr("Boot Menu"), false);
    if (IsServerSystem) {
        m_frameProxy->setModuleVisible(this, false);
    }
#else
    m_frameProxy->setRemoveableDeviceStatus(tr("Boot Menu"), true);
#endif
}

void CommonInfoModule::initialize()
{
    if (m_commonModel) {
        delete m_commonModel;
    }
    m_commonModel = new CommonInfoModel();
    m_commonWork = new CommonInfoWork(m_commonModel);

    m_commonWork->moveToThread(qApp->thread());
    m_commonModel->moveToThread(qApp->thread());
}

const QString CommonInfoModule::name() const
{
    return QStringLiteral("commoninfo");
}

const QString CommonInfoModule::displayName() const
{
    return tr("General Settings");
}

void CommonInfoModule::active()
{
    m_commonWidget = new CommonInfoWidget();
    m_commonWidget->setVisible(false);
    connect(m_commonWidget, &CommonInfoWidget::requestShowBootWidget, this, &CommonInfoModule::onShowBootWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowDeveloperModeWidget, this, &CommonInfoModule::onShowDeveloperWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowUEPlanWidget, this, &CommonInfoModule::onShowUEPlanWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowTabletModeWidget, this, &CommonInfoModule::onShowTabletModeWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestUpdateSecondMenu, this, [=] (bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
            m_frameProxy->popWidget(this);
        m_commonWidget->showDefaultWidget();
    });
    m_frameProxy->pushWidget(this, m_commonWidget);
    m_commonWidget->setVisible(true);
    m_commonWidget->showDefaultWidget();
}

void CommonInfoModule::deactive()
{
    m_commonWork->deactivate();
}

int CommonInfoModule::load(const QString &path)
{
    if (!m_commonWidget) {
        active();
    }

    QListView *list = m_commonWidget->getCommonListView();
    if (!list) {
        return 0;
    }

    int indexRow = 0;
    if (path == "Boot Menu") {
        indexRow = 0;
    }

    if (!IsServerSystem) {
        if (!IsCommunitySystem && (path == "Developer Mode")) {
            // 为开发者模式的search预留
            indexRow = 1;
        } else if (!IsCommunitySystem && (path == "User Experience Program")) {
            // 为用户体验计划的search预留
            indexRow = 2;
        } else if (path == "Tablet Mode") {
            // 为平板模式的search预留
            //indexRow = 3;
        }
    }

#ifdef DCC_DISABLE_GRUB
    indexRow ? indexRow-- : indexRow;
    Q_ASSERT(indexRow >= 0);
#endif

    QModelIndex idx = list->model()->index(indexRow, 0);
    list->setCurrentIndex(idx);
    list->clicked(idx);

    return 0;
}

QStringList CommonInfoModule::availPage() const
{
    QStringList sl;
    sl << "Boot Menu";

    if (!IsServerSystem && !IsCommunitySystem) {
        sl << "User Experience Program" << "Developer mode";
    }

    return sl;
}

void CommonInfoModule::onShowBootWidget()
{
    m_commonWork->loadGrubSettings();
    initBootWidget();
    m_frameProxy->pushWidget(this, m_bootWidget);
    m_bootWidget->setVisible(true);
}

void CommonInfoModule::onShowDeveloperWidget()
{
    DeveloperModeWidget *pWidget = new DeveloperModeWidget;
    pWidget->setVisible(false);
    pWidget->setModel(m_commonModel);
    connect(pWidget, &DeveloperModeWidget::requestLogin, m_commonWork, &CommonInfoWork::login);
    connect(pWidget, &DeveloperModeWidget::enableDeveloperMode, this, [=](bool enabled) {
        m_commonWork->setEnableDeveloperMode(enabled, m_pMainWindow);
    });
    m_frameProxy->pushWidget(this, pWidget);
    pWidget->setVisible(true);
}

void CommonInfoModule::onShowUEPlanWidget()
{
    initUeProgramWidget();
    m_frameProxy->pushWidget(this, m_ueProgramWidget);
    m_ueProgramWidget->setVisible(true);
}

// 以下内容为平板模式做预留
void CommonInfoModule::onShowTabletModeWidget()
{

}

void CommonInfoModule::initBootWidget()
{
    m_bootWidget = new BootWidget;
    m_bootWidget->setVisible(false);

    connect(m_bootWidget, &BootWidget::bootdelay, m_commonWork, &CommonInfoWork::setBootDelay);
    connect(m_bootWidget, &BootWidget::enableTheme, m_commonWork, &CommonInfoWork::setEnableTheme);
    connect(m_bootWidget, &BootWidget::defaultEntry, m_commonWork, &CommonInfoWork::setDefaultEntry);
    connect(m_bootWidget, &BootWidget::requestSetBackground, m_commonWork, &CommonInfoWork::setBackground);

    m_bootWidget->setModel(m_commonModel);
}

void CommonInfoModule::initUeProgramWidget()
{
    m_ueProgramWidget = new UserExperienceProgramWidget();
    m_ueProgramWidget->setVisible(false);
    m_ueProgramWidget->setModel(m_commonModel);
    if (!IsCommunitySystem) {
        m_ueProgramWidget->setDefaultUeProgram(m_commonWork->defaultUeProgram());
    }
    connect(m_ueProgramWidget, &UserExperienceProgramWidget::enableUeProgram, this, [=](bool enabled) {
        MainWindow *pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
        m_commonWork->setUeProgram(enabled, pMainWindow);
    });
}
