/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef SNIDAEMON_H
#define SNIDAEMON_H

#include <QtDBus/QtDBus>
#include <QStringList>
#include <QDBusAbstractInterface>
class SniDaemon : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierWatcher")

    Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ RegisteredStatusNotifierItems)
    Q_PROPERTY(bool IsStatusNotifierHostRegistered READ IsStatusNotifierHostRegistered)
    Q_PROPERTY(int ProtocolVersion READ ProtocolVersion)
public:
    SniDaemon();
    ~SniDaemon();

    QStringList RegisteredStatusNotifierItems() const;

    bool IsStatusNotifierHostRegistered() const;

    int ProtocolVersion() const;
public Q_SLOTS:
    void RegisterStatusNotifierItem(const QString &service);

    void RegisterStatusNotifierHost(const QString &service);

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    QStringList m_registeredServices;
    QSet<QString> m_statusNotifierHostServices;

Q_SIGNALS:
    void StatusNotifierItemRegistered(const QString &service);
    void StatusNotifierItemUnregistered(const QString &service);
    void StatusNotifierHostRegistered();
    void StatusNotifierHostUnregistered();

protected Q_SLOTS:
    void serviceUnregistered(const QString& name);

};
#endif // SNIDAEMON_H
