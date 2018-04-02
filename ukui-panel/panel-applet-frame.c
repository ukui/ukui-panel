/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * panel-applet-frame.c: panel side container for applets
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * Authors:
 *	Mark McLoughlin <mark@skynet.ie>
 */

#include <config.h>
#include <string.h>

#include <glib/gi18n.h>

#include <gio/gio.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "panel-applets-manager.h"
#include "panel-profile.h"
#include "panel.h"
#include "applet.h"
#include "panel-marshal.h"
#include "panel-background.h"
#include "panel-lockdown.h"
#include "panel-stock-icons.h"
#include "xstuff.h"
#include "panel-schemas.h"

#include "panel-applet-frame.h"

#define PANEL_RESPONSE_DELETE       0
#define PANEL_RESPONSE_DONT_RELOAD  1
#define PANEL_RESPONSE_RELOAD       2

static void ukui_panel_applet_frame_activating_free (UkuiPanelAppletFrameActivating *frame_act);

static void ukui_panel_applet_frame_loading_failed  (const char  *iid,
					        PanelWidget *panel,
					        const char  *id);

static void ukui_panel_applet_frame_load            (const gchar *iid,
						PanelWidget *panel,
						gboolean     locked,
						int          position,
						gboolean     exactpos,
						const char  *id);

struct _UkuiPanelAppletFrameActivating {
	gboolean     locked;
	PanelWidget *panel;
	int          position;
	gboolean     exactpos;
	char        *id;
};

/* UkuiPanelAppletFrame implementation */

G_DEFINE_TYPE (UkuiPanelAppletFrame, ukui_panel_applet_frame, GTK_TYPE_EVENT_BOX)

#define UKUI_PANEL_APPLET_FRAME_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), PANEL_TYPE_APPLET_FRAME, UkuiPanelAppletFramePrivate))

#define HANDLE_SIZE 10
#define UKUI_PANEL_APPLET_PREFS_PATH "/org/ukui/panel/objects/%s/prefs/"

struct _UkuiPanelAppletFramePrivate {
	PanelWidget     *panel;
	AppletInfo      *applet_info;

	PanelOrientation orientation;

	gchar           *iid;

	GtkAllocation    child_allocation;
	GdkRectangle     handle_rect;

	guint            has_handle : 1;
};

static gboolean
ukui_panel_applet_frame_draw (GtkWidget *widget,
                         cairo_t   *cr)
{
	UkuiPanelAppletFrame *frame = UKUI_PANEL_APPLET_FRAME (widget);
	GtkStyleContext *context;
	GtkStateFlags     state;
	cairo_pattern_t  *bg_pattern;
	PanelBackground  *background;

	if (GTK_WIDGET_CLASS (ukui_panel_applet_frame_parent_class)->draw)
		GTK_WIDGET_CLASS (ukui_panel_applet_frame_parent_class)->draw (widget, cr);

	if (!frame->priv->has_handle)
		return FALSE;

	context = gtk_widget_get_style_context (widget);
	state = gtk_widget_get_state_flags (widget);
	gtk_style_context_save (context);
	gtk_style_context_set_state (context, state);

	cairo_save (cr);

	/* Set the pattern transform so as to correctly render a patterned
	 * background with the handle */
	gtk_style_context_get (context, state,
			       "background-image", &bg_pattern,
			       NULL);

#if GTK_CHECK_VERSION (3, 18, 0)
	background = &frame->priv->panel->toplevel->background;
#else
	background = &frame->priv->panel->background;
#endif
	if (bg_pattern && (background->type == PANEL_BACK_IMAGE ||
	    (background->type == PANEL_BACK_COLOR && background->has_alpha))) {
		cairo_matrix_t ptm;

		cairo_matrix_init_translate (&ptm,
					     frame->priv->handle_rect.x,
					     frame->priv->handle_rect.y);
		cairo_matrix_scale (&ptm,
				    frame->priv->handle_rect.width,
				    frame->priv->handle_rect.height);
		cairo_pattern_set_matrix (bg_pattern, &ptm);
		cairo_pattern_destroy (bg_pattern);
	}

	cairo_rectangle (cr,
		frame->priv->handle_rect.x,
		frame->priv->handle_rect.y,
		frame->priv->handle_rect.width,
		frame->priv->handle_rect.height);
	cairo_clip (cr);
	gtk_render_handle (context, cr,
			   0, 0,
			   gtk_widget_get_allocated_width (widget),
			   gtk_widget_get_allocated_height (widget));

	cairo_restore (cr);

	gtk_style_context_restore (context);

	return FALSE;
}

static void
ukui_panel_applet_frame_update_background_size (UkuiPanelAppletFrame *frame,
					   GtkAllocation    *old_allocation,
					   GtkAllocation    *new_allocation)
{
	PanelBackground *background;

	if (old_allocation->x      == new_allocation->x &&
	    old_allocation->y      == new_allocation->y &&
	    old_allocation->width  == new_allocation->width &&
	    old_allocation->height == new_allocation->height)
		return;
#if GTK_CHECK_VERSION (3, 18, 0)
	background = &frame->priv->panel->toplevel->background;
#else
	background = &frame->priv->panel->background;
#endif
	if (background->type == PANEL_BACK_NONE ||
	   (background->type == PANEL_BACK_COLOR && !background->has_alpha))
		return;

	ukui_panel_applet_frame_change_background (frame, background->type);
}

static void
ukui_panel_applet_frame_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width)
{
	UkuiPanelAppletFrame *frame;
	GtkBin           *bin;
	GtkWidget        *child;
	guint             border_width;

	frame = UKUI_PANEL_APPLET_FRAME (widget);
	bin = GTK_BIN (widget);

	if (!frame->priv->has_handle) {
		GTK_WIDGET_CLASS (ukui_panel_applet_frame_parent_class)->get_preferred_width (widget, minimal_width, natural_width);
		return;
	}

	child = gtk_bin_get_child (bin);
	if (child && gtk_widget_get_visible (child))
		gtk_widget_get_preferred_width (child, minimal_width, natural_width);

	border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
	*minimal_width += border_width;
	*natural_width += border_width;

	switch (frame->priv->orientation) {
	case PANEL_ORIENTATION_TOP:
	case PANEL_ORIENTATION_BOTTOM:
		*minimal_width += HANDLE_SIZE;
		*natural_width += HANDLE_SIZE;
		break;
	case PANEL_ORIENTATION_LEFT:
	case PANEL_ORIENTATION_RIGHT:
		break;
	default:
		g_assert_not_reached ();
		break;
	}
}

static void
ukui_panel_applet_frame_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height)
{
	UkuiPanelAppletFrame *frame;
	GtkBin           *bin;
	GtkWidget        *child;
	guint             border_width;

	frame = UKUI_PANEL_APPLET_FRAME (widget);
	bin = GTK_BIN (widget);

	if (!frame->priv->has_handle) {
		GTK_WIDGET_CLASS (ukui_panel_applet_frame_parent_class)->get_preferred_height (widget, minimal_height, natural_height);
		return;
	}

	child = gtk_bin_get_child (bin);
	if (child && gtk_widget_get_visible (child))
		gtk_widget_get_preferred_height (child, minimal_height, natural_height);

	border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
	*minimal_height += border_width;
	*natural_height += border_width;

	switch (frame->priv->orientation) {
	case PANEL_ORIENTATION_LEFT:
	case PANEL_ORIENTATION_RIGHT:
		*minimal_height += HANDLE_SIZE;
		*natural_height += HANDLE_SIZE;
		break;
	case PANEL_ORIENTATION_TOP:
	case PANEL_ORIENTATION_BOTTOM:
		break;
	default:
		g_assert_not_reached ();
		break;
	}
}

static void
ukui_panel_applet_frame_size_allocate (GtkWidget     *widget,
				  GtkAllocation *allocation)
{
	UkuiPanelAppletFrame *frame;
	GtkBin           *bin;
	GtkWidget        *child;
	GdkWindow        *window;
	GtkAllocation     new_allocation;
	GtkAllocation     old_allocation;
	GtkAllocation     widget_allocation;

	gtk_widget_get_allocation (widget, &widget_allocation);

	old_allocation.x      = widget_allocation.x;
	old_allocation.y      = widget_allocation.y;
	old_allocation.width  = widget_allocation.width;
	old_allocation.height = widget_allocation.height;

	frame = UKUI_PANEL_APPLET_FRAME (widget);
	bin = GTK_BIN (widget);

	if (!frame->priv->has_handle) {
		GTK_WIDGET_CLASS (ukui_panel_applet_frame_parent_class)->size_allocate (widget,  allocation);
		ukui_panel_applet_frame_update_background_size (frame, &old_allocation, allocation);
		return;
	}

	window = gtk_widget_get_window (widget);
	child = gtk_bin_get_child (bin);
	gtk_widget_set_allocation (widget, allocation);

	frame->priv->handle_rect.x = 0;
	frame->priv->handle_rect.y = 0;

	switch (frame->priv->orientation) {
	case PANEL_ORIENTATION_TOP:
	case PANEL_ORIENTATION_BOTTOM:
		frame->priv->handle_rect.width  = HANDLE_SIZE;
		frame->priv->handle_rect.height = allocation->height;

		if (gtk_widget_get_direction (GTK_WIDGET (frame)) !=
		    GTK_TEXT_DIR_RTL) {
			frame->priv->handle_rect.x = 0;
			new_allocation.x = HANDLE_SIZE;
		} else {
			frame->priv->handle_rect.x = allocation->width - HANDLE_SIZE;
			new_allocation.x = 0;
		}

		new_allocation.y      = 0;
		new_allocation.width  = allocation->width - HANDLE_SIZE;
		new_allocation.height = allocation->height;
		break;
	case PANEL_ORIENTATION_LEFT:
	case PANEL_ORIENTATION_RIGHT:
		frame->priv->handle_rect.width  = allocation->width;
		frame->priv->handle_rect.height = HANDLE_SIZE;

		new_allocation.x      = 0;
		new_allocation.y      = HANDLE_SIZE;
		new_allocation.width  = allocation->width;
		new_allocation.height = allocation->height - HANDLE_SIZE;
		break;
	default:
		g_assert_not_reached ();
		break;
	}

	new_allocation.width  = MAX (1, new_allocation.width);
	new_allocation.height = MAX (1, new_allocation.height);

	/* If the child allocation changed, that means that the frame is drawn
	 * in a new place, so we must redraw the entire widget.
	 */
	if (gtk_widget_get_mapped (widget) &&
	    (new_allocation.x != frame->priv->child_allocation.x ||
	     new_allocation.y != frame->priv->child_allocation.y ||
	     new_allocation.width != frame->priv->child_allocation.width ||
	     new_allocation.height != frame->priv->child_allocation.height))
		gdk_window_invalidate_rect (window, &widget_allocation, FALSE);

	if (gtk_widget_get_realized (widget)) {
		guint border_width;

		border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));
		gdk_window_move_resize (window,
			allocation->x + border_width,
			allocation->y + border_width,
			MAX (allocation->width - border_width * 2, 0),
			MAX (allocation->height - border_width * 2, 0));
	}

	if (child && gtk_widget_get_visible (child))
		gtk_widget_size_allocate (child, &new_allocation);

	frame->priv->child_allocation = new_allocation;

	ukui_panel_applet_frame_update_background_size (frame,
						   &old_allocation,
						   allocation);
}

static inline gboolean
button_event_in_rect (GdkEventButton *event,
		      GdkRectangle   *rect)
{
	if (event->x >= rect->x &&
	    event->x <= (rect->x + rect->width) &&
	    event->y >= rect->y &&
	    event->y <= (rect->y + rect->height))
		return TRUE;

	return FALSE;
}

static gboolean
ukui_panel_applet_frame_button_changed (GtkWidget      *widget,
					GdkEventButton *event)
{
	UkuiPanelAppletFrame *frame;
	gboolean              handled = FALSE;
	GdkDisplay *display;
#if GTK_CHECK_VERSION (3, 20, 0)
	GdkSeat *seat;
#else
	GdkDevice *pointer;
	GdkDeviceManager *device_manager;
#endif

	frame = UKUI_PANEL_APPLET_FRAME (widget);

	if (!frame->priv->has_handle)
		return handled;

	if (event->window != gtk_widget_get_window (widget))
		return FALSE;

	switch (event->button) {
	case 1:
	case 2:
		if (button_event_in_rect (event, &frame->priv->handle_rect)) {
			if (event->type == GDK_BUTTON_PRESS ||
			    event->type == GDK_2BUTTON_PRESS) {
				panel_widget_applet_drag_start (
					frame->priv->panel, GTK_WIDGET (frame),
					PW_DRAG_OFF_CURSOR, event->time);
				handled = TRUE;
			} else if (event->type == GDK_BUTTON_RELEASE) {
				panel_widget_applet_drag_end (frame->priv->panel);
				handled = TRUE;
			}
		}
		break;
	case 3:
		if (event->type == GDK_BUTTON_PRESS ||
		    event->type == GDK_2BUTTON_PRESS) {
#if GTK_CHECK_VERSION(3, 20, 0)
			display = gtk_widget_get_display (widget);
			seat = gdk_display_get_default_seat (display);
			gdk_seat_ungrab (seat);
#else
			display = gtk_widget_get_display (widget);
			device_manager = gdk_display_get_device_manager (display);
			pointer = gdk_device_manager_get_client_pointer (device_manager);
			gdk_device_ungrab (pointer, GDK_CURRENT_TIME);
#endif

			UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->popup_menu (frame,
									  event->button,
									  event->time);

			handled = TRUE;
		} else if (event->type == GDK_BUTTON_RELEASE)
			handled = TRUE;
		break;
	default:
		break;
	}

	return handled;
}

static void
ukui_panel_applet_frame_finalize (GObject *object)
{
	UkuiPanelAppletFrame *frame = UKUI_PANEL_APPLET_FRAME (object);

	ukui_panel_applets_manager_factory_deactivate (frame->priv->iid);

	panel_lockdown_notify_remove (G_CALLBACK (ukui_panel_applet_frame_sync_menu_state),
				      frame);

	g_free (frame->priv->iid);
	frame->priv->iid = NULL;

	G_OBJECT_CLASS (ukui_panel_applet_frame_parent_class)->finalize (object);
}

static void
ukui_panel_applet_frame_class_init (UkuiPanelAppletFrameClass *klass)
{
	GObjectClass   *gobject_class = (GObjectClass *) klass;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;

	gobject_class->finalize = ukui_panel_applet_frame_finalize;

	widget_class->draw                 = ukui_panel_applet_frame_draw;
	widget_class->get_preferred_width  = ukui_panel_applet_frame_get_preferred_width;
	widget_class->get_preferred_height = ukui_panel_applet_frame_get_preferred_height;
	widget_class->size_allocate        = ukui_panel_applet_frame_size_allocate;
	widget_class->button_press_event   = ukui_panel_applet_frame_button_changed;
	widget_class->button_release_event = ukui_panel_applet_frame_button_changed;

	g_type_class_add_private (klass, sizeof (UkuiPanelAppletFramePrivate));
}

static void
ukui_panel_applet_frame_init (UkuiPanelAppletFrame *frame)
{
	frame->priv = UKUI_PANEL_APPLET_FRAME_GET_PRIVATE (frame);

	frame->priv->panel       = NULL;
	frame->priv->orientation = PANEL_ORIENTATION_TOP;
	frame->priv->applet_info = NULL;
	frame->priv->has_handle  = FALSE;
}

static void
ukui_panel_applet_frame_init_properties (UkuiPanelAppletFrame *frame)
{
	UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->init_properties (frame);
}

void
ukui_panel_applet_frame_sync_menu_state (UkuiPanelAppletFrame *frame)
{
	PanelWidget *panel_widget;
	gboolean     locked_down;
	gboolean     locked;
	gboolean     lockable;
	gboolean     movable;
	gboolean     removable;

	panel_widget = PANEL_WIDGET (gtk_widget_get_parent (GTK_WIDGET (frame)));

	movable = ukui_panel_applet_can_freely_move (frame->priv->applet_info);
	removable = panel_profile_id_lists_are_writable ();
	lockable = ukui_panel_applet_lockable (frame->priv->applet_info);

	locked = panel_widget_get_applet_locked (panel_widget, GTK_WIDGET (frame));
	locked_down = panel_lockdown_get_locked_down ();

	UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->sync_menu_state (frame, movable, removable, lockable, locked, locked_down);
}

void
ukui_panel_applet_frame_change_orientation (UkuiPanelAppletFrame *frame,
				       PanelOrientation  orientation)
{
	if (orientation == frame->priv->orientation)
		return;

	frame->priv->orientation = orientation;
	UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->change_orientation (frame, orientation);
}

void
ukui_panel_applet_frame_change_size (UkuiPanelAppletFrame *frame,
				guint             size)
{
	UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->change_size (frame, size);
}

void
ukui_panel_applet_frame_change_background (UkuiPanelAppletFrame    *frame,
				      PanelBackgroundType  type)
{
	GtkWidget *parent;

	g_return_if_fail (PANEL_IS_APPLET_FRAME (frame));

	parent = gtk_widget_get_parent (GTK_WIDGET (frame));

	g_return_if_fail (PANEL_IS_WIDGET (parent));

	if (frame->priv->has_handle) {
		PanelBackground *background;
#if GTK_CHECK_VERSION (3, 18, 0)
		background = &PANEL_WIDGET (parent)->toplevel->background;
#else
		background = &PANEL_WIDGET (parent)->background;
#endif
		panel_background_apply_css (background, GTK_WIDGET (frame));
	}

	UKUI_PANEL_APPLET_FRAME_GET_CLASS (frame)->change_background (frame, type);
}

void
ukui_panel_applet_frame_set_panel (UkuiPanelAppletFrame *frame,
			      PanelWidget      *panel)
{
	g_return_if_fail (PANEL_IS_APPLET_FRAME (frame));
	g_return_if_fail (PANEL_IS_WIDGET (panel));

	frame->priv->panel = panel;
}

void
_ukui_panel_applet_frame_set_iid (UkuiPanelAppletFrame *frame,
			     const gchar      *iid)
{
	if (frame->priv->iid)
		g_free (frame->priv->iid);
	frame->priv->iid = g_strdup (iid);
}

void
_ukui_panel_applet_frame_activated (UkuiPanelAppletFrame           *frame,
			       UkuiPanelAppletFrameActivating *frame_act,
			       GError                     *error)
{
	AppletInfo *info;

	g_assert (frame->priv->iid != NULL);

	if (error != NULL) {
		g_warning ("Failed to load applet %s:\n%s",
			   frame->priv->iid, error->message);
		g_error_free (error);

		ukui_panel_applet_frame_loading_failed (frame->priv->iid,
						   frame_act->panel,
						   frame_act->id);
		ukui_panel_applet_frame_activating_free (frame_act);
		gtk_widget_destroy (GTK_WIDGET (frame));

		return;
	}

	frame->priv->panel = frame_act->panel;
	gtk_widget_show_all (GTK_WIDGET (frame));

	info = ukui_panel_applet_register (GTK_WIDGET (frame), GTK_WIDGET (frame),
				      NULL, frame->priv->panel,
				      frame_act->locked, frame_act->position,
				      frame_act->exactpos, PANEL_OBJECT_APPLET,
				      frame_act->id);
	frame->priv->applet_info = info;

	panel_widget_set_applet_size_constrained (frame->priv->panel,
						  GTK_WIDGET (frame), TRUE);

	ukui_panel_applet_frame_sync_menu_state (frame);
	ukui_panel_applet_frame_init_properties (frame);

	panel_lockdown_notify_add (G_CALLBACK (ukui_panel_applet_frame_sync_menu_state),
				   frame);

	ukui_panel_applet_stop_loading (frame_act->id);
	ukui_panel_applet_frame_activating_free (frame_act);
}

void
_ukui_panel_applet_frame_update_flags (UkuiPanelAppletFrame *frame,
				  gboolean          major,
				  gboolean          minor,
				  gboolean          has_handle)
{
	gboolean old_has_handle;

	panel_widget_set_applet_expandable (
		frame->priv->panel, GTK_WIDGET (frame), major, minor);

	old_has_handle = frame->priv->has_handle;
	frame->priv->has_handle = has_handle;

	if (!old_has_handle && frame->priv->has_handle) {
		/* we've added an handle, so we need to get the background for
		 * it */
		PanelBackground *background;

#if GTK_CHECK_VERSION (3, 18, 0)
		background = &frame->priv->panel->toplevel->background;
#else
		background = &frame->priv->panel->background;
#endif
		ukui_panel_applet_frame_change_background (frame, background->type);
	}
}

void
_ukui_panel_applet_frame_update_size_hints (UkuiPanelAppletFrame *frame,
				       gint             *size_hints,
				       guint             n_elements)
{
	if (frame->priv->has_handle) {
		gint extra_size = HANDLE_SIZE + 1;
		gint i;
	
		                GdkScreen* screen;
                gint       screen_width;
                gint       screen_height;
                screen = gtk_window_get_screen(GTK_WINDOW(frame->priv->panel->toplevel));
                gdk_window_get_geometry (gdk_screen_get_root_window (screen), NULL, NULL,
                                 &screen_width, &screen_height);

		char          	*path;
		int 		 value = 0;
		GSettings	*settings;

	        path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
	        settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);

		if (g_settings_get_int(settings, "position") !=0){
                    value=g_settings_get_int(settings, "position")-180-g_settings_get_int(settings, "launcher-nums")*40;
		}
		for (i = 0; i < n_elements; i++) {

			size_hints[i] += extra_size;

			if (value == 0){
				value = screen_width - 195 -200;
			}
			if (size_hints[i] > value) {

				size_hints[i] = value;	

			}
		}
	}

	/* It takes the ownership of size-hints array */
	panel_widget_set_applet_size_hints (frame->priv->panel,
					    GTK_WIDGET (frame),
					    size_hints,
					    n_elements);
}

char *
_ukui_panel_applet_frame_get_background_string (UkuiPanelAppletFrame    *frame,
					   PanelWidget         *panel,
					   PanelBackgroundType  type)
{
	GtkAllocation allocation;
	int x;
	int y;

	gtk_widget_get_allocation (GTK_WIDGET (frame), &allocation);

	x = allocation.x;
	y = allocation.y;

	if (frame->priv->has_handle) {
		switch (frame->priv->orientation) {
		case PANEL_ORIENTATION_TOP:
		case PANEL_ORIENTATION_BOTTOM:
			if (gtk_widget_get_direction (GTK_WIDGET (frame)) !=
			    GTK_TEXT_DIR_RTL)
				x += frame->priv->handle_rect.width;
			break;
		case PANEL_ORIENTATION_LEFT:
		case PANEL_ORIENTATION_RIGHT:
			y += frame->priv->handle_rect.height;
			break;
		default:
			g_assert_not_reached ();
			break;
		}
	}
#if GTK_CHECK_VERSION (3, 18, 0)
	return panel_background_make_string (&panel->toplevel->background, x, y);
#else
	return panel_background_make_string (&panel->background, x, y);
#endif
}

static void
ukui_panel_applet_frame_reload_response (GtkWidget        *dialog,
				    int               response,
				    UkuiPanelAppletFrame *frame)
{
	AppletInfo *info;

	g_return_if_fail (PANEL_IS_APPLET_FRAME (frame));

	if (!frame->priv->iid || !frame->priv->panel) {
		g_object_unref (frame);
		gtk_widget_destroy (dialog);
		return;
	}

	info = frame->priv->applet_info;

	if (response == PANEL_RESPONSE_RELOAD) {
		PanelWidget *panel;
		char        *iid;
		char        *id = NULL;
		int          position = -1;
		gboolean     locked = FALSE;

		panel = frame->priv->panel;
		iid   = g_strdup (frame->priv->iid);

		if (info) {
			id = g_strdup (info->id);
			position  = ukui_panel_applet_get_position (info);
			locked = panel_widget_get_applet_locked (panel, info->widget);
			ukui_panel_applet_clean (info);
		}

		ukui_panel_applet_frame_load (iid, panel, locked,
					 position, TRUE, id);

		g_free (iid);
		g_free (id);

	} else if (response == PANEL_RESPONSE_DELETE) {
		/* if we can't write to applets list we can't really delete
		   it, so we'll just ignore this.  FIXME: handle this
		   more correctly I suppose. */
		if (panel_profile_id_lists_are_writable () && info)
			panel_profile_delete_object (info);
	}

	g_object_unref (frame);
	gtk_widget_destroy (dialog);
}

void
_ukui_panel_applet_frame_applet_broken (UkuiPanelAppletFrame *frame)
{
	GtkWidget  *dialog;
	GdkScreen  *screen;
	const char *applet_name = NULL;
	char       *dialog_txt;

	screen = gtk_widget_get_screen (GTK_WIDGET (frame));

	if (xstuff_is_display_dead ())
		return;

	if (frame->priv->iid) {
		UkuiPanelAppletInfo *info;

		info = (UkuiPanelAppletInfo *)ukui_panel_applets_manager_get_applet_info (frame->priv->iid);
		applet_name = ukui_panel_applet_info_get_name (info);
	}

	if (applet_name)
		dialog_txt = g_strdup_printf (_("\"%s\" has quit unexpectedly"), applet_name);
	else
		dialog_txt = g_strdup (_("Panel object has quit unexpectedly"));

	dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
					 GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE,
					 dialog_txt, applet_name ? applet_name : NULL);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
						  _("If you reload a panel object, it will automatically "
						    "be added back to the panel."));

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);

	if (panel_profile_id_lists_are_writable ()) {
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					_("D_elete"), PANEL_RESPONSE_DELETE,
					_("_Don't Reload"), PANEL_RESPONSE_DONT_RELOAD,
					_("_Reload"), PANEL_RESPONSE_RELOAD,
					NULL);
	} else {
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					_("_Don't Reload"), PANEL_RESPONSE_DONT_RELOAD,
					_("_Reload"), PANEL_RESPONSE_RELOAD,
					NULL);
	}

	gtk_dialog_set_default_response (GTK_DIALOG (dialog),
					 PANEL_RESPONSE_RELOAD);

	gtk_window_set_screen (GTK_WINDOW (dialog), screen);

	g_signal_connect (dialog, "response",
			  G_CALLBACK (ukui_panel_applet_frame_reload_response),
			  g_object_ref (frame));

	panel_widget_register_open_dialog (frame->priv->panel, dialog);
	gtk_window_set_urgency_hint (GTK_WINDOW (dialog), TRUE);
	/* FIXME: http://bugzilla.gnome.org/show_bug.cgi?id=165132 */
	gtk_window_set_title (GTK_WINDOW (dialog), _("Error"));

	gtk_widget_show (dialog);
	g_free (dialog_txt);
}

void
_ukui_panel_applet_frame_applet_remove (UkuiPanelAppletFrame *frame)
{
	AppletInfo *info;

	if (!frame->priv->applet_info)
		return;

	info = frame->priv->applet_info;
	frame->priv->applet_info = NULL;

	panel_profile_delete_object (info);
}

void
_ukui_panel_applet_frame_applet_move (UkuiPanelAppletFrame *frame)
{
	GtkWidget *widget = GTK_WIDGET (frame);
	GtkWidget *parent = gtk_widget_get_parent (widget);

	if (!PANEL_IS_WIDGET (parent))
		return;

	panel_widget_applet_drag_start (PANEL_WIDGET (parent),
					widget,
					PW_DRAG_OFF_CENTER,
					GDK_CURRENT_TIME);
}

void
_ukui_panel_applet_frame_applet_lock (UkuiPanelAppletFrame *frame,
				 gboolean          locked)
{
	PanelWidget *panel_widget = PANEL_WIDGET (gtk_widget_get_parent (GTK_WIDGET (frame)));

	if (panel_widget_get_applet_locked (panel_widget, GTK_WIDGET (frame)) == locked)
		return;

	ukui_panel_applet_toggle_locked (frame->priv->applet_info);
}

/* Generic methods */

static GSList *no_reload_applets = NULL;

enum {
	LOADING_FAILED_RESPONSE_DONT_DELETE,
	LOADING_FAILED_RESPONSE_DELETE
};

static void
ukui_panel_applet_frame_activating_free (UkuiPanelAppletFrameActivating *frame_act)
{
	g_free (frame_act->id);
	g_slice_free (UkuiPanelAppletFrameActivating, frame_act);
}

GdkScreen *
panel_applet_frame_activating_get_screen (UkuiPanelAppletFrameActivating *frame_act)
{
    return gtk_widget_get_screen (GTK_WIDGET(frame_act->panel));
}

PanelOrientation
ukui_panel_applet_frame_activating_get_orientation(UkuiPanelAppletFrameActivating *frame_act)
{
	return panel_widget_get_applet_orientation(frame_act->panel);
}

guint32
ukui_panel_applet_frame_activating_get_size (UkuiPanelAppletFrameActivating *frame_act)
{
	return frame_act->panel->sz;
}

gboolean
ukui_panel_applet_frame_activating_get_locked (UkuiPanelAppletFrameActivating *frame_act)
{
	return frame_act->locked;
}

gboolean
ukui_panel_applet_frame_activating_get_locked_down (UkuiPanelAppletFrameActivating *frame_act)
{
	return panel_lockdown_get_locked_down ();
}

gchar *
ukui_panel_applet_frame_activating_get_conf_path (UkuiPanelAppletFrameActivating *frame_act)
{
	return g_strdup_printf (UKUI_PANEL_APPLET_PREFS_PATH, frame_act->id);
}

static void
ukui_panel_applet_frame_loading_failed_response (GtkWidget *dialog,
					    guint      response,
					    char      *id)
{
	gtk_widget_destroy (dialog);

	if (response == LOADING_FAILED_RESPONSE_DELETE &&
	    !panel_lockdown_get_locked_down () &&
	    panel_profile_id_lists_are_writable ()) {
		GSList *item;

		item = g_slist_find_custom (no_reload_applets, id,
					    (GCompareFunc) strcmp);
		if (item) {
			g_free (item->data);
			no_reload_applets = g_slist_delete_link (no_reload_applets,
								 item);
		}

		panel_profile_remove_from_list (PANEL_GSETTINGS_OBJECTS, id);
	}

	g_free (id);
}

static void
ukui_panel_applet_frame_loading_failed (const char  *iid,
				   PanelWidget *panel,
				   const char  *id)
{
	GtkWidget *dialog;
	char      *problem_txt;
	gboolean   locked_down;

	no_reload_applets = g_slist_prepend (no_reload_applets,
					     g_strdup (id));

	locked_down = panel_lockdown_get_locked_down ();

	problem_txt = g_strdup_printf (_("The panel encountered a problem "
					 "while loading \"%s\"."),
				       iid);

	dialog = gtk_message_dialog_new (NULL, 0,
					 locked_down ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
					 GTK_BUTTONS_NONE,
					 "%s", problem_txt);
	g_free (problem_txt);

	if (locked_down) {
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					GTK_STOCK_OK, LOADING_FAILED_RESPONSE_DONT_DELETE,
					NULL);
	} else {
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
					_("Do you want to delete the applet "
					  "from your configuration?"));
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					PANEL_STOCK_DONT_DELETE, LOADING_FAILED_RESPONSE_DONT_DELETE,
					GTK_STOCK_DELETE, LOADING_FAILED_RESPONSE_DELETE,
					NULL);
	}

	gtk_dialog_set_default_response (GTK_DIALOG (dialog),
					 LOADING_FAILED_RESPONSE_DONT_DELETE);

	gtk_window_set_screen (GTK_WINDOW (dialog),
			       gtk_window_get_screen (GTK_WINDOW (panel->toplevel)));

	g_signal_connect (dialog, "response",
			  G_CALLBACK (ukui_panel_applet_frame_loading_failed_response),
			  g_strdup (id));

	panel_widget_register_open_dialog (panel, dialog);
	gtk_window_set_urgency_hint (GTK_WINDOW (dialog), TRUE);
	/* FIXME: http://bugzilla.gnome.org/show_bug.cgi?id=165132 */
	gtk_window_set_title (GTK_WINDOW (dialog), _("Error"));

	gtk_widget_show_all (dialog);

	/* Note: this call will free the memory for id, so the variable should
	 * not get accessed afterwards. */
	ukui_panel_applet_stop_loading (id);
}

static void
ukui_panel_applet_frame_load (const gchar *iid,
			 PanelWidget *panel,
			 gboolean     locked,
			 int          position,
			 gboolean     exactpos,
			 const char  *id)
{
	UkuiPanelAppletFrameActivating *frame_act;

	g_return_if_fail (iid != NULL);
	g_return_if_fail (panel != NULL);
	g_return_if_fail (id != NULL);

	if (g_slist_find_custom (no_reload_applets, id,
				 (GCompareFunc) strcmp)) {
		ukui_panel_applet_stop_loading (id);
		return;
	}

	if (panel_lockdown_is_applet_disabled (iid)) {
		ukui_panel_applet_stop_loading (id);
		return;
	}

	frame_act = g_slice_new0 (UkuiPanelAppletFrameActivating);
	frame_act->locked   = locked;
	frame_act->panel    = panel;
	frame_act->position = position;
	frame_act->exactpos = exactpos;
	frame_act->id       = g_strdup (id);

	if (!ukui_panel_applets_manager_load_applet (iid, frame_act)) {
		ukui_panel_applet_frame_loading_failed (iid, panel, id);
		ukui_panel_applet_frame_activating_free (frame_act);
	}
}

void
ukui_panel_applet_frame_load_from_gsettings (PanelWidget *panel_widget,
				    gboolean     locked,
				    int          position,
				    const char  *id)
{
	GSettings *settings;
	gchar *path;
	gchar *applet_iid;

	g_return_if_fail (panel_widget != NULL);
	g_return_if_fail (id != NULL);

	path = g_strdup_printf (PANEL_OBJECT_PATH "%s/", id);
	settings = g_settings_new_with_path (PANEL_OBJECT_SCHEMA, path);
	applet_iid = g_settings_get_string (settings, PANEL_OBJECT_APPLET_IID_KEY);
	g_object_unref (settings);
	g_free (path);

	if (!applet_iid) {
		ukui_panel_applet_stop_loading (id);
		return;
	}

	ukui_panel_applet_frame_load (applet_iid, panel_widget,
				 locked, position, TRUE, id);

	g_free (applet_iid);
}

void
ukui_panel_applet_frame_create (PanelToplevel *toplevel,
			   int            position,
			   const char    *iid)
{
	GSettings   *settings;
	gchar       *path;
	char        *id;

	g_return_if_fail (iid != NULL);

	id = panel_profile_prepare_object (PANEL_OBJECT_APPLET, toplevel, position, FALSE);

	path = g_strdup_printf (PANEL_OBJECT_PATH "%s/", id);
	settings = g_settings_new_with_path (PANEL_OBJECT_SCHEMA, path);
	g_settings_set_string (settings, PANEL_OBJECT_APPLET_IID_KEY, iid);

	panel_profile_add_to_list (PANEL_GSETTINGS_OBJECTS, id);

	g_free (id);
	g_free (path);
	g_object_unref (settings);
}
