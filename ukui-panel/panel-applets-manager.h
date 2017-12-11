/*
 * panel-applets-manager.h
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

#ifndef __PANEL_APPLETS_MANAGER_H__
#define __PANEL_APPLETS_MANAGER_H__

#include <glib-object.h>

#include "panel-applet-frame.h"
#include "panel-applet-info.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANEL_TYPE_APPLETS_MANAGER		(ukui_panel_applets_manager_get_type ())
#define UKUI_PANEL_APPLETS_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PANEL_TYPE_APPLETS_MANAGER, UkuiPanelAppletsManager))
#define UKUI_PANEL_APPLETS_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PANEL_TYPE_APPLETS_MANAGER, UkuiPanelAppletsManagerClass))
#define PANEL_IS_APPLETS_MANAGER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PANEL_TYPE_APPLETS_MANAGER))
#define PANEL_IS_APPLETS_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PANEL_TYPE_APPLETS_MANAGER))
#define UKUI_PANEL_APPLETS_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), PANEL_TYPE_APPLETS_MANAGER, UkuiPanelAppletsManagerClass))

/**
 * UKUI_PANEL_APPLETS_MANAGER_EXTENSION_POINT_NAME:
 *
 * Extension point for #UkuiPanelAppletsManager functionality.
 **/
#define UKUI_PANEL_APPLETS_MANAGER_EXTENSION_POINT_NAME "ukui-panel-applets-manager"

typedef struct _UkuiPanelAppletsManager		UkuiPanelAppletsManager;
typedef struct _UkuiPanelAppletsManagerClass	UkuiPanelAppletsManagerClass;

struct _UkuiPanelAppletsManagerClass {
	GObjectClass parent_class;

	GList *            (*get_applets)           (UkuiPanelAppletsManager  *manager);

	gboolean           (*factory_activate)      (UkuiPanelAppletsManager  *manager,
						     const gchar          *iid);
	gboolean           (*factory_deactivate)    (UkuiPanelAppletsManager  *manager,
						     const gchar          *iid);

	UkuiPanelAppletInfo  * (*get_applet_info)       (UkuiPanelAppletsManager  *manager,
						     const gchar          *iid);

	UkuiPanelAppletInfo  * (*get_applet_info_from_old_id) (UkuiPanelAppletsManager  *manager,
							   const gchar          *iid);

	gboolean           (*load_applet)           (UkuiPanelAppletsManager         *manager,
						     const gchar                 *iid,
						     UkuiPanelAppletFrameActivating  *frame_act);

	GtkWidget        * (*get_applet_widget)     (UkuiPanelAppletsManager         *manager,
	                                             const gchar                 *iid,
	                                             guint                        uid);
};

struct _UkuiPanelAppletsManager {
	GObject parent;
};

GType             ukui_panel_applets_manager_get_type                    (void);

GList            *ukui_panel_applets_manager_get_applets                 (void);

gboolean          ukui_panel_applets_manager_factory_activate            (const gchar     *iid);
void              ukui_panel_applets_manager_factory_deactivate          (const gchar     *iid);

UkuiPanelAppletInfo  *ukui_panel_applets_manager_get_applet_info             (const gchar     *iid);
UkuiPanelAppletInfo  *ukui_panel_applets_manager_get_applet_info_from_old_id (const gchar     *iid);

gboolean          ukui_panel_applets_manager_load_applet                 (const gchar                *iid,
								     UkuiPanelAppletFrameActivating *frame_act);

GtkWidget        *ukui_panel_applets_manager_get_applet_widget           (const gchar     *iid,
                                                                     guint            uid);

#ifdef __cplusplus
}
#endif

#endif /* __PANEL_APPLETS_MANAGER_H__ */
