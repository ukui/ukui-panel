#ifndef WATCHERMANAGER_H
#define WATCHERMANAGER_H

#include <QObject>
#include "filewatcher/filewatcher.h"

#include "dbus-server/server.h"
#include "dbus-server/dbus-adaptor.h"
#include "pin-totaskbar/pintotaskbar.h"
#include "pin-totaskbar/taskbar-dbus-adaptor.h"

class WatcherManager : public QObject
{
    Q_OBJECT
public:
    explicit WatcherManager(QObject *parent = nullptr);
    ~WatcherManager();
private:
    void register_dbus();

    PinToTaskbar* taskbar_dbus;
};

#endif // WATCHERMANAGER_H
