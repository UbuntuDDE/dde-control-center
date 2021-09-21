/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef WIRELESSSECTION_H
#define WIRELESSSECTION_H

#include "abstractsection.h"

#include "widgets/comboxwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/optionitem.h"
#include "widgets/switchwidget.h"
#include "widgets/spinboxwidget.h"

#include <QComboBox>

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelessdevice.h>

namespace DCC_NAMESPACE {
namespace network {

class WirelessSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit WirelessSection(NetworkManager::WirelessSetting::Ptr wiredSetting, bool isHotSpot = false, QFrame *parent = nullptr);
    virtual ~WirelessSection() override;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    void setSsidEditable(const bool editable);
    inline bool ssidIsEditable() const { return m_apSsid->isEnabled(); }
    inline const QString ssid() const { return m_apSsid->text(); }
    inline void setSsid(const QString &ssid) { m_apSsid->setText(ssid); }

Q_SIGNALS:
    void ssidChanged(const QString &ssid);

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    dcc::widgets::LineEditWidget *m_apSsid;
    QComboBox *m_deviceMacComboBox;
    dcc::widgets::ComboxWidget *m_deviceMacLine;
    //LineEditWidget *m_clonedMac;
    dcc::widgets::SwitchWidget *m_customMtuSwitch;
    dcc::widgets::SpinBoxWidget *m_customMtu;

    NetworkManager::WirelessSetting::Ptr m_wirelessSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

} /* network */
} /* dcc */

#endif /* WIRELESSSECTION_H */
