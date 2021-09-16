#ifndef WATCHERMANAGER_H
#define WATCHERMANAGER_H

#include <QObject>

class WatcherManager : public QObject
{
    Q_OBJECT
public:
    explicit WatcherManager(QObject *parent = nullptr);


};

#endif // WATCHERMANAGER_H
