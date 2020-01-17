/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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



#include "startmenu.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>

StartMenu::StartMenu(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    realign();
    mCapturing = false;
    connect(&mButton, SIGNAL(clicked()), this, SLOT(captureMouse()));




}


StartMenu::~StartMenu()
{
}


void StartMenu::realign()
{
    mButton.setFixedSize(46,panel()->panelSize());
    mButton.setStyleSheet(
                //正常状态样式
                "QToolButton{"
                /*"background-color:rgba(100,225,100,80%);"//背景色（也可以设置图片）*/
                "qproperty-icon:url(/usr/share/ukui-panel/plugin-startmenu/img/startmenu.svg);"
                "qproperty-iconSize:40px 40px;"
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-width:0px;"                     //边框宽度像素
                "border-radius:0px;"                   //边框圆角半径像素
                "border-color:rgba(255,255,255,30);"    //边框颜色
                "font:SimSun 14px;"                       //字体，字体大小
                "color:rgba(0,0,0,100);"                //字体颜色
                "padding:0px;"                          //填衬
                "border-bottom-style:solid"
                "}"
                //鼠标悬停样式
                "QToolButton:hover{"
                "background-color:rgba(190,216,239,20%);"
                "}"
                //鼠标按下样式
                "QToolButton:pressed{"
                "background-color:rgba(190,216,239,12%);"
                "}"
                );

}

void StartMenu::captureMouse()
{
    if(QFileInfo::exists(QString("/usr/bin/ukui-menu")))
    {
    QProcess *process =new QProcess(this);
    process->startDetached("/usr/bin/ukui-menu");
    }
    else{qDebug()<<"not find /usr/bin/ukui-start-menu"<<endl;}
}
