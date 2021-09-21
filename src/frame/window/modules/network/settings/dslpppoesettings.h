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

#ifndef DSLPPPOESETTINGS_H
#define DSLPPPOESETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace DCC_NAMESPACE {
namespace network {
class EthernetSection;
class DslPppoeSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit DslPppoeSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QString devPath = QString(), QWidget *parent = nullptr);
    virtual ~DslPppoeSettings() override;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE;

private:
    QString m_devicePath;
    QObject *m_parent;
    EthernetSection *m_etherNetSection;
};

} /* network */
} /* dcc */

#endif /* DSLPPPOESETTINGS_H */
