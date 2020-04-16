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


#include "taskview.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QToolBar>
#include <QStyle>
#include <QDBusInterface>
#include <QDBusReply>
#include "../panel/customstyle.h"
#include <QPalette>
#include <QToolTip>
TaskViewButton::TaskViewButton(){
}
TaskViewButton::~TaskViewButton(){
}
TaskView::TaskView(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    mButton =new TaskViewButton();
    mButton->setStyle(new CustomStyle());
    mButton->setIcon(QIcon("/usr/share/ukui-panel/panel/img/taskview.svg"));
    mButton->paintTooltipStyle();
    mButton->setToolTip(tr("taskviewWindow"));
    realign();
}


TaskView::~TaskView()
{
}


void TaskView::realign()
{
    mButton->setFixedSize(panel()->panelSize(),panel()->panelSize());
    mButton->setIconSize(QSize(panel()->iconSize(),panel()->iconSize()));
}

void TaskViewButton::mousePressEvent(QMouseEvent *event)
{
    const Qt::MouseButton b = event->button();

    if (Qt::LeftButton == b)
    {
        //Two ways to call the taskview
        //system("ukui-window-switch --show-workspace");
        QDBusInterface interface("org.ukui.WindowSwitch", "/org/ukui/WindowSwitch",
                                    "org.ukui.WindowSwitch",
                                    QDBusConnection::sessionBus());
        if (!interface.isValid()) {
            qCritical() << QDBusConnection::sessionBus().lastError().message();
        }
        //调用远程的value方法
        QDBusReply<bool> reply = interface.call("handleWorkspace");
        if (reply.isValid()&& interface.isValid()) {
            if (!reply.value())
                qWarning() << "Handle Workspace View Failed";
        } else {
            qCritical() << "Call Dbus method failed";
        }
    }

    QWidget::mousePressEvent(event);
}


void TaskViewButton::mouseMoveEvent(QMouseEvent *e)
{
    qDebug()<<"mouse move enent";
    QWidget::mouseMoveEvent(e);
}

void TaskViewButton::contextMenuEvent(QContextMenuEvent *event){
}
