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

#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define SHOW_TASKVIEW       "showtaskview"

TaskViewButton::TaskViewButton(){
    setFocusPolicy(Qt::NoFocus);
}
TaskViewButton::~TaskViewButton(){
}
TaskView::TaskView(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    mButton =new TaskViewButton();
    mButton->setStyle(new CustomStyle());
    mButton->setIcon(QIcon::fromTheme("taskview",QIcon("/usr/share/ukui-panel/panel/img/taskview.svg")));
    mButton->setToolTip(tr("Show Taskview"));

    /* hide/show taskview
     * Monitor gsettings to set TaskViewButton size
    */
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        gsettings = new QGSettings(id);
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==SHOW_TASKVIEW)
            realign();
    });

    realign();
}


TaskView::~TaskView()
{
}

/* 隐藏任务视图按钮的逻辑是将buttton的大小设置为０*/
void TaskView::realign()
{
    if(gsettings->get(SHOW_TASKVIEW).toBool())
        mButton->setFixedSize(panel()->panelSize(),panel()->panelSize());
    else
        mButton->setFixedSize(0,0);
    mButton->setIconSize(QSize(panel()->iconSize(),panel()->iconSize()));
}

/* 两种方式可调用任务视图
 * 1.调用Dbus接口
 * 2.调用二进制
*/
void TaskViewButton::mousePressEvent(QMouseEvent *event)
{
    const Qt::MouseButton b = event->button();

    QString object = QString(getenv("DISPLAY"));
    object = object.trimmed().replace(":", "_").replace(".", "_").replace("-", "_");
    object = "/org/ukui/WindowSwitch/display/" + object;
    QDBusInterface interface("org.ukui.WindowSwitch", object,
                             "org.ukui.WindowSwitch",
                             QDBusConnection::sessionBus());
    if (!interface.isValid()) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
    }

    if (Qt::LeftButton == b  && interface.isValid())
    {
        /* Call binary display task view
         * system("ukui-window-switch --show-workspace");
         */

        /*调用远程的value方法*/
        QDBusReply<bool> reply = interface.call("handleWorkspace");
        if (reply.isValid()) {
            if (!reply.value())
                qWarning() << "Handle Workspace View Failed";
        } else {
            qCritical() << "Call Dbus method failed";
        }
    }

    QWidget::mousePressEvent(event);
}
