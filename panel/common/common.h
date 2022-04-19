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

#ifndef COMMON_H
#define COMMON_H

#define PANEL_DEFAULT_SIZE 32
#define PANEL_MINIMUM_SIZE 16
#define PANEL_MAXIMUM_SIZE 200
#define PANEL_HIDE_SIZE 4

#define PANEL_DEFAULT_ICON_SIZE 22
#define PANEL_DEFAULT_LINE_COUNT 1

#define PANEL_DEFAULT_BACKGROUND_COLOR "#CCCCCC"

#define PANEL_HIDE_DELAY 500
#define PANEL_HIDE_FIRST_TIME (5000 - PANEL_HIDE_DELAY)

#define PANEL_SHOW_DELAY 0

#define SETTINGS_SAVE_DELAY 1000

// Config keys and groups
#define CFG_KEY_SCREENNUM          "desktop"
#define CFG_KEY_POSITION           "position"
#define CFG_KEY_LINECNT            "lineCount"
#define CFG_KEY_LENGTH             "width"
#define CFG_KEY_PERCENT            "width-percent"
#define CFG_KEY_ALIGNMENT          "alignment"
#define CFG_KEY_RESERVESPACE       "reserve-space"
#define CFG_KEY_PLUGINS            "plugins"
#define CFG_KEY_HIDABLE            "hidable"
#define CFG_KEY_VISIBLE_MARGIN     "visible-margin"
#define CFG_KEY_ANIMATION          "animation-duration"
#define CFG_KEY_SHOW_DELAY         "show-delay"
#define CFG_KEY_LOCKPANEL          "lockPanel"
#define CFG_KEY_PLUGINS_PC         "plugins-pc"
#define CFG_KEY_PLUGINS_PAD         "plugins-pad"

#define GSETTINGS_SCHEMA_SCREENSAVER "org.mate.interface"
#define KEY_MODE "gtk-theme"

#define PANEL_SIZE_LARGE  92
#define PANEL_SIZE_MEDIUM 70
#define PANEL_SIZE_SMALL  46
#define ICON_SIZE_LARGE   64
#define ICON_SIZE_MEDIUM  48
#define ICON_SIZE_SMALL   32
#define PANEL_SIZE_LARGE_V 70
#define PANEL_SIZE_MEDIUM_V 62
#define PANEL_SIZE_SMALL_V 47
#define POPUP_BORDER_SPACING 4
#define SETTINGS_SAVE_DELAY 1000

#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_SIZE_KEY      "panelsize"
#define ICON_SIZE_KEY       "iconsize"
#define PANEL_POSITION_KEY  "panelposition"
#define SHOW_TASKVIEW       "showtaskview"
#define SHOW_NIGHTMODE      "shownightmode"

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"

#define UKUI_SERVICE        "org.gnome.SessionManager"
#define UKUI_PATH           "/org/gnome/SessionManager"
#define UKUI_INTERFACE      "org.gnome.SessionManager"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"

#endif // COMMON_H

