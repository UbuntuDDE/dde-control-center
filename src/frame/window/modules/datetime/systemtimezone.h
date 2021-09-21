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
#pragma once

#include "interface/namespace.h"

#include <types/zoneinfo.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

namespace dcc {
namespace datetime {
class TimezoneItem;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class SystemTimezone : public QWidget
{
    Q_OBJECT
public:
    explicit SystemTimezone(QWidget *parent = nullptr);

Q_SIGNALS:
    void requestSetSystemTimezone();

public Q_SLOTS:
    void setSystemTimezone(const ZoneInfo &zone);

private:
    QVBoxLayout *m_layout;
    dcc::datetime::TimezoneItem *m_systemTimezone;
    QPushButton *m_setSystemTimezone;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
