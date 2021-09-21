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

#ifndef CATEGORY_H
#define CATEGORY_H
#include <QObject>
#include <QList>
#include <QJsonObject>
namespace dcc
{
namespace defapp
{
struct App {
    QString Id;
    QString Name;
    QString DisplayName;
    QString Description;
    QString Icon;
    QString Exec;
    bool isUser;
    bool CanDelete;
    bool MimeTypeFit;

    App() : isUser(false), CanDelete(false), MimeTypeFit(false) {}

    bool operator ==(const App &app) const {
        return app.Id == Id && app.isUser == isUser;
    }

    bool operator !=(const App &app) const {
        return app.Id != Id && app.isUser != isUser;
    }
};

class Category : public QObject
{
    Q_OBJECT

public:
    explicit Category(QObject *parent = 0);

    void setDefault(const App &def);

    const QString getName() const { return m_category;}
    void setCategory(const QString &category);
    inline const QList<App> getappItem() const { return m_applist;}
    inline const QList<App> systemAppList() const { return m_systemAppList; }
    inline const QList<App> userAppList() const { return m_userAppList; }
    inline const App getDefault() { return m_default;}
    void clear();
    void addUserItem(const App &value);
    void delUserItem(const App &value);

Q_SIGNALS:
    void defaultChanged(const App &id);
    void addedUserItem(const App &app);
    void removedUserItem(const App &app);
    void categoryNameChanged(const QString &name);
    void clearAll();

private:
    QList<App> m_applist;
    QList<App> m_systemAppList;
    QList<App> m_userAppList;
    QString m_category;
    App m_default;
};
}
}

#endif // CATEGORY_H
