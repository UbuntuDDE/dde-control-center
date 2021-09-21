/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef POWERWORKER_H
#define POWERWORKER_H

#include <com_deepin_daemon_power.h>
#include <com_deepin_system_systempower.h>
#include <org_freedesktop_login1.h>
#include <QObject>

using PowerInter = com::deepin::daemon::Power;
using SysPowerInter = com::deepin::system::Power;
using Login1ManagerInter = org::freedesktop::login1::Manager;

namespace dcc{
namespace power {
class PowerModel;
class PowerWorker : public QObject
{
    Q_OBJECT

public:
    explicit PowerWorker(PowerModel *model, QObject *parent = 0);

    void active();
    void deactive();

public Q_SLOTS:
    void setScreenBlackLock(const bool lock);
    void setSleepLock(const bool lock);
    void setSleepOnLidOnPowerClosed(const bool sleep);
    void setSleepDelayOnPower(const int delay);
    void setSleepDelayOnBattery(const int delay);
    void setScreenBlackDelayOnPower(const int delay);
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayToModelOnPower(const int delay);
    void setScreenBlackDelayToModelOnPower(const int delay);
    void setSleepDelayToModelOnBattery(const int delay);
    void setScreenBlackDelayToModelOnBattery(const int delay);
    void setLockScreenDelayOnBattery(const int delay);
    void setLockScreenDelayOnPower(const int delay);
    void setResponseBatteryLockScreenDelay(const int delay);
    void setResponsePowerLockScreenDelay(const int delay);
    //------------sp2 add-----------------------
    void setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode);
    void setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode);
    void setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold);
    void setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction);
    void setLinePowerLidClosedAction(int nLinePowerLidClosedAction);
    void setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction);
    void setBatteryLidClosedAction(int nBatteryLidClosedAction);
    void setLowPowerNotifyEnable(bool bLowPowerNotifyEnable);
    void setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold);
    void setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold);
    //------------------------------------------
    void setPowerPlan(const QString &powerPlan);

#ifndef DCC_DISABLE_POWERSAVE
    void setEnablePowerSave(const bool isEnable);
    void setAutoEnablePowerSave(const bool isEnable);
#endif

private:
    int  converToDelayModel(int value);
    int  converToDelayDBus(int value);

private:
    PowerModel *m_powerModel;
    PowerInter *m_powerInter;
    SysPowerInter *m_sysPowerInter;
    Login1ManagerInter *m_login1ManagerInter;
};

}
}


#endif // POWERWORKER_H
