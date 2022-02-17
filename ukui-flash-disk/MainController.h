/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
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
#ifndef _MAINCONTROL_H_
#define _MAINCONTROL_H_

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>

#include "mainwindow.h"
#include "MacroFile.h"

class MainController : public QObject
{
    Q_OBJECT
public:
    static MainController* self();
    virtual ~MainController();
    int init();
private:
    explicit MainController();

public:
Q_SIGNALS:
    void notifyWnd(QObject* obj, QEvent *event);
    
private:
    static MainController *mSelf;
    MainWindow *m_DiskWindow;
};

#endif //_MAINCONTROL_H_
