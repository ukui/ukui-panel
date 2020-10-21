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
#include <QPalette>

#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define SHOW_TASKVIEW       "showtaskview"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"
#define ICON_COLOR_LOGHT      255
#define ICON_COLOR_DRAK       0

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

    const QByteArray style_id(ORG_UKUI_STYLE);
    QStringList stylelist;
    stylelist<<STYLE_NAME_KEY_DARK<<STYLE_NAME_KEY_BLACK<<STYLE_NAME_KEY_DEFAULT;
    if(QGSettings::isSchemaInstalled(style_id)){
        style_settings = new QGSettings(style_id);
        if(stylelist.contains(style_settings->get(STYLE_NAME).toString())){
            icon_color=ICON_COLOR_LOGHT;
        }else{
            icon_color=ICON_COLOR_DRAK;
        }
        mButton->setIcon(HighLightEffect::drawSymbolicColoredIcon(QIcon::fromTheme("/usr/share/ukui-panel/plugin-taskview/img/taskview.svg")));
        }
    connect(style_settings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==STYLE_NAME){
            if(stylelist.contains(style_settings->get(STYLE_NAME).toString())){
                icon_color=ICON_COLOR_LOGHT;
            }
            else
                icon_color=ICON_COLOR_DRAK;
            mButton->setIcon(HighLightEffect::drawSymbolicColoredIcon(QIcon::fromTheme("/usr/share/ukui-panel/plugin-taskview/img/taskview.svg")));
        }
    });

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
    QDBusInterface interface("org.ukui.WindowSwitch", "/org/ukui/WindowSwitch",
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

QPixmap TaskView::drawSymbolicColoredPixmap(const QPixmap &source)
{
    QColor gray(128,128,128);
    QColor standard (31,32,34);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<255 && qAbs(color.green()-gray.green())<255 && qAbs(color.blue()-gray.blue())<255) {
//                    qDebug()<<"tray_icon_colopr"<<icon_color;
                    color.setRed(icon_color);
                    color.setGreen(icon_color);
                    color.setBlue(icon_color);
                    img.setPixelColor(x, y, color);
                }
                else if(qAbs(color.red()-standard.red())<20 && qAbs(color.green()-standard.green())<20 && qAbs(color.blue()-standard.blue())<20)
                {
                    color.setRed(icon_color);
                    color.setGreen(icon_color);
                    color.setBlue(icon_color);
                    img.setPixelColor(x, y, color);
                }
                else
                {
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}
