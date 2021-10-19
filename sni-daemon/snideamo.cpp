#include "snideamo.h"
#include "statusnotifieritem_interface.h"
SniDeamo::SniDeamo()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject(QStringLiteral("/StatusNotifierWatcher"),"org.kde.StatusNotifierWatcher",this,QDBusConnection::ExportAllContents);
    dbus.registerService(QStringLiteral("org.kde.StatusNotifierWatcher"));

    m_serviceWatcher = new QDBusServiceWatcher(this);
    m_serviceWatcher->setConnection(dbus);
    m_serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);

    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &SniDeamo::serviceUnregistered);

}

SniDeamo::~SniDeamo()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(QStringLiteral("org.kde.StatusNotifierWatcher"));
}


void SniDeamo::serviceUnregistered(const QString& name)
{
    qDebug()<<"Service "<< name << "unregistered";
    m_serviceWatcher->removeWatchedService(name);

    QString match = name + QLatin1Char('/');
    QStringList::Iterator it = m_registeredServices.begin();
    qDebug()<<m_registeredServices;
    while (it != m_registeredServices.end()) {
        if (it->startsWith(match)) {
            QString name = *it;
            it = m_registeredServices.erase(it);
            emit StatusNotifierItemUnregistered(name);
        } else {
            ++it;
        }
    }

    if (m_statusNotifierHostServices.contains(name)) {
        m_statusNotifierHostServices.remove(name);
        emit StatusNotifierHostUnregistered();
    }
}

void SniDeamo::RegisterStatusNotifierItem(const QString &serviceOrPath)
{
    QString service;
    QString path;
    if (serviceOrPath.startsWith(QLatin1Char('/'))) {
        service = message().service();
        path = serviceOrPath;
    } else {
        service = serviceOrPath;
        path = QStringLiteral("/StatusNotifierItem");
    }
    QString notifierItemId = service + path;
    if (m_registeredServices.contains(notifierItemId)) {
        return;
    }
    m_serviceWatcher->addWatchedService(service);
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(service).value()) {
        //check if the service has registered a SystemTray object
        org::kde::StatusNotifierItem trayclient(service, path, QDBusConnection::sessionBus());
        if (trayclient.isValid()) {
            qDebug() << "Registering" << notifierItemId << "to system tray";
            m_registeredServices.append(notifierItemId);
            emit StatusNotifierItemRegistered(notifierItemId);
        } else {
            m_serviceWatcher->removeWatchedService(service);
        }
    } else {
        m_serviceWatcher->removeWatchedService(service);
    }
}

void SniDeamo::RegisterStatusNotifierHost(const QString &service)
{
    if (service.contains(QLatin1String("org.kde.StatusNotifierHost-")) &&
        QDBusConnection::sessionBus().interface()->isServiceRegistered(service).value() &&
        !m_statusNotifierHostServices.contains(service)) {
        qDebug()<<"Registering"<<service<<"as system tray";

        m_statusNotifierHostServices.insert(service);
        m_serviceWatcher->addWatchedService(service);
        emit StatusNotifierHostRegistered();
    }
}

QStringList SniDeamo::RegisteredStatusNotifierItems() const
{
    return m_registeredServices;
}

bool SniDeamo::IsStatusNotifierHostRegistered() const
{
    return !m_statusNotifierHostServices.isEmpty();
}

int SniDeamo::ProtocolVersion() const
{
    return 0;
}
