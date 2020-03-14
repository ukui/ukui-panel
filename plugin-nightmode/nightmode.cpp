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


#include "nightmode.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include "../panel/customstyle.h"

#define NIGHT_MODE_LIGHT "light"
#define NIGHE_MODE_NIGHT "night"
NightMode::NightMode(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    realign();
    mButton.setStyle(new CustomStyle());
//    mButton.setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode.svg"));
}


NightMode::~NightMode(){
}

void NightMode::realign()
{
    mButton.setFixedSize(panel()->panelSize(),panel()->panelSize());
    mButton.setIconSize(QSize(panel()->iconSize(),panel()->iconSize()));
}

NightModeButton::NightModeButton(){
    gsettings= new QGSettings("org.ukui.panel.plugins", "", this);
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
}
NightModeButton::~NightModeButton(){
}
void NightModeButton::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();
    if (Qt::LeftButton == b){
        QString mode=gsettings->get("nightmode").toString();
        if(mode==NIGHE_MODE_NIGHT){
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        gsettings->set("nightmode", NIGHT_MODE_LIGHT);
        }
        else{
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
        gsettings->set("nightmode", NIGHE_MODE_NIGHT);
        }
    }
    QWidget::mousePressEvent(event);
}

