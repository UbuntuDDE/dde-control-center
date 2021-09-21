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
#include "window/insertplugin.h"
#include "window/utils.h"

#include <DListView>

#include <QWidget>

namespace dcc {
namespace power {
class PowerModel;
}
namespace widgets {
class MultiSelectListView;
}
} // namespace dcc

namespace DCC_NAMESPACE {
namespace power {

class PowerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerWidget(QWidget *parent = nullptr);
    virtual ~PowerWidget();

    void initialize(bool hasBattery);
    void setModel(const dcc::power::PowerModel *model);
    DTK_WIDGET_NAMESPACE::DListView *getListViewPointer();
    bool getIsUseBattety();
    void showDefaultWidget();

Q_SIGNALS:
    void requestPushWidget(int index);
    void requestShowGeneral() const;
    void requestShowUseElectric() const;
    void requestShowUseBattery() const;
    void requestUpdateSecondMenu(bool);

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void removeBattery(bool state);

private:
    void initUi();
    void initMembers();
    void initConnections();

private:
    dcc::widgets::MultiSelectListView *m_listView;
    QStandardItemModel *m_itemModel;
    const dcc::power::PowerModel *m_model;
    bool m_bhasBattery;
    QList<ListSubItem> m_menuIconText;
    QModelIndex m_lastIndex;
    int m_batteryIndex;
};

} // namespace power
} // namespace DCC_NAMESPACE
