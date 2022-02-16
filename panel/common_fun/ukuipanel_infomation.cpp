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

#include "ukuipanel_infomation.h"
//Qt
#include <QDebug>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_SIZE_KEY      "panelsize"

QString panelPositionTransform(int p)
{
    switch (p){
    case 1:
        return "Top";
        break;
    case 2:
        return "Left";
        break;
    case 3:
        return "Right";
        break;
    default:
        return "Bottom";
        break;
    }

}
UKuiPanelInformation::UKuiPanelInformation(QObject *parent) : QObject(parent)
{
    QDBusConnection::sessionBus().connect(QString(), QString( "/panel/position"),
                                          "org.ukui.panel",
                                          "UKuiPanelPosition",
                                          this,
                                          SLOT(setPanelInformation(int,int,int,int,int,int))
                                          );
}

void UKuiPanelInformation::setPanelInformation(int x, int y, int width, int height, int size, int position)
{
    screen_x=x;
    screen_y=y;
    screen_width=width;
    screen_height=height;
    panelsize=size;
    panelposition=position;

    QDBusMessage message = QDBusMessage::createSignal("/panel/position", "org.ukui.panel", "PrimaryScreenAvailiableGeometryChanged");
    QList<QVariant> args;
    args.append(screen_x);
    args.append(screen_y);
    args.append(screen_width);
    args.append(screen_height);
    message.setArguments(args);
    QDBusConnection::sessionBus().send(message);

    QDBusMessage message_refresh = QDBusMessage::createSignal("/panel/position", "org.ukui.panel", "PanelGeometryRefresh");
    QDBusConnection::sessionBus().send(message_refresh);
}
QVariantList UKuiPanelInformation::GetPrimaryScreenGeometry()
{
    int available_primary_screen_x;int available_primary_screen_y ; int available_primary_screen_width; int available_primary_screen_height;int available_panel_position;
    QVariantList vlist;
    switch(panelposition){
    case 0:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 1:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y+panelsize;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 2:
        available_primary_screen_x=screen_x + panelsize;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelsize;
        available_primary_screen_height=screen_height;
        break;
    case 3:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelsize;
        available_primary_screen_height=screen_height;
        break;
    default:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    }
    vlist<<available_primary_screen_x<<available_primary_screen_y<<available_primary_screen_width<<available_primary_screen_height<<panelposition;
    return vlist;
}

QVariantList UKuiPanelInformation::GetPrimaryScreenAvailableGeometry()
{
    int available_primary_screen_x;int available_primary_screen_y ; int available_primary_screen_width; int available_primary_screen_height;int available_panel_position;
    QVariantList vlist;
    switch(panelposition){
    case 0:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 1:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y+panelsize;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    case 2:
        available_primary_screen_x=screen_x + panelsize;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelsize;
        available_primary_screen_height=screen_height;
        break;
    case 3:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width-panelsize;
        available_primary_screen_height=screen_height;
        break;
    default:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height-panelsize;
        break;
    }
    vlist<<available_primary_screen_x<<available_primary_screen_y<<available_primary_screen_width<<available_primary_screen_height<<panelposition;
    return vlist;
}

QVariantList UKuiPanelInformation::GetPrimaryScreenPhysicalGeometry()
{
    int available_primary_screen_x;int available_primary_screen_y ; int available_primary_screen_width; int available_primary_screen_height;int available_panel_position;
    QVariantList vlist;
    switch(panelposition){
    case 1:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height;
        break;
    case 2:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height;
        break;
    case 3:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height;
        break;
    default:
        available_primary_screen_x=screen_x;
        available_primary_screen_y=screen_y;
        available_primary_screen_width=screen_width;
        available_primary_screen_height=screen_height;
        break;
    }
    vlist<<available_primary_screen_x<<available_primary_screen_y<<available_primary_screen_width<<available_primary_screen_height<<panelsize<<panelposition<<panelPositionTransform(panelposition);
    return vlist;
}


QString UKuiPanelInformation::GetPanelPosition()
{
    QString str="bottom";
    return str;
}
