/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "updatemodule.h"
#include "modules/update/updatemodel.h"
#include "modules/update/updatework.h"
#include "updatewidget.h"
#include "mirrorswidget.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "modules/systeminfo/systeminfowork.h"
#include "widgets/utils.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QGSettings>

DCORE_USE_NAMESPACE

using namespace dcc;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateModule::UpdateModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_updateWidget(nullptr)
    , m_mirrorsWidget(nullptr)
{

}

UpdateModule::~UpdateModule()
{
    if (m_workThread) {
        m_workThread->quit();
        m_workThread->wait();
    }
}

void UpdateModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    if (!DSysInfo::isDeepin()) {
        qInfo() << "module: " << displayName() << " is disable now!";
        m_frameProxy->setModuleVisible(this, false);
        setDeviceUnavailabel(true);
        return;
    }

    Q_UNUSED(sync);
    Q_UNUSED(pushtype);

    m_workThread = QSharedPointer<QThread>(new QThread);
    m_model = new UpdateModel(this);
    m_work  = QSharedPointer<UpdateWorker>(new UpdateWorker(m_model));
    m_work->moveToThread(m_workThread.get());
    m_workThread->start(QThread::LowPriority);

    connect(m_work.get(), &UpdateWorker::requestInit, m_work.get(), &UpdateWorker::init);
    connect(m_work.get(), &UpdateWorker::requestActive, m_work.get(), &UpdateWorker::activate);
    connect(m_work.get(), &UpdateWorker::requestRefreshLicenseState, m_work.get(), &UpdateWorker::licenseStateChangeSlot);

#ifndef DISABLE_SYS_UPDATE_MIRRORS
    connect(m_work.get(), &UpdateWorker::requestRefreshMirrors, m_work.get(), &UpdateWorker::refreshMirrors);
#endif
    // ???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
    connect(m_model, &UpdateModel::updateNotifyChanged, this, [this](const bool state) {
        //?????????????????????????????????????????????
        if (!state) {
            m_frameProxy->setModuleSubscriptVisible(name(), false);
        } else {
            UpdatesStatus status = m_model->status();
            if (status == UpdatesStatus::UpdatesAvailable || status == UpdatesStatus::Downloading || status == UpdatesStatus::DownloadPaused || status == UpdatesStatus::Downloaded ||
                status == UpdatesStatus::Installing || status == UpdatesStatus::RecoveryBackingup || status == UpdatesStatus::RecoveryBackingSuccessed || m_model->getUpdatablePackages()) {
                m_frameProxy->setModuleSubscriptVisible(name(), true);
            }
        }
    });
    connect(m_model, &UpdateModel::statusChanged, this, &UpdateModule::notifyDisplayReminder);

    // ??????????????????????????????
    onUpdatablePackagesChanged(m_model->getUpdatablePackages());
    connect(m_model, &UpdateModel::updatablePackagesChanged, this, &UpdateModule::onUpdatablePackagesChanged);

    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler && m_hideModuleName.contains("update")) {
        m_frameProxy->setModuleVisible(this, false);
        setDeviceUnavailabel(true);
    } else {
        bool bShowUpdate = valueByQSettings<bool>(DCC_CONFIG_FILES, "", "showUpdate", true);
        m_frameProxy->setModuleVisible(this, bShowUpdate);
        setDeviceUnavailabel(!bShowUpdate);
    }

#ifndef DISABLE_ACTIVATOR
    connect(m_model, &UpdateModel::systemActivationChanged, this, [=](UiActiveState systemactivation) {
        if (systemactivation == UiActiveState::Authorized || systemactivation == UiActiveState::TrialAuthorized || systemactivation == UiActiveState::AuthorizedLapse) {
            if (m_updateWidget)
                m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
        }
    });
#endif

    Q_EMIT m_work->requestInit();
    Q_EMIT m_work->requestActive();
}

void UpdateModule::initialize()
{
}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

const QString UpdateModule::displayName() const
{
    return tr("Updates");
}

void UpdateModule::active()
{
    connect(m_model, &UpdateModel::downloadInfoChanged, m_work.get(), &UpdateWorker::onNotifyDownloadInfoChanged);
    connect(m_model, &UpdateModel::beginCheckUpdate, m_work.get(), &UpdateWorker::checkForUpdates);
    connect(m_model, &UpdateModel::updateHistoryAppInfos, m_work.get(), &UpdateWorker::refreshHistoryAppsInfo, Qt::DirectConnection);
    connect(m_model, &UpdateModel::updateCheckUpdateTime, m_work.get(), &UpdateWorker::refreshLastTimeAndCheckCircle, Qt::DirectConnection);

    m_updateWidget= new UpdateWidget;
    m_updateWidget->setVisible(false);
    m_updateWidget->initialize();

    Q_EMIT m_work->requestRefreshLicenseState();

    if (m_model->systemActivation() == UiActiveState::Authorized || m_model->systemActivation() == UiActiveState::TrialAuthorized || m_model->systemActivation() == UiActiveState::AuthorizedLapse) {
        m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
    }
    m_updateWidget->setModel(m_model, m_work.get());

    connect(m_updateWidget, &UpdateWidget::pushMirrorsView, this, [=]() {
        m_mirrorsWidget = new MirrorsWidget(m_model);
        m_mirrorsWidget->setVisible(false);
        int topWidgetWidth = m_updateWidget->parentWidget()->parentWidget()->width();
        m_work->checkNetselect();
        m_mirrorsWidget->setMinimumWidth(topWidgetWidth / 2);
        m_mirrorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work.get(), &UpdateWorker::setMirrorSource);
        connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work.get(), &UpdateWorker::testMirrorSpeed);
        connect(m_mirrorsWidget, &MirrorsWidget::notifyDestroy, this, [this]() {
            //notifyDestroy?????????????????????????????????????????????????????????????????????????????????????????????
            m_mirrorsWidget = nullptr;
        });
        connect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);

        //mainWidget->parentWidget()->parentWidget()???mainwindow
        //1690?????????????????????????????????????????????(????????????????????????)
        if (topWidgetWidth <= 1690) {
            m_frameProxy->pushWidget(this, m_mirrorsWidget, dccV20::FrameProxyInterface::PushType::DirectTop);
        } else {
            m_frameProxy->pushWidget(this, m_mirrorsWidget);
        }
        m_mirrorsWidget->setVisible(true);
    });

#ifndef DISABLE_ACTIVATOR
    if (m_model->systemActivation() == UiActiveState::Authorized || m_model->systemActivation() == UiActiveState::TrialAuthorized || m_model->systemActivation() == UiActiveState::AuthorizedLapse) {
        m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
    }
#else
    mainWidget->setSystemVersion(m_model->systemVersionInfo());
#endif

    m_frameProxy->pushWidget(this, m_updateWidget);
    m_updateWidget->setVisible(true);
    m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
}

void UpdateModule::deactive()
{
    if (m_model) {
        m_model->deleteLater();
        m_model = nullptr;
    }

    if (m_work) {
        m_work->deleteLater();
        m_work = nullptr;
    }
}

int UpdateModule::load(const QString &path)
{
    int hasPage = -1;
    if (m_updateWidget) {
        if (path == "Update Settings") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSetting);
        } else if (path == "Update") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        } else if (path == "Update Settings/Mirror List") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSettingMir);
        } else if (path == "Checking") {
            m_model->setStatus(UpdatesStatus::Checking);
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        }
    }

    return hasPage;
}

QStringList UpdateModule::availPage() const
{
    return QStringList() << "Update Settings" << "Update" << "Update Settings/Mirror List" << "Checking";
}

void UpdateModule::onNotifyDealMirrorWidget(bool state)
{
    //m_mirrorsWidget??????????????????????????????
    if (state && m_mirrorsWidget) {
        m_frameProxy->popWidget(this);
        //popWidget?????????????????????????????????,???m_mirrorsWidget????????????,???????????????????????????nullptr
        //?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????(?????????????????????)
        m_mirrorsWidget = nullptr;
        //?????????????????????????????????,?????????????????????
        disconnect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);
    }
}

void UpdateModule::notifyDisplayReminder(UpdatesStatus status)
{
    if (!m_model->updateNotify()) {
        m_frameProxy->setModuleSubscriptVisible(name(), false);
        return;
    }

    if (status == UpdatesStatus::Checking) {
        //do nothing
    } else if (status == UpdatesStatus::UpdatesAvailable ||
               status == UpdatesStatus::Downloading ||
               status == UpdatesStatus::DownloadPaused ||
               status == UpdatesStatus::Downloaded ||
               status == UpdatesStatus::Installing ||
               status == UpdatesStatus::RecoveryBackingup ||
               status == UpdatesStatus::RecoveryBackingSuccessed) {
        m_frameProxy->setModuleSubscriptVisible(name(), true);
    } else {
        m_frameProxy->setModuleSubscriptVisible(name(), false);
    }
}

void UpdateModule::onUpdatablePackagesChanged(const bool isUpdatablePackages)
{
    m_frameProxy->setModuleSubscriptVisible(name(), isUpdatablePackages && m_model->updateNotify());
}
