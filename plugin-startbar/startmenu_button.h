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

#ifndef STARTMENUBUTTON_H
#define STARTMENUBUTTON_H

#include <QToolButton>
#include <QTimer>
#include <QProcess>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QtDBus/QtDBus>
#include <QMouseEvent>
#include <QContextMenuEvent>

#include "../panel/iukuipanelplugin.h"
#include "../panel/customstyle.h"

class StartMenuButton : public QToolButton
{
    Q_OBJECT
public:
    StartMenuButton(IUKUIPanelPlugin *plugin,QWidget* parent = 0);
    ~StartMenuButton();
    void realign();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QMenu *rightPressMenu;
    IUKUIPanelPlugin * m_plugin;
    QString version;
    QWidget *m_parent;

    void getOsRelease();
    QString getCanHibernateResult();
    bool hasMultipleUsers();

private slots:
    void ScreenServer();
    void SessionSwitch();
    void SessionLogout();
    void SessionReboot();
    void TimeShutdown();
    void SessionShutdown();
    void SessionSuspend();
    void SessionHibernate();
};

#endif // STARTMENUBUTTON_H
