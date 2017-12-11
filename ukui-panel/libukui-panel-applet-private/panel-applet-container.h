/*
 * panel-applet-container.h: a container for applets.
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef __PANEL_APPLET_CONTAINER_H__
#define __PANEL_APPLET_CONTAINER_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include "panel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANEL_TYPE_APPLET_CONTAINER            (ukui_panel_applet_container_get_type ())
#define UKUI_PANEL_APPLET_CONTAINER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PANEL_TYPE_APPLET_CONTAINER, UkuiPanelAppletContainer))
#define UKUI_PANEL_APPLET_CONTAINER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), PANEL_TYPE_APPLET_CONTAINER, UkuiPanelAppletContainerClass))
#define PANEL_IS_APPLET_CONTAINER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PANEL_TYPE_APPLET_CONTAINER))
#define PANEL_IS_APPLET_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PANEL_TYPE_APPLET_CONTAINER))
#define UKUI_PANEL_APPLET_CONTAINER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PANEL_TYPE_APPLET_CONTAINER, UkuiPanelAppletContainerClass))

#define UKUI_PANEL_APPLET_CONTAINER_ERROR           (ukui_panel_applet_container_error_quark())

typedef enum {
	UKUI_PANEL_APPLET_CONTAINER_INVALID_APPLET,
	UKUI_PANEL_APPLET_CONTAINER_INVALID_CHILD_PROPERTY
} UkuiPanelAppletContainerError;

typedef struct _UkuiPanelAppletContainer        UkuiPanelAppletContainer;
typedef struct _UkuiPanelAppletContainerClass   UkuiPanelAppletContainerClass;
typedef struct _UkuiPanelAppletContainerPrivate UkuiPanelAppletContainerPrivate;

struct _UkuiPanelAppletContainer {
	GtkEventBox parent;

	UkuiPanelAppletContainerPrivate *priv;
};

struct _UkuiPanelAppletContainerClass {
	GtkEventBoxClass parent_class;

	/* Signals */
	void (*applet_broken)          (UkuiPanelAppletContainer *container);
	void (*applet_move)            (UkuiPanelAppletContainer *container);
	void (*applet_remove)          (UkuiPanelAppletContainer *container);
	void (*applet_lock)            (UkuiPanelAppletContainer *container,
					gboolean              locked);
	void (*child_property_changed) (UkuiPanelAppletContainer *container,
					const gchar          *property_name,
					GVariant             *value);
};

GType      ukui_panel_applet_container_get_type                (void) G_GNUC_CONST;
GQuark     ukui_panel_applet_container_error_quark             (void) G_GNUC_CONST;
GtkWidget *ukui_panel_applet_container_new                     (void);


void       ukui_panel_applet_container_add                     (UkuiPanelAppletContainer *container,
							   GdkScreen            *screen,
							   const gchar          *iid,
							   GCancellable        *cancellable,
							   GAsyncReadyCallback  callback,
							   gpointer             user_data,
							   GVariant            *properties);
gboolean   ukui_panel_applet_container_add_finish              (UkuiPanelAppletContainer *container,
							   GAsyncResult         *result,
							   GError              **error);
void       ukui_panel_applet_container_child_popup_menu        (UkuiPanelAppletContainer *container,
							   guint                 button,
							   guint32               timestamp,
							   GCancellable         *cancellable,
							   GAsyncReadyCallback   callback,
							   gpointer              user_data);
gboolean   ukui_panel_applet_container_child_popup_menu_finish (UkuiPanelAppletContainer *container,
							   GAsyncResult         *result,
							   GError              **error);

void       ukui_panel_applet_container_child_set               (UkuiPanelAppletContainer *container,
							   const gchar          *property_name,
							   const GVariant       *value,
							   GCancellable         *cancellable,
							   GAsyncReadyCallback   callback,
							   gpointer              user_data);
gboolean   ukui_panel_applet_container_child_set_finish        (UkuiPanelAppletContainer *container,
							   GAsyncResult         *result,
							   GError              **error);
void       ukui_panel_applet_container_child_get               (UkuiPanelAppletContainer *container,
							   const gchar          *property_name,
							   GCancellable         *cancellable,
							   GAsyncReadyCallback   callback,
							   gpointer              user_data);
GVariant  *ukui_panel_applet_container_child_get_finish        (UkuiPanelAppletContainer *container,
							   GAsyncResult         *result,
							   GError              **error);

#ifdef __cplusplus
}
#endif

#endif /* __PANEL_APPLET_CONTAINER_H__ */
