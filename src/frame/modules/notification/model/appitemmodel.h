/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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

#include <QObject>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace dcc {
namespace notification {

class AppItemModel : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        APPNAME,
        APPICON,
        ENABELNOTIFICATION,
        ENABELPREVIEW,
        ENABELSOUND,
        SHOWINNOTIFICATIONCENTER,
        LOCKSCREENSHOWNOTIFICATION
    } AppConfigurationItem;

    explicit AppItemModel(QObject *parent = nullptr);

    inline QString getAppName()const {return  m_softName;}
    void setSoftName(const QString &name);

    inline QString getIcon()const {return m_icon;}
    void setIcon(const QString &icon);

    inline bool isAllowNotify()const {return m_isAllowNotify;}
    void setAllowNotify(const bool &state);

    inline bool isNotifySound()const {return m_isNotifySound;}
    void setNotifySound(const bool &state);

    inline bool isLockShowNotify()const {return m_isLockShowNotify;}
    void setLockShowNotify(const bool &state);

    inline bool isShowInNotifyCenter()const {return m_isShowInNotifyCenter;}
    void setShowInNotifyCenter(const bool &state);

    inline bool isShowNotifyPreview()const {return m_isShowNotifyPreview;}
    void setShowNotifyPreview(const bool &state);

    inline QString getActName()const {return m_actName;}
    void setActName(const QString &name);

public Q_SLOTS:
    void onSettingChanged(const QString &id, const uint &item, QDBusVariant var);

Q_SIGNALS:
    void softNameChanged(QString name);
    void iconChanged(QString icon);
    void allowNotifyChanged(bool state);
    void notifySoundChanged(bool state);
    void lockShowNotifyChanged(bool state);
    void showInNotifyCenterChanged(bool state);
    void showNotifyPreviewChanged(bool state);

private:
    QString m_softName;//???????????????
    QString m_icon;//??????????????????
    QString m_actName;//?????????????????????
    bool m_isAllowNotify;//??????????????????
    bool m_isNotifySound;//?????????????????????
    bool m_isLockShowNotify;//??????????????????
    bool m_isShowInNotifyCenter;//??????????????????????????????
    bool m_isShowNotifyPreview;//??????????????????
};

}
}
