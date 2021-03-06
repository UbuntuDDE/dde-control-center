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

#include "vpnl2tpsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnsection.h"
#include "../../sections/vpn/vpnpppsection.h"
#include "../../sections/vpn/vpnipsecsection.h"
#include "../../sections/ipvxsection.h"
#include "../../sections/dnssection.h"

using namespace DCC_NAMESPACE::network;
using namespace NetworkManager;

VpnL2tpSettings::VpnL2tpSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnL2tpSettings::~VpnL2tpSettings()
{
}

void VpnL2tpSettings::initSections()
{
    NetworkManager::VpnSetting::Ptr vpnSetting = m_connSettings->setting(
            NetworkManager::Setting::SettingType::Vpn).staticCast<NetworkManager::VpnSetting>();

    if (!vpnSetting) {
        qDebug() << "vpn setting is invalid...";
        return;
    }

    GenericSection *genericSection = new GenericSection(m_connSettings);
    genericSection->setConnectionType(NetworkManager::ConnectionSettings::Vpn);

    VpnSection *vpnSection = new VpnSection(vpnSetting);

    VpnPPPSection *vpnPPPSection = new VpnPPPSection(vpnSetting);
    QStringList supportOptions = {
        "refuse-eap", "refuse-pap", "refuse-chap", "refuse-mschap", "refuse-mschapv2",
        "nobsdcomp", "nodeflate", "no-vj-comp", "nopcomp", "noaccomp", "lcp-echo-interval"
    };
    vpnPPPSection->setSupportOptions(supportOptions);

    VpnIpsecSection *vpnIpsecSection = new VpnIpsecSection(vpnSetting);

    IpvxSection *ipv4Section = new IpvxSection(
            m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<NetworkManager::Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);
    DNSSection *dnsSection = new DNSSection(m_connSettings, false);

    connect(genericSection, &GenericSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);
    connect(vpnSection, &VpnSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);
    connect(vpnPPPSection, &VpnPPPSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);
    connect(vpnIpsecSection, &VpnIpsecSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);
    connect(ipv4Section, &IpvxSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &VpnL2tpSettings::anyEditClicked);

    connect(vpnSection, &VpnSection::requestNextPage, this, &VpnL2tpSettings::requestNextPage);
    connect(vpnPPPSection, &VpnPPPSection::requestNextPage, this, &VpnL2tpSettings::requestNextPage);
    connect(vpnIpsecSection, &VpnIpsecSection::requestNextPage, this, &VpnL2tpSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnL2tpSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &VpnL2tpSettings::requestNextPage);

    connect(vpnSection, &VpnSection::requestFrameAutoHide, this, &VpnL2tpSettings::requestFrameAutoHide);
    connect(vpnPPPSection, &VpnPPPSection::requestFrameAutoHide, this, &VpnL2tpSettings::requestFrameAutoHide);
    connect(vpnIpsecSection, &VpnIpsecSection::requestFrameAutoHide, this, &VpnL2tpSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnL2tpSettings::requestFrameAutoHide);
    connect(dnsSection, &DNSSection::requestFrameAutoHide, this, &VpnL2tpSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnSection);
    m_sectionsLayout->addWidget(vpnPPPSection);
    m_sectionsLayout->addWidget(vpnIpsecSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(dnsSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnSection);
    m_settingSections.append(vpnPPPSection);
    m_settingSections.append(vpnIpsecSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(dnsSection);
}
