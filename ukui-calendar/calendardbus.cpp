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
