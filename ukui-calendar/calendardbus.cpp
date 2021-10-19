#include "calendardbus.h"

#define SERVICE "org.ukui.panel.calendar"
#define PATH "/calendarWidget"
#define INTERFACE "org.ukui.panel.calendar"

CalendarDBus::CalendarDBus(QObject *parent) : QObject(parent)
{
    QDBusConnection con = QDBusConnection::sessionBus();
    con.registerService(SERVICE);
    con.registerObject(PATH,INTERFACE,this,QDBusConnection::ExportAllSlots);
}

void CalendarDBus::ShowCalendar(){
    qDebug()<<"CalendarDBus is call";
    Q_EMIT ShowCalendarWidget();
}
