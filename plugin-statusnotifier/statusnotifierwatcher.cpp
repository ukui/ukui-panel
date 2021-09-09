/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "statusnotifierwatcher.h"
#include <QDebug>
#include <QDBusConnectionInterface>
#include "dbusproperties.h"
static const QString s_watcherServiceName(QStringLiteral("org.kde.StatusNotifierWatcher"));
StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<IconPixmap>("IconPixmap");
    qDBusRegisterMetaType<IconPixmap>();
    qRegisterMetaType<IconPixmapList>("IconPixmapList");
    qDBusRegisterMetaType<IconPixmapList>();
    qRegisterMetaType<ToolTip>("ToolTip");
    qDBusRegisterMetaType<ToolTip>();


    m_statusNotifierWatcher=NULL;
    init();
}

StatusNotifierWatcher::~StatusNotifierWatcher()
{
    QDBusConnection::sessionBus().unregisterService("org.kde.StatusNotifierWatcher");
}

void StatusNotifierWatcher::init(){
    if (QDBusConnection::sessionBus().isConnected()) {
        m_serviceName = "org.kde.StatusNotifierHost-" + QString::number(QCoreApplication::applicationPid());
        QDBusConnection::sessionBus().registerService(m_serviceName);

        QDBusServiceWatcher *watcher = new QDBusServiceWatcher(s_watcherServiceName, QDBusConnection::sessionBus(),
                                                               QDBusServiceWatcher::WatchForOwnerChange, this);
        connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged,
                this, &StatusNotifierWatcher::serviceChange);
        registerWatcher(s_watcherServiceName);
    }
}

void StatusNotifierWatcher::registerWatcher(const QString& service)
{
    //qCDebug(DATAENGINE_SNI)<<"service appeared"<<service;
    if (service == s_watcherServiceName) {

        m_statusNotifierWatcher = new org::kde::StatusNotifierWatcher(s_watcherServiceName, QStringLiteral("/StatusNotifierWatcher"),
                                      QDBusConnection::sessionBus());
        if (m_statusNotifierWatcher->isValid()) {
            m_statusNotifierWatcher->call(QDBus::NoBlock, QStringLiteral("RegisterStatusNotifierHost"), m_serviceName);

            OrgFreedesktopDBusPropertiesInterface  propetriesIface(m_statusNotifierWatcher->service(), m_statusNotifierWatcher->path(), m_statusNotifierWatcher->connection());

            QDBusPendingReply<QDBusVariant> pendingItems = propetriesIface.Get(m_statusNotifierWatcher->interface(), "RegisteredStatusNotifierItems");

            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingItems, this);
            connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
                watcher->deleteLater();
                QDBusReply<QDBusVariant> reply = *watcher;
                QStringList registeredItems = reply.value().variant().toStringList();
                foreach (const QString &service, registeredItems) {
                    newItem(service);
                }
            });

            connect(m_statusNotifierWatcher, &OrgKdeStatusNotifierWatcherInterface::StatusNotifierItemRegistered, this, &StatusNotifierWatcher::serviceRegistered);
            connect(m_statusNotifierWatcher, &OrgKdeStatusNotifierWatcherInterface::StatusNotifierItemUnregistered, this, &StatusNotifierWatcher::serviceUnregistered);

        } else {
            delete m_statusNotifierWatcher;
            m_statusNotifierWatcher = nullptr;
        }
    }
}

void StatusNotifierWatcher::serviceChange(const QString& name, const QString& oldOwner, const QString& newOwner)
{
    qDebug()<< "Service" << name << "status change, old owner:" << oldOwner << "new:" << newOwner;

    if (newOwner.isEmpty()) {
        //unregistered
        unregisterWatcher(name);
    } else if (oldOwner.isEmpty()) {
        //registered
        registerWatcher(name);
    }
}

void StatusNotifierWatcher::unregisterWatcher(const QString& service)
{
    if (service == s_watcherServiceName) {
        qDebug()<< s_watcherServiceName << "disappeared";

        disconnect(m_statusNotifierWatcher, &OrgKdeStatusNotifierWatcherInterface::StatusNotifierItemRegistered, this, &StatusNotifierWatcher::serviceRegistered);
        disconnect(m_statusNotifierWatcher, &OrgKdeStatusNotifierWatcherInterface::StatusNotifierItemUnregistered, this, &StatusNotifierWatcher::serviceUnregistered);

        delete m_statusNotifierWatcher;
        m_statusNotifierWatcher = nullptr;
    }
}

void StatusNotifierWatcher::serviceRegistered(const QString &service)
{
    qDebug() << "Registering"<<service;
    newItem(service);
}

void StatusNotifierWatcher::serviceUnregistered(const QString &service)
{
    qDebug()<<"serviceUnregistered"<<service;
    emit StatusNotifierItemUnregistered(service);
}

void StatusNotifierWatcher::newItem(const QString &service)
{
    qDebug()<<"newItem  ";
    emit StatusNotifierItemRegistered(service);
}

