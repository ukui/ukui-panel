#ifndef DATEWATCHER_H
#define DATEWATCHER_H

#include <QObject>

class DateWatcher : public QObject
{
    Q_OBJECT
public:
    explicit DateWatcher(QObject *parent = nullptr);


};

#endif // DATEWATCHER_H
