#ifndef SNIDEAMO_H
#define SNIDEAMO_H

#include <QtDBus/QtDBus>
#include <QStringList>
#include <QDBusAbstractInterface>
class SniDeamo : public QObject,protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierWatcher")

    Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ RegisteredStatusNotifierItems)
    Q_PROPERTY(bool IsStatusNotifierHostRegistered READ IsStatusNotifierHostRegistered)
    Q_PROPERTY(int ProtocolVersion READ ProtocolVersion)
public:
    SniDeamo();
    ~SniDeamo();

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
#endif // SNIDEAMO_H
