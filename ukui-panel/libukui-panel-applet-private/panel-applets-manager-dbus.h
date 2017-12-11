/*
 * panel-applets-manager-dbus.h
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2010 Vincent Untz <vuntz@gnome.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __PANEL_APPLETS_MANAGER_DBUS_H__
#define __PANEL_APPLETS_MANAGER_DBUS_H__

#include <panel-applets-manager.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PANEL_TYPE_APPLETS_MANAGER_DBUS			(ukui_panel_applets_manager_dbus_get_type ())
#define UKUI_PANEL_APPLETS_MANAGER_DBUS(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), PANEL_TYPE_APPLETS_MANAGER_DBUS, UkuiPanelAppletsManagerDBus))
#define UKUI_PANEL_APPLETS_MANAGER_DBUS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), PANEL_TYPE_APPLETS_MANAGER_DBUS, UkuiPanelAppletsManagerDBusClass))
#define PANEL_IS_APPLETS_MANAGER_DBUS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PANEL_TYPE_APPLETS_MANAGER_DBUS))
#define PANEL_IS_APPLETS_MANAGER_DBUS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PANEL_TYPE_APPLETS_MANAGER_DBUS))
#define UKUI_PANEL_APPLETS_MANAGER_DBUS_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), PANEL_TYPE_APPLETS_MANAGER_DBUS, UkuiPanelAppletsManagerDBusClass))

typedef struct _UkuiPanelAppletsManagerDBus		UkuiPanelAppletsManagerDBus;
typedef struct _UkuiPanelAppletsManagerDBusClass	UkuiPanelAppletsManagerDBusClass;
typedef struct _UkuiPanelAppletsManagerDBusPrivate	UkuiPanelAppletsManagerDBusPrivate;

struct _UkuiPanelAppletsManagerDBusClass {
	UkuiPanelAppletsManagerClass parent_class;
};

struct _UkuiPanelAppletsManagerDBus {
	UkuiPanelAppletsManager parent;

	/*< private > */
	UkuiPanelAppletsManagerDBusPrivate *priv;
};

GType ukui_panel_applets_manager_dbus_get_type (void);

#ifdef __cplusplus
}
#endif

#endif /* __PANEL_APPLETS_MANAGER_DBUS_H__ */
