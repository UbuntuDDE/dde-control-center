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

#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "widgets/settingsitem.h"
#include "clock.h"
#include <types/zoneinfo.h>

#include <DIconButton>
#include <DTipLabel>

#include <QLabel>
#include <QString>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace datetime {

class TimezoneItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent = nullptr);

    inline ZoneInfo timeZone() const { return m_timezone; }
    void setTimeZone(const ZoneInfo &info);

    void toRemoveMode();
    void toNormalMode();

    void updateInfo();
    void setDetailVisible(bool state);

Q_SIGNALS:
    void removeClicked();

private:
    ZoneInfo m_timezone;

    QLabel *m_city;
    DTipLabel *m_details;
    Clock *m_clock;
    DIconButton *m_removeBtn;
};
}
}
#endif // TIMEZONEITEM_H
