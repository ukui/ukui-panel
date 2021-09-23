#ifndef CALENDARDBUS_H
#define CALENDARDBUS_H

#include <QObject>
#include <QtDBus>
class CalendarDBus : public QObject
{
    Q_OBJECT
public:
    explicit CalendarDBus(QObject *parent = nullptr);

public Q_SLOTS:
    void ShowCalendar();

Q_SIGNALS:
    void ShowCalendarWidget();


};

#endif // CALENDARDBUS_H
