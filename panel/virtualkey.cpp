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

#include "virtualkey.h"
#include <QDesktopWidget>
#include <QApplication>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"


/*
 * 虚拟按键
 * 实现的功能是在平板模式并且隐藏任务栏的情况下
 * 提供屏幕虚拟按键，用以提醒用户唤起任务栏 （类似部分安卓或IOS的全面屏时候的底部虚拟按键）
 * 应实现的功能
 * 1.监听任务栏隐藏和平板模式开启 为真则显示虚拟按键，为假则隐藏虚拟按键
 * 2.监听虚拟键盘的开启或关闭  为真则显示虚拟按键，为假则隐藏虚拟按键
 * 备注：
 * 根据Interface Segregation Principle（接口隔离原则），
 * 本功能相对任务栏的主体功能是完全分离的，
 * 因此隐藏及显示相关逻辑应在VirtualKey中进行，而不应该放在ukuipanel.cpp或其他类似文件中进行
 *
*/
VirtualKey::VirtualKey(QToolButton *parent)
    : QToolButton(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    QDesktopWidget * desktop = QApplication::desktop();

    const QByteArray id(PANEL_SETTINGS);
    gsettings = new QGSettings(id);
    setPosition();

    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANEL_POSITION_KEY){
            setPosition();
        }
    });
}

VirtualKey::~VirtualKey()
{
}

void VirtualKey::setPosition()
{
    switch (gsettings->get(PANEL_POSITION_KEY).toInt()) {
    case 1:
        setText("-");
        this->move(QPoint(QApplication::desktop()->screen()->rect().center().x(),30));
        break;
    case 2:
        setText(">");
        this->move(QPoint(30,QApplication::desktop()->screen()->rect().center().y()));
        break;
    case 3:
        setText("<");
        this->move(QPoint(QApplication::desktop()->screen()->width()-30,QApplication::desktop()->screen()->rect().center().y()));
        break;
    default:
        setText("^");
        this->move(QPoint(QApplication::desktop()->screen()->rect().center().x(),QApplication::desktop()->screen()->height()-30));
        break;
    }
}
