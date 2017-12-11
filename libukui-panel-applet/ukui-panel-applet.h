/*
 * ukui-panel-applet.h: panel applet writing API.
 *
 * Copyright (C) 2001 Sun Microsystems, Inc.
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
 * Authors:
 *     Mark McLoughlin <mark@skynet.ie>
 */

#ifndef __UKUI_PANEL_APPLET_H__
#define __UKUI_PANEL_APPLET_H__

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <cairo-gobject.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	UKUI_PANEL_APPLET_ORIENT_UP,
	UKUI_PANEL_APPLET_ORIENT_DOWN,
	UKUI_PANEL_APPLET_ORIENT_LEFT,
	UKUI_PANEL_APPLET_ORIENT_RIGHT
#define UKUI_PANEL_APPLET_ORIENT_FIRST UKUI_PANEL_APPLET_ORIENT_UP
#define UKUI_PANEL_APPLET_ORIENT_LAST  UKUI_PANEL_APPLET_ORIENT_RIGHT
} UkuiPanelAppletOrient;

#define PANEL_TYPE_APPLET              (ukui_panel_applet_get_type ())
#define UKUI_PANEL_APPLET(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PANEL_TYPE_APPLET, UkuiPanelApplet))
#define UKUI_PANEL_APPLET_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PANEL_TYPE_APPLET, UkuiPanelAppletClass))
#define PANEL_IS_APPLET(o)             (G_TYPE_CHECK_INSTANCE_TYPE ((o), PANEL_TYPE_APPLET))
#define PANEL_IS_APPLET_CLASS(k)       (G_TYPE_CHECK_CLASS_TYPE ((k), PANEL_TYPE_APPLET))
#define UKUI_PANEL_APPLET_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PANEL_TYPE_APPLET, UkuiPanelAppletClass))

typedef enum {
	PANEL_NO_BACKGROUND,
	PANEL_COLOR_BACKGROUND,
	PANEL_PIXMAP_BACKGROUND
} UkuiPanelAppletBackgroundType;

typedef enum {
	UKUI_PANEL_APPLET_FLAGS_NONE   = 0,
	UKUI_PANEL_APPLET_EXPAND_MAJOR = 1 << 0,
	UKUI_PANEL_APPLET_EXPAND_MINOR = 1 << 1,
	UKUI_PANEL_APPLET_HAS_HANDLE   = 1 << 2
#define UKUI_PANEL_APPLET_FLAGS_ALL (UKUI_PANEL_APPLET_EXPAND_MAJOR|UKUI_PANEL_APPLET_EXPAND_MINOR|UKUI_PANEL_APPLET_HAS_HANDLE)
} UkuiPanelAppletFlags;

typedef struct _UkuiPanelApplet        UkuiPanelApplet;
typedef struct _UkuiPanelAppletClass   UkuiPanelAppletClass;
typedef struct _UkuiPanelAppletPrivate UkuiPanelAppletPrivate;

typedef gboolean (*UkuiPanelAppletFactoryCallback) (UkuiPanelApplet* applet, const gchar *iid, gpointer user_data);

struct _UkuiPanelApplet {
	GtkEventBox event_box;

	UkuiPanelAppletPrivate* priv;
};

struct _UkuiPanelAppletClass {
	GtkEventBoxClass event_box_class;

	void (*change_orient) (UkuiPanelApplet* applet, UkuiPanelAppletOrient orient);

	void (*change_size) (UkuiPanelApplet* applet, guint size);

	void (*change_background) (UkuiPanelApplet *applet, UkuiPanelAppletBackgroundType type, GdkRGBA* color, cairo_pattern_t *pattern);

	void (*move_focus_out_of_applet) (UkuiPanelApplet* frame, GtkDirectionType direction);
};

GType ukui_panel_applet_get_type(void) G_GNUC_CONST;

GtkWidget* ukui_panel_applet_new(void);

UkuiPanelAppletOrient ukui_panel_applet_get_orient(UkuiPanelApplet* applet);
guint ukui_panel_applet_get_size(UkuiPanelApplet* applet);
UkuiPanelAppletBackgroundType ukui_panel_applet_get_background (UkuiPanelApplet *applet, /* return values */ GdkRGBA* color, cairo_pattern_t** pattern);
void ukui_panel_applet_set_background_widget(UkuiPanelApplet* applet, GtkWidget* widget);

gchar* ukui_panel_applet_get_preferences_path(UkuiPanelApplet* applet);

UkuiPanelAppletFlags ukui_panel_applet_get_flags(UkuiPanelApplet* applet);
void ukui_panel_applet_set_flags(UkuiPanelApplet* applet, UkuiPanelAppletFlags flags);

void ukui_panel_applet_set_size_hints(UkuiPanelApplet* applet, const int* size_hints, int n_elements, int base_size);

gboolean ukui_panel_applet_get_locked_down(UkuiPanelApplet* applet);

void ukui_panel_applet_request_focus(UkuiPanelApplet* applet, guint32 timestamp);

void ukui_panel_applet_setup_menu(UkuiPanelApplet* applet, const gchar* xml, GtkActionGroup* action_group);
void ukui_panel_applet_setup_menu_from_file(UkuiPanelApplet* applet, const gchar* filename, GtkActionGroup* action_group);

int ukui_panel_applet_factory_main(const gchar* factory_id,gboolean  out_process, GType applet_type, UkuiPanelAppletFactoryCallback callback, gpointer data);

int  ukui_panel_applet_factory_setup_in_process (const gchar               *factory_id,
							  GType                      applet_type,
							  UkuiPanelAppletFactoryCallback callback,
							  gpointer                   data);


/*
 * These macros are getting a bit unwieldy.
 *
 * Things to define for these:
 *	+ required if Native Language Support is enabled (ENABLE_NLS):
 *                   GETTEXT_PACKAGE and UKUILOCALEDIR
 */

#if !defined(ENABLE_NLS)
	#define _UKUI_PANEL_APPLET_SETUP_GETTEXT(call_textdomain) \
		do { } while (0)
#else /* defined(ENABLE_NLS) */
	#include <libintl.h>
	#define _UKUI_PANEL_APPLET_SETUP_GETTEXT(call_textdomain) \
	do { \
		bindtextdomain (GETTEXT_PACKAGE, UKUILOCALEDIR); \
		bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8"); \
		if (call_textdomain) \
			textdomain (GETTEXT_PACKAGE); \
	} while (0)
#endif /* !defined(ENABLE_NLS) */

#define UKUI_PANEL_APPLET_OUT_PROCESS_FACTORY(factory_id, type, name, callback, data) \
int main(int argc, char* argv[]) \
{ \
	GOptionContext* context; \
	GError* error; \
	int retval; \
	 \
	_UKUI_PANEL_APPLET_SETUP_GETTEXT (TRUE); \
	 \
	context = g_option_context_new(""); \
	g_option_context_add_group (context, gtk_get_option_group(TRUE)); \
	 \
	error = NULL; \
	if (!g_option_context_parse (context, &argc, &argv, &error)) \
	{ \
		if (error) \
		{ \
			g_printerr ("Cannot parse arguments: %s.\n", error->message); \
			g_error_free (error); \
		} \
		else \
		{ \
			g_printerr ("Cannot parse arguments.\n"); \
		} \
		g_option_context_free (context); \
		return 1; \
	} \
	 \
	gtk_init (&argc, &argv); \
	 \
	retval = ukui_panel_applet_factory_main (factory_id,TRUE, type, callback, data); \
	g_option_context_free (context); \
	 \
	return retval; \
}

#define UKUI_PANEL_APPLET_IN_PROCESS_FACTORY(factory_id, type, descr, callback, data) \
gboolean _ukui_panel_applet_shlib_factory (void);	\
G_MODULE_EXPORT gint _ukui_panel_applet_shlib_factory(void) \
{ \
	_UKUI_PANEL_APPLET_SETUP_GETTEXT(FALSE); \
return ukui_panel_applet_factory_setup_in_process (factory_id, type,                 \
                                               callback, data);  \
}

#ifdef __cplusplus
}
#endif

#endif /* __UKUI_PANEL_APPLET_H__ */
