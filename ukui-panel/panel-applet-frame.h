/*
 * panel-applet-frame.h: panel side container for applets
 *
 * Copyright (C) 2001 - 2003 Sun Microsystems, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * Authors:
 *	Mark McLoughlin <mark@skynet.ie>
 */

#ifndef __PANEL_APPLET_FRAME_H__
#define __PANEL_APPLET_FRAME_H__

#include <gtk/gtk.h>

#include "panel-widget.h"
#include "applet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PANEL_TYPE_APPLET_FRAME         (ukui_panel_applet_frame_get_type ())
#define UKUI_PANEL_APPLET_FRAME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PANEL_TYPE_APPLET_FRAME, UkuiPanelAppletFrame))
#define UKUI_PANEL_APPLET_FRAME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PANEL_TYPE_APPLET_FRAME, UkuiPanelAppletFrameClass))
#define PANEL_IS_APPLET_FRAME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PANEL_TYPE_APPLET_FRAME))
#define PANEL_IS_APPLET_FRAME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PANEL_TYPE_APPLET_FRAME))
#define UKUI_PANEL_APPLET_FRAME_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PANEL_TYPE_APPLET_FRAME, UkuiPanelAppletFrameClass))

typedef struct _UkuiPanelAppletFrame        UkuiPanelAppletFrame;
typedef struct _UkuiPanelAppletFrameClass   UkuiPanelAppletFrameClass;
typedef struct _UkuiPanelAppletFramePrivate UkuiPanelAppletFramePrivate;

struct _UkuiPanelAppletFrameClass {
        GtkEventBoxClass parent_class;

	void     (*init_properties)       (UkuiPanelAppletFrame    *frame);

	void     (*sync_menu_state)       (UkuiPanelAppletFrame    *frame,
					   gboolean             movable,
					   gboolean             removable,
					   gboolean             lockable,
					   gboolean             locked,
					   gboolean             locked_down);

	void     (*popup_menu)            (UkuiPanelAppletFrame    *frame,
					   guint                button,
					   guint32              timestamp);

	void     (*change_orientation)    (UkuiPanelAppletFrame    *frame,
					   PanelOrientation     orientation);

	void     (*change_size)           (UkuiPanelAppletFrame    *frame,
					   guint                size);

	void     (*change_background)     (UkuiPanelAppletFrame    *frame,
					   PanelBackgroundType  type);
};

struct _UkuiPanelAppletFrame {
	GtkEventBox parent;

        UkuiPanelAppletFramePrivate  *priv;
};

GType ukui_panel_applet_frame_get_type           (void) G_GNUC_CONST;

void  ukui_panel_applet_frame_create             (PanelToplevel       *toplevel,
					     int                  position,
					     const char          *iid);

void  ukui_panel_applet_frame_load_from_gsettings    (PanelWidget         *panel_widget,
					     gboolean             locked,
					     int                  position,
					     const char          *id);

void  ukui_panel_applet_frame_sync_menu_state    (UkuiPanelAppletFrame    *frame);

void  ukui_panel_applet_frame_change_orientation (UkuiPanelAppletFrame    *frame,
					     PanelOrientation     orientation);

void  ukui_panel_applet_frame_change_size        (UkuiPanelAppletFrame    *frame,
					     guint                size);

void  ukui_panel_applet_frame_change_background  (UkuiPanelAppletFrame    *frame,
					     PanelBackgroundType  type);

void  ukui_panel_applet_frame_set_panel          (UkuiPanelAppletFrame    *frame,
					     PanelWidget         *panel);


/* For module implementations only */

typedef struct _UkuiPanelAppletFrameActivating        UkuiPanelAppletFrameActivating;

GdkScreen        *panel_applet_frame_activating_get_screen      (UkuiPanelAppletFrameActivating *frame_act);
PanelOrientation  ukui_panel_applet_frame_activating_get_orientation (UkuiPanelAppletFrameActivating *frame_act);
guint32           ukui_panel_applet_frame_activating_get_size        (UkuiPanelAppletFrameActivating *frame_act);
gboolean          ukui_panel_applet_frame_activating_get_locked      (UkuiPanelAppletFrameActivating *frame_act);
gboolean          ukui_panel_applet_frame_activating_get_locked_down (UkuiPanelAppletFrameActivating *frame_act);
gchar            *ukui_panel_applet_frame_activating_get_conf_path   (UkuiPanelAppletFrameActivating *frame_act);

void  _ukui_panel_applet_frame_set_iid               (UkuiPanelAppletFrame           *frame,
						 const gchar                *iid);

void  _ukui_panel_applet_frame_activated             (UkuiPanelAppletFrame           *frame,
						 UkuiPanelAppletFrameActivating *frame_act,
						 GError                     *error);

void  _ukui_panel_applet_frame_update_flags          (UkuiPanelAppletFrame *frame,
						 gboolean          major,
						 gboolean          minor,
						 gboolean          has_handle);

void  _ukui_panel_applet_frame_update_size_hints     (UkuiPanelAppletFrame *frame,
						 gint             *size_hints,
						 guint             n_elements);

char *_ukui_panel_applet_frame_get_background_string (UkuiPanelAppletFrame    *frame,
						 PanelWidget         *panel,
						 PanelBackgroundType  type);

void  _ukui_panel_applet_frame_applet_broken         (UkuiPanelAppletFrame *frame);

void  _ukui_panel_applet_frame_applet_remove         (UkuiPanelAppletFrame *frame);
void  _ukui_panel_applet_frame_applet_move           (UkuiPanelAppletFrame *frame);
void  _ukui_panel_applet_frame_applet_lock           (UkuiPanelAppletFrame *frame,
						 gboolean          locked);
#ifdef __cplusplus
}
#endif

#endif /* __PANEL_APPLET_FRAME_H__ */

