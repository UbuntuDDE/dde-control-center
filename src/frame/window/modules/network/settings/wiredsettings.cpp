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

#include "wiredsettings.h"
#include "../sections/genericsection.h"
#include "../sections/secretwiredsection.h"
#include "../sections/ipvxsection.h"
#include "../sections/dnssection.h"
#include "../sections/ethernetsection.h"
#include "window/gsettingwatcher.h"

using namespace DCC_NAMESPACE::network;
using namespace NetworkManager;

WiredSettings::WiredSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

WiredSettings::~WiredSettings()
{
}

void WiredSettings::initSections()
{
    QFrame *frame = new QFrame(this);
    GenericSection *genericSection = new GenericSection(m_connSettings, frame);
    genericSection->setConnectionType(NetworkManager::ConnectionSettings::Wired);
    Secret8021xSection *secretSection = new SecretWiredSection(
        m_connSettings->setting(Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>(), frame);
    IpvxSection *ipv4Section = new IpvxSection(
        m_connSettings->setting(Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>(), frame);
    IpvxSection *ipv6Section = new IpvxSection(
        m_connSettings->setting(Setting::Ipv6).staticCast<NetworkManager::Ipv6Setting>(), frame);
    DNSSection *dnsSection = new DNSSection(m_connSettings);
    EthernetSection *etherNetSection = new EthernetSection(
        m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>(), QString(), frame);

    // 指针destroyed时自动解绑
    GSettingWatcher::instance()->bind("wiredEditConnectionName", genericSection->connIdItem());
    GSettingWatcher::instance()->bind("wiredAutoConnect", genericSection->autoConnItem());
    GSettingWatcher::instance()->bind("wiredSecurity", secretSection);
    GSettingWatcher::instance()->bind("wiredIpv4", ipv4Section);
    GSettingWatcher::instance()->bind("wiredIpv6", ipv6Section);
    GSettingWatcher::instance()->bind("wiredEtherNet", etherNetSection);

    connect(genericSection, &GenericSection::editClicked, this, &WiredSettings::anyEditClicked);
    connect(secretSection, &Secret8021xSection::editClicked, this, &WiredSettings::anyEditClicked);
    connect(ipv4Section, &IpvxSection::editClicked, this, &WiredSettings::anyEditClicked);
    connect(ipv6Section, &IpvxSection::editClicked, this, &WiredSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &WiredSettings::anyEditClicked);
    connect(etherNetSection, &EthernetSection::editClicked, this, &WiredSettings::anyEditClicked);

    connect(secretSection, &Secret8021xSection::requestNextPage, this, &WiredSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &WiredSettings::requestNextPage);
    connect(ipv6Section, &IpvxSection::requestNextPage, this, &WiredSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &WiredSettings::requestNextPage);
    connect(etherNetSection, &EthernetSection::requestNextPage, this, &WiredSettings::requestNextPage);

    connect(secretSection, &Secret8021xSection::requestFrameAutoHide, this, &WiredSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &WiredSettings::requestFrameAutoHide);
    connect(ipv6Section, &IpvxSection::requestFrameAutoHide, this, &WiredSettings::requestFrameAutoHide);
    connect(dnsSection, &DNSSection::requestFrameAutoHide, this, &WiredSettings::requestFrameAutoHide);
    connect(etherNetSection, &EthernetSection::requestFrameAutoHide, this, &WiredSettings::requestFrameAutoHide);


    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(secretSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(ipv6Section);
    m_sectionsLayout->addWidget(dnsSection);
    m_sectionsLayout->addWidget(etherNetSection);
    m_sectionsLayout->addStretch();

    m_settingSections.append(genericSection);
    m_settingSections.append(secretSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(ipv6Section);
    m_settingSections.append(dnsSection);
    m_settingSections.append(etherNetSection);
}

bool WiredSettings::clearInterfaceName()
{
    NetworkManager::WiredSetting::Ptr wiredSetting = m_connSettings->setting(Setting::Wired).staticCast<NetworkManager::WiredSetting>();
    return wiredSetting->macAddress().isEmpty();
}
