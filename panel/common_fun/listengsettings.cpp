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
#include "listengsettings.h"
//Qt
#include <QDebug>

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_SIZE_KEY      "panelsize"

ListenGsettings::ListenGsettings()
{
    const QByteArray id(PANEL_SETTINGS);
    panel_gsettings = new QGSettings(id);
    QObject::connect(panel_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key == PANEL_POSITION_KEY){
            emit panelpositionchanged(panel_gsettings->get(PANEL_POSITION_KEY).toInt());
        }
        if(key == ICON_SIZE_KEY){
            emit iconsizechanged(panel_gsettings->get(ICON_SIZE_KEY).toInt());
        }
        if(key == PANEL_SIZE_KEY){
            emit panelsizechanged(panel_gsettings->get(PANEL_SIZE_KEY).toInt());
        }
    });
}
