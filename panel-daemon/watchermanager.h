#ifndef WATCHERMANAGER_H
#define WATCHERMANAGER_H

#include <QObject>
#include "filewatcher/filewatcher.h"

class WatcherManager : public QObject
{
    Q_OBJECT
public:
    explicit WatcherManager(QObject *parent = nullptr);
    ~WatcherManager();
private:
    void register_dbus();


};

#endif // WATCHERMANAGER_H
