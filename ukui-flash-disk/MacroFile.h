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
#ifndef _MACROFILE_H_
#define _MACROFILE_H_
typedef enum {downPosition = 0,upPosition,leftPosition,rightPosition}Suit;

#define SERVICE_NAME_SIZE 64
#define DistanceToPanel 2
#define SmallPanelSize 46
#define UKUI_FLASH_DISK_PATH "/"
#define UKUI_FLASH_DISK_SERVICE "com.ukuiflashdisk.hotel"
#define UKUI_FLASH_DISK_INTERFACE "com.flshdisk.hotel"

#define NORMALDEVICE 0
#define DATADEVICE 1
#define OCCUPYDEVICE 2
#define GPARTEDINTERFACE 3
#define DISTANCEPADDING 6
#define DISTANCEMEND 2

#define FLASHDISKITEM_TITLE_HEIGHT  45
#define FLASHDISKITEM_CONTENT_HEIGHT  45

/**
 * QT主题
 */
#define THEME_QT_SCHEMA "org.ukui.style"
//opacity setting
#define THEME_QT_TRANS "org.ukui.control-center.personalise"
#define FONT_SIZE "system-font-size"
#define MODE_QT_KEY "style-name"
#define AUTOLOAD "org.ukui.flash-disk.autoload"
#define IFAUTOLOAD "ifautoload"
#endif //_MACROFILE_H_
