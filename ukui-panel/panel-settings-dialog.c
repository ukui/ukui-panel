/*
 * panel-settings-dialog.c:
 *
 * Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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

 * Authors:
 *	quankang@kylinos.cn
 */

#include <config.h>

#include "panel-settings-dialog.h"

#include <string.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

#include <libpanel-util/panel-error.h>
#include <libpanel-util/panel-glib.h>
#include <libpanel-util/panel-gtk.h>
#include <libpanel-util/panel-icon-chooser.h>
#include <libpanel-util/panel-show.h>

#include "panel-profile.h"
#include "panel-schemas.h"
#include "panel-util.h"
#include "panel-globals.h"
#include "panel-icon-names.h"

#define DEFAULT_SIZE 40
#define MEDIUM_SIZE  60 
#define LARGE_SIZE   80
int add = 0;
char *Applet_Name = "name";
typedef struct {
	PanelToplevel *toplevel;
	GSettings     *settings;
	GSettings     *background_settings;

	GtkWidget     *properties_dialog;

	GtkWidget     *general_table;
	GtkWidget     *general_vbox;
	GtkWidget     *orientation_combo;
	GtkWidget     *orientation_label;
	GtkWidget     *size_widgets;
	GtkWidget     *size_spin;
	GtkWidget     *size_label;
	GtkWidget     *size_label_pixels;
	GtkWidget     *icon_align;
	GtkWidget     *icon_chooser;
	GtkWidget     *icon_label;
  	GtkWidget     *expand_toggle;
	GtkWidget     *autohide_toggle;
	GtkWidget     *hidebuttons_toggle;
	GtkWidget     *arrows_toggle;
	GtkWidget     *default_radio;
	GtkWidget     *color_radio;
	GtkWidget     *image_radio;
	GtkWidget     *color_widgets;
	GtkWidget     *image_widgets;
	GtkWidget     *color_button;
	GtkWidget     *color_label;
	GtkWidget     *image_chooser;
	GtkWidget     *opacity_scale;
	GtkWidget     *opacity_label;
	GtkWidget     *opacity_legend;
	GtkWidget     *lock_toggle;
	GtkWidget     *hide_workspace;
	GtkWidget     *default_height_size_radio;
	GtkWidget     *medium_height_size_radio;
	GtkWidget     *large_height_size_radio;
	GtkWidget     *manage_icons_label;

	GtkWidget     *writability_warn_general;
	GtkWidget     *writability_warn_background;

	/* FIXME: This is a workaround for GTK+ bug #327243 */
	int            selection_emitted;
} PanelPropertiesDialog;

static GQuark panel_properties_dialog_quark = 0;

static void panel_properties_dialog_opacity_changed (PanelPropertiesDialog *dialog);

static void
panel_properties_dialog_free (PanelPropertiesDialog *dialog)
{
	if (dialog->settings)
		g_object_unref (dialog->settings);
	dialog->settings = NULL;

	if (dialog->background_settings)
		g_object_unref (dialog->background_settings);
	dialog->background_settings = NULL;

	if (dialog->properties_dialog)
		gtk_widget_destroy (dialog->properties_dialog);
	dialog->properties_dialog = NULL;

	g_free (dialog);
}

enum {
	COLUMN_TEXT,
	COLUMN_ITEM,
	NUMBER_COLUMNS
};

typedef struct {
	const char       *name;
	PanelOrientation  orientation;
} OrientationComboItem;

static OrientationComboItem orientation_items [] = {
	{ NC_("Orientation", "Top"),    PANEL_ORIENTATION_TOP    },
	{ NC_("Orientation", "Bottom"), PANEL_ORIENTATION_BOTTOM },
	{ NC_("Orientation", "Left"),   PANEL_ORIENTATION_LEFT   },
	{ NC_("Orientation", "Right"),  PANEL_ORIENTATION_RIGHT  }
};

static void
panel_properties_dialog_orientation_changed (PanelPropertiesDialog *dialog,
					     GtkComboBox           *combo_box)
{
	GtkTreeIter           iter;
	GtkTreeModel         *model;
	OrientationComboItem *item;

	g_assert (dialog->orientation_combo == GTK_WIDGET (combo_box));

	if (!gtk_combo_box_get_active_iter (combo_box, &iter))
		return;

	model = gtk_combo_box_get_model (combo_box);
	gtk_tree_model_get (model, &iter, COLUMN_ITEM, &item, -1);
	if (item == NULL)
		return;

	panel_profile_set_toplevel_orientation (dialog->toplevel,
						item->orientation);
}

static void
panel_properties_dialog_setup_orientation_combo_sensitivty (PanelPropertiesDialog *dialog, GtkToggleButton *n)
{
	gboolean expand;
	expand = panel_profile_get_toplevel_expand (dialog->toplevel);

	if (! panel_profile_key_is_writable (dialog->toplevel, PANEL_TOPLEVEL_ORIENTATION_KEY)) {
		gtk_widget_set_sensitive (dialog->orientation_combo, FALSE);
		gtk_widget_set_sensitive (dialog->orientation_label, FALSE);
		gtk_widget_show (dialog->writability_warn_general);
	}
	else {
		/* enable orientation only for non-expanded panels */
		gtk_widget_set_sensitive (dialog->orientation_combo, expand);
		gtk_widget_set_sensitive (dialog->orientation_label, expand);
	}
}

static void
panel_settings_toggle_hide_workspace_toggle (PanelPropertiesDialog *dialog,
				   GtkToggleButton       *toggle)
{
	char          *path;
	GSettings     *settings;
	path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
	settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);

	if (gtk_toggle_button_get_active (toggle)) {
		g_settings_set_boolean(settings, "hide-workspace",TRUE);
	}
	else {
		g_settings_set_boolean(settings, "hide-workspace",FALSE);
	}
}

static void
panel_settings_toggle_lock_toggle (PanelPropertiesDialog *dialog,
				   GtkToggleButton       *toggle)
{
	char          *path;
	GSettings     *settings;
	path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
	settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);

	if (gtk_toggle_button_get_active (toggle)) {
		g_settings_set_boolean(settings, "lock-taskbar",TRUE);
		gtk_combo_box_set_button_sensitivity (dialog->orientation_combo, GTK_SENSITIVITY_OFF);
	}
	else {
		g_settings_set_boolean(settings, "lock-taskbar",FALSE);
		gtk_combo_box_set_button_sensitivity (dialog->orientation_combo, GTK_SENSITIVITY_ON);
	}

	
}

static void
panel_properties_dialog_setup_hide_workspace (PanelPropertiesDialog *dialog,
                                             GtkBuilder            *gui)
{
	char          *path;
	GSettings     *settings;

	dialog->hide_workspace = PANEL_GTK_BUILDER_GET (gui, "hide_workspace");
	path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
	settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);
	gboolean hide_workspace = g_settings_get_boolean(settings, "hide-workspace");
	
	if (hide_workspace){
                gtk_toggle_button_set_active(dialog->hide_workspace,TRUE);
        } else{
                gtk_toggle_button_set_active(dialog->hide_workspace,FALSE);
        }

	g_signal_connect_swapped (dialog->hide_workspace, "toggled",
				  G_CALLBACK (panel_settings_toggle_hide_workspace_toggle),
				  dialog);
}

static void
panel_properties_dialog_setup_lock_toggle (PanelPropertiesDialog *dialog,
					     GtkBuilder            *gui)
{
	char          *path;
	GSettings     *settings;

	dialog->lock_toggle = PANEL_GTK_BUILDER_GET (gui, "lock_toggle");	
	path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
	settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);
	gboolean lock_taskbar = g_settings_get_boolean(settings, "lock-taskbar");

	if (lock_taskbar){
		gtk_toggle_button_set_active(dialog->lock_toggle,TRUE);
	} else{
		gtk_toggle_button_set_active(dialog->lock_toggle,FALSE);
	}

	if (gtk_toggle_button_get_active (dialog->lock_toggle)) {
		gtk_combo_box_set_button_sensitivity (dialog->orientation_combo, GTK_SENSITIVITY_OFF);
	}
	else {
		gtk_combo_box_set_button_sensitivity (dialog->orientation_combo, GTK_SENSITIVITY_ON);
	}

	g_signal_connect_swapped (dialog->lock_toggle, "toggled",
				  G_CALLBACK (panel_settings_toggle_lock_toggle),
				  dialog);
}

static void
panel_properties_dialog_setup_orientation_combo (PanelPropertiesDialog *dialog,
						 GtkBuilder            *gui)
{
	PanelOrientation  orientation;
	GtkListStore     *model;
	GtkTreeIter       iter;
	GtkCellRenderer  *renderer;
	int               i;

	dialog->orientation_combo = PANEL_GTK_BUILDER_GET (gui, "orientation_combo");
	g_return_if_fail (dialog->orientation_combo != NULL);
	dialog->orientation_label = PANEL_GTK_BUILDER_GET (gui, "orientation_label");
	g_return_if_fail (dialog->orientation_label != NULL);

	orientation = panel_profile_get_toplevel_orientation (dialog->toplevel);

	model = gtk_list_store_new (NUMBER_COLUMNS,
				    G_TYPE_STRING,
				    G_TYPE_POINTER);

	gtk_combo_box_set_model (GTK_COMBO_BOX (dialog->orientation_combo),
				 GTK_TREE_MODEL (model));

	for (i = 0; i < G_N_ELEMENTS (orientation_items); i++) {
		gtk_list_store_append (model, &iter);
		gtk_list_store_set (model, &iter,
				    COLUMN_TEXT, g_dpgettext2 (NULL, "Orientation", orientation_items [i].name),
				    COLUMN_ITEM, &(orientation_items [i]),
				    -1);
		if (orientation == orientation_items [i].orientation)
			gtk_combo_box_set_active_iter (GTK_COMBO_BOX (dialog->orientation_combo),
						       &iter);
	}

	renderer = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (dialog->orientation_combo),
				    renderer, TRUE);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (dialog->orientation_combo),
					renderer, "text", COLUMN_TEXT, NULL);

	g_signal_connect_swapped (dialog->orientation_combo, "changed",
				  G_CALLBACK (panel_properties_dialog_orientation_changed),
				  dialog);

	panel_properties_dialog_setup_orientation_combo_sensitivty (dialog, NULL);
}

static void
panel_properties_dialog_size_changed (PanelPropertiesDialog *dialog,
				      GtkSpinButton         *spin_button)
{
	panel_profile_set_toplevel_size (dialog->toplevel,
					 gtk_spin_button_get_value_as_int (spin_button));
}

static void
panel_properties_dialog_setup_size_spin (PanelPropertiesDialog *dialog,
					 GtkBuilder            *gui)
{
	dialog->size_widgets = PANEL_GTK_BUILDER_GET (gui, "size_widgets");
	g_return_if_fail (dialog->size_widgets != NULL);
	dialog->size_spin = PANEL_GTK_BUILDER_GET (gui, "size_spin");
	g_return_if_fail (dialog->size_spin != NULL);
	dialog->size_label = PANEL_GTK_BUILDER_GET (gui, "size_label");
	g_return_if_fail (dialog->size_label != NULL);
	dialog->size_label_pixels = PANEL_GTK_BUILDER_GET (gui, "size_label_pixels");
	g_return_if_fail (dialog->size_label_pixels != NULL);

	gtk_spin_button_set_range (GTK_SPIN_BUTTON (dialog->size_spin),
				   panel_toplevel_get_minimum_size (dialog->toplevel),
				   panel_toplevel_get_maximum_size (dialog->toplevel));

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (dialog->size_spin),
				   panel_profile_get_toplevel_size (dialog->toplevel));

	g_signal_connect_swapped (dialog->size_spin, "value_changed",
				  G_CALLBACK (panel_properties_dialog_size_changed),
				  dialog);

	if ( ! panel_profile_key_is_writable (dialog->toplevel, PANEL_TOPLEVEL_SIZE_KEY)) {
		gtk_widget_set_sensitive (dialog->size_spin, FALSE);
		gtk_widget_set_sensitive (dialog->size_label, FALSE);
		gtk_widget_set_sensitive (dialog->size_label_pixels, FALSE);
		gtk_widget_show (dialog->writability_warn_general);
	}
}

static void
panel_properties_dialog_icon_changed (PanelIconChooser      *chooser,
				      const char            *icon,
				      PanelPropertiesDialog *dialog)
{
        panel_profile_set_attached_custom_icon (dialog->toplevel, icon);
}

static void
panel_properties_dialog_setup_icon_chooser (PanelPropertiesDialog *dialog,
					    GtkBuilder            *gui)
{
	char *custom_icon;

	dialog->icon_align = PANEL_GTK_BUILDER_GET (gui, "icon_align");
	g_return_if_fail (dialog->icon_align != NULL);

	dialog->icon_chooser = panel_icon_chooser_new (NULL);
	panel_icon_chooser_set_fallback_icon_name (PANEL_ICON_CHOOSER (dialog->icon_chooser),
						   PANEL_ICON_DRAWER);
	gtk_widget_show (dialog->icon_chooser);
	gtk_container_add (GTK_CONTAINER (dialog->icon_align),
			   dialog->icon_chooser);

	dialog->icon_label = PANEL_GTK_BUILDER_GET (gui, "icon_label");
	g_return_if_fail (dialog->icon_label != NULL);

	custom_icon = panel_profile_get_attached_custom_icon (dialog->toplevel);
	panel_icon_chooser_set_icon (PANEL_ICON_CHOOSER (dialog->icon_chooser),
				     custom_icon);
	g_free (custom_icon);

	g_signal_connect (dialog->icon_chooser, "changed",
			  G_CALLBACK (panel_properties_dialog_icon_changed), dialog);

	if (!panel_profile_is_writable_attached_custom_icon (dialog->toplevel)) {
		gtk_widget_set_sensitive (dialog->icon_chooser, FALSE);
		gtk_widget_set_sensitive (dialog->icon_label, FALSE);
		if (panel_toplevel_get_is_attached (dialog->toplevel))
			gtk_widget_show (dialog->writability_warn_general);
	}
}

/* Note: this is only for toggle buttons on the general page, if needed for togglebuttons
   elsewhere you must make this respect the writability warning thing for the right page */
#define SETUP_TOGGLE_BUTTON(wid, n, p, key)                                                            \
	static void                                                                               \
	panel_properties_dialog_##n (PanelPropertiesDialog *dialog,                               \
				     GtkToggleButton       *n)                                    \
	{                                                                                         \
		panel_profile_set_toplevel_##p (dialog->toplevel,                                 \
						gtk_toggle_button_get_active (n));                \
	}                                                                                         \
	static void                                                                               \
	panel_properties_dialog_setup_##n (PanelPropertiesDialog *dialog,                         \
					   GtkBuilder            *gui)                            \
	{                                                                                         \
		dialog->n = PANEL_GTK_BUILDER_GET (gui, wid);                                      \
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog->n),                      \
					      panel_profile_get_toplevel_##p (dialog->toplevel)); \
		g_signal_connect_swapped (dialog->n, "toggled",                                   \
					  G_CALLBACK (panel_properties_dialog_##n), dialog);      \
		if ( ! panel_profile_key_is_writable (dialog->toplevel, key)) {               \
			gtk_widget_set_sensitive (dialog->n, FALSE);                              \
			gtk_widget_show (dialog->writability_warn_general);			  \
		}										  \
	}

SETUP_TOGGLE_BUTTON ("expand_toggle",      expand_toggle,      expand,           PANEL_TOPLEVEL_EXPAND_KEY)
SETUP_TOGGLE_BUTTON ("autohide_toggle",    autohide_toggle,    auto_hide,        PANEL_TOPLEVEL_AUTO_HIDE_KEY)
SETUP_TOGGLE_BUTTON ("hidebuttons_toggle", hidebuttons_toggle, enable_buttons,   PANEL_TOPLEVEL_ENABLE_BUTTONS_KEY)
SETUP_TOGGLE_BUTTON ("arrows_toggle",      arrows_toggle,      enable_arrows,    PANEL_TOPLEVEL_ENABLE_ARROWS_KEY)

static void
panel_properties_dialog_color_changed (PanelPropertiesDialog *dialog,
				       GtkColorChooser       *color_button)
{
	GdkRGBA color;

	g_assert (dialog->color_button == GTK_WIDGET (color_button));

	gtk_color_chooser_get_rgba (color_button, &color);
	panel_profile_set_background_gdk_rgba (dialog->toplevel, &color);
	panel_properties_dialog_opacity_changed (dialog);
}

static void
panel_properties_dialog_setup_color_button (PanelPropertiesDialog *dialog,
					    GtkBuilder            *gui)
{
	GdkRGBA color;

	dialog->color_button = PANEL_GTK_BUILDER_GET (gui, "color_button");
	g_return_if_fail (dialog->color_button != NULL);
	dialog->color_label = PANEL_GTK_BUILDER_GET (gui, "color_label");
	g_return_if_fail (dialog->color_label != NULL);

	panel_profile_get_background_color (dialog->toplevel, &color);

	gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog->color_button),
				     &color);

	g_signal_connect_swapped (dialog->color_button, "color_set",
				  G_CALLBACK (panel_properties_dialog_color_changed),
				  dialog);

	if ( ! panel_profile_background_key_is_writable (dialog->toplevel, "color")) {
		gtk_widget_set_sensitive (dialog->color_button, FALSE);
		gtk_widget_set_sensitive (dialog->color_label, FALSE);
		gtk_widget_show (dialog->writability_warn_background);
	}
}

static void
panel_properties_dialog_image_changed (PanelPropertiesDialog *dialog)
{
	char *image;

	image = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog->image_chooser));

	/* FIXME: This is an ugly workaround for GTK+ bug #327243.
	 * FIXME: Note that GTK+ 2.12 and file-set signal might help. */
	if (! (dialog->selection_emitted < 2) && !image) {
		dialog->selection_emitted++;
		return;
	}
	panel_profile_set_background_image (dialog->toplevel, image);

	g_free (image);
}

static void
panel_properties_dialog_setup_image_chooser (PanelPropertiesDialog *dialog,
					     GtkBuilder            *gui)
{
	char *image;

	dialog->image_chooser = PANEL_GTK_BUILDER_GET (gui, "image_chooser");
	panel_gtk_file_chooser_add_image_preview (GTK_FILE_CHOOSER (dialog->image_chooser));

	image = panel_profile_get_background_image (dialog->toplevel);

	if (PANEL_GLIB_STR_EMPTY (image))
		gtk_file_chooser_unselect_all (GTK_FILE_CHOOSER (dialog->image_chooser));
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog->image_chooser),
					       image);
	
	if (image)
		g_free (image);

	dialog->selection_emitted = 0;
	g_signal_connect_swapped (dialog->image_chooser, "selection-changed",
				  G_CALLBACK (panel_properties_dialog_image_changed),
				  dialog);

	if ( ! panel_profile_background_key_is_writable (dialog->toplevel, "image")) {
		gtk_widget_set_sensitive (dialog->image_chooser, FALSE);
		gtk_widget_show (dialog->writability_warn_background);
	}
}

static void
panel_properties_dialog_opacity_changed (PanelPropertiesDialog *dialog)
{
	gdouble percentage;
	guint16 opacity;

	percentage = gtk_range_get_value (GTK_RANGE (dialog->opacity_scale));

	if (percentage >= 98)
		percentage = 100;
	else if (percentage <= 2)
		percentage = 0;

	opacity = (percentage / 100) * 65535;

	panel_profile_set_background_opacity (dialog->toplevel, opacity);
}

static void
panel_properties_dialog_upd_sensitivity (PanelPropertiesDialog *dialog,
					 PanelBackgroundType    background_type)
{
	gtk_widget_set_sensitive (dialog->color_widgets,
				  background_type == PANEL_BACK_COLOR);
	gtk_widget_set_sensitive (dialog->image_widgets,
				  background_type == PANEL_BACK_IMAGE);
}

GdkColor
get_border_color (char *color_name)
{
        GdkColor color;

        GObject *gs = (GObject *)gtk_settings_get_default ();
        GValue color_scheme_value = G_VALUE_INIT;
        g_value_init (&color_scheme_value, G_TYPE_STRING);
        g_object_get_property (gs, "gtk-color-scheme", &color_scheme_value);
        gchar *color_scheme = (char *)g_value_get_string (&color_scheme_value);
        gchar color_spec[16] = { 0 };
        char *needle = strstr(color_scheme, color_name);
        if (needle) {
                while (1) {
                        if (color_spec[0] != '#') {
                                color_spec[0] = *needle;
                                needle++;
                                continue;
                        }

                        if ((*needle >= 0x30 && *needle <= 0x39) ||
                            (*needle >= 0x41 && *needle <= 0x46) ||
                            (*needle >= 0x61 && *needle <= 0x66)) {
                                color_spec[strlen(color_spec)] = *needle;
                                needle++;
                        } else {
                                break;
                        }
                }
                gdk_color_parse (color_spec, &color);
        } else {
                gdk_color_parse ("#3B9DC5", &color);
        }

        return color;
}


static void
panel_settings_toggle_opacity_scale_toggle (PanelPropertiesDialog *dialog,
                                   GtkToggleButton       *toggle)
{
	guint16 opacity;
        char          *path;
        GSettings     *settings;	

        path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
        settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);	

	if (gtk_toggle_button_get_active (toggle)){
		g_settings_set_boolean(settings, "transparent",TRUE);
		PanelBackgroundType background_type = PANEL_BACK_NONE;
		background_type = PANEL_BACK_COLOR;
        	panel_profile_set_background_type (dialog->toplevel, background_type);		

		opacity = (80.000000 / 100) * 65535;
		panel_profile_set_background_opacity (dialog->toplevel, opacity);
	} else {
		g_settings_set_boolean(settings, "transparent",FALSE);
		PanelBackgroundType background_type = PANEL_BACK_NONE;
		background_type = PANEL_BACK_COLOR;
	        panel_profile_set_background_type (dialog->toplevel, background_type);		

		opacity = (100.000000 / 100) * 65535;
		panel_profile_set_background_opacity (dialog->toplevel, opacity);
	}
}

static void
panel_properties_dialog_setup_opacity_scale (PanelPropertiesDialog *dialog,
					     GtkBuilder            *gui)
{
	char 				*color_str;
        char          			*path;
	guint16 			 opacity;
        GSettings     			*settings;

//	GdkColor color = get_border_color ("panel_normal_bg_color");
	GdkColor color;
	GtkStyle *style=gtk_rc_get_style(dialog->toplevel);
	gtk_style_lookup_color (style,"panel_normal_bg_color",&color);

	color_str=gdk_color_to_string(&color);
	g_settings_set_string (dialog->toplevel->background_settings, "color", color_str);

	dialog->opacity_scale = PANEL_GTK_BUILDER_GET (gui, "opacity_scale");
	g_return_if_fail (dialog->opacity_scale != NULL);


        path = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
        settings = g_settings_new_with_path ("org.ukui.panel.toplevel",path);
        gboolean transparent = g_settings_get_boolean(settings, "transparent");		

        if (transparent){
                gtk_toggle_button_set_active(dialog->opacity_scale,TRUE);
        } else{
                gtk_toggle_button_set_active(dialog->opacity_scale,FALSE);
        }	

	if (gtk_toggle_button_get_active (dialog->opacity_scale)){
		PanelBackgroundType background_type = PANEL_BACK_NONE;
		background_type = PANEL_BACK_COLOR;
        	panel_profile_set_background_type (dialog->toplevel, background_type);		

		opacity = (80.000000 / 100) * 65535;
		panel_profile_set_background_opacity (dialog->toplevel, opacity);
	} else {
		PanelBackgroundType background_type = PANEL_BACK_NONE;
		background_type = PANEL_BACK_COLOR;
        	panel_profile_set_background_type (dialog->toplevel, background_type);		

		opacity = (100.000000 / 100) * 65535;
		panel_profile_set_background_opacity (dialog->toplevel, opacity);
	}

	g_signal_connect_swapped (dialog->opacity_scale, "toggled",
				  G_CALLBACK (panel_settings_toggle_opacity_scale_toggle),
				  dialog);

/*	dialog->opacity_label = PANEL_GTK_BUILDER_GET (gui, "opacity_label");
	g_return_if_fail (dialog->opacity_label != NULL);
	dialog->opacity_legend = PANEL_GTK_BUILDER_GET (gui, "opacity_legend");
	g_return_if_fail (dialog->opacity_legend != NULL);

	opacity = panel_profile_get_background_opacity (dialog->toplevel);

	percentage = (opacity * 100.0) / 65535;

	gtk_range_set_value (GTK_RANGE (dialog->opacity_scale), percentage);

	g_signal_connect_swapped (dialog->opacity_scale, "value_changed",
				  G_CALLBACK (panel_properties_dialog_opacity_changed),
				  dialog);

	if ( ! panel_profile_background_key_is_writable (dialog->toplevel, "opacity")) {
		gtk_widget_set_sensitive (dialog->opacity_scale, FALSE);
		gtk_widget_set_sensitive (dialog->opacity_label, FALSE);
		gtk_widget_set_sensitive (dialog->opacity_legend, FALSE);
		gtk_widget_show (dialog->writability_warn_background);
	}
*/	
}

static void
panel_properties_dialog_background_toggled (PanelPropertiesDialog *dialog,
					    GtkWidget             *radio)
{
	PanelBackgroundType background_type = PANEL_BACK_NONE;

	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radio)))
		return;

	if (radio == dialog->default_radio)
		background_type = PANEL_BACK_NONE;

	else if (radio == dialog->color_radio)
		background_type = PANEL_BACK_COLOR;

	else if (radio == dialog->image_radio)
		background_type = PANEL_BACK_IMAGE;

	panel_properties_dialog_upd_sensitivity (dialog, background_type);

	panel_profile_set_background_type (dialog->toplevel, background_type);
}

static void
panel_settings_dialog_height_size_toggled (PanelPropertiesDialog *dialog,
					   GtkWidget             *radio)
{
	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radio)))
		return;

	if (radio == dialog->default_height_size_radio){
		panel_profile_set_toplevel_size (dialog->toplevel,40);
	}
	else if (radio == dialog->medium_height_size_radio){
		panel_profile_set_toplevel_size (dialog->toplevel,60);
	}

	else if (radio == dialog->large_height_size_radio){
		panel_profile_set_toplevel_size (dialog->toplevel,80);
	}
}
static void
cell_edited (GtkCellRendererText *cell,
             const gchar         *path_string,
             const gchar         *new_text,
             gpointer             data)
{
  int 		 i=1,
		 number;
  char 		*path2, 
		*applet_name,
		*applet_name_value;

  GSettings 	*settings;
  const gchar 	*show = "显示", *hide = "隐藏";

  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  GtkTreeIter iter;

  gint column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));

  gtk_tree_model_get_iter (model, &iter, path);

  gtk_list_store_set (data, &iter, 2,
                      new_text, -1);

  gtk_tree_model_get(model, &iter,1,&applet_name_value,-1);

  while(i<100){

  	path2 = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", i);

  	settings = g_settings_new_with_path( "org.ukui.panel.indicator.tray", path2);

	applet_name=g_settings_get_string (settings,"applet-name");

	number=g_settings_get_int (settings,"number");

	if (strcmp (applet_name_value,applet_name) == 0 && number!=0 && number!=-1){

  		if (strcmp (new_text,show) == 0 ){
  			g_settings_set_boolean (settings,"show",TRUE);
  		} else{
  			g_settings_set_boolean (settings,"show",FALSE);
 	 	}

	}

	if (number == 0) 
		break;
  	i++;
  }

  gtk_tree_path_free (path);
}

static gboolean
tray_widget_show_notify (GSettings             *settings,
			 gchar                 *key,
			 GtkListStore          *store)
{
	GSettings             *settings1;

	if (strcpy (key,"number")){
		int 			k, number;
		GtkTreeIter 		iter;
		char 			*path, *applet_name, *applet_icon;
		char 			*show="显示";
		char 			*hide="隐藏";
		GdkPixbuf       	*icon;
		GError          	*error = NULL;
		GtkIconTheme		*icon_theme = gtk_icon_theme_get_default ();

		number = 		g_settings_get_int(settings, "number");
		applet_name = 		g_settings_get_string (settings, "applet-name");
		applet_icon = 		g_settings_get_string (settings, "applet-icon");

//	gtk_list_store_clear(store);
	  	if(number==-1 || number==0 ){
			if(add == 1){
				gboolean show_value = 	g_settings_get_boolean (settings, "show");
				icon = 			gtk_icon_theme_load_icon (icon_theme,
				 	 	  		 		 applet_icon,
				    				 		 16,
				    		 				 0,
				    		 				 NULL);
//		if(applet_name != NULL && strcmp(applet_name,"ukui") != 0){
				gtk_list_store_append (store, &iter);
				if ( show_value ){
					gtk_list_store_set (store, &iter, 0, icon,
							 		  1, applet_name,
							 		  2, show, -1);
				}else {
					gtk_list_store_set (store, &iter, 0, icon,
							 		  1, applet_name,
							 		  2, hide, -1);
				}
//		}
			add = 0;
			Applet_Name = applet_name;
			} else{
				gtk_list_store_clear(store);
				for(int i = 0; i < 100; i ++){
					k = i + 1;
					path = 			g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", k);
					settings1 = 		g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);
					number=			g_settings_get_int(settings1, "number");

	  				if(number!=-1 && number!=0){
						applet_name = 		g_settings_get_string (settings1, "applet-name");
						applet_icon = 		g_settings_get_string (settings1, "applet-icon");
						gboolean show_value = 	g_settings_get_boolean (settings1, "show");
						icon = 			gtk_icon_theme_load_icon (icon_theme,
				    		 						  applet_icon,
				    		 						  16,
				    		 						  0,
				    		 						  NULL);
//		if(applet_name != NULL && strcmp(applet_name,"ukui") != 0){
						gtk_list_store_append (store, &iter);
						if ( show_value ){
							gtk_list_store_set (store, &iter, 0, icon,
							 			   	  1, applet_name,
							 			  	  2, show, -1);
						}else {
							gtk_list_store_set (store, &iter, 0, icon,
							 				  1, applet_name,
							 				  2, hide, -1);
						}
					}

//		}
				}

			}
		}
		else{
			if(strcmp( Applet_Name, applet_name ) != 0){
				add=1;
			}
		}
	return FALSE;

	}
}

static void
panel_settings_dialog_setup_notifcation_area_treeview (PanelPropertiesDialog *dialog,
					     GtkBuilder            *gui)
{
	int i = 0 ;
	int n = 100;
	char text[32] = {0};
	GtkWidget *notification_area_treeview;
	GtkTreeViewColumn* column;
	GtkCellRenderer* cell;
	GtkListStore* store, *store1;
	GtkTreeIter iter;
	GtkTreeIter iter1;
	GtkWidget *scrolledwindow;

	scrolledwindow=PANEL_GTK_BUILDER_GET 			(gui, "scrolledwindow");
	notification_area_treeview=PANEL_GTK_BUILDER_GET 	(gui, "notification_area_treeview");

	column = gtk_tree_view_column_new();
	gtk_tree_view_append_column(notification_area_treeview, column);
	
	cell = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (column, cell, TRUE);
	gtk_tree_view_column_set_attributes (column, cell, "pixbuf", 0, NULL);

	column = gtk_tree_view_column_new ();
	gtk_tree_view_append_column (notification_area_treeview, column);

 	cell = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, cell, TRUE);
	gtk_tree_view_column_set_attributes(column, cell, "text", 1, NULL);

	column = gtk_tree_view_column_new();
	gtk_tree_view_append_column(notification_area_treeview, column);

	char *show="显示";
	char *hide="隐藏";
	store = gtk_list_store_new(3, 
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING);
	gtk_tree_view_set_model(notification_area_treeview, GTK_TREE_MODEL(store));

//添加
	int 		 j = 1;
	char 		*path1;
	GSettings 	*settings1;
	while (j<100){
		path1 = 	g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", j);
		settings1 = 	g_settings_new_with_path ("org.ukui.panel.indicator.tray", path1);

//		g_signal_connect (settings1,
//				  "changed::" "number",
//				  G_CALLBACK (tray_widget_show_notify),
//				  store);
		j++;
	}


	int num = 1, number;
	GSettings *settings;
	char *path,*applet_name,*applet_icon;
	path = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", num);
	settings = g_settings_new_with_path( "org.ukui.panel.indicator.tray", path);
	number = g_settings_get_int (settings, "number");
	while( number != 0) {
		num=num+1;
		path=g_strdup_printf ("%s%d/","/org/ukui/panel/indicator/tray",num);
		settings = g_settings_new_with_path("org.ukui.panel.indicator.tray",path);
		number=g_settings_get_int(settings, "number");
	}

	num=num-1;
	int k;
	GdkPixbuf       *icon;
	GError          *error = NULL;
	GtkIconTheme	*icon_theme = gtk_icon_theme_get_default ();

	for(int i = 0; i < num; i ++){
		k = i + 1;
		path = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", k);
		settings = 		g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);
		number=g_settings_get_int(settings, "number");

	  	if(number!=-1){
			applet_name = 		g_settings_get_string (settings, "applet-name");
			applet_icon = 		g_settings_get_string (settings, "applet-icon");
			gboolean show_value = 	g_settings_get_boolean (settings, "show");
			icon = 			gtk_icon_theme_load_icon (icon_theme,
				    		 		applet_icon,
				    		 		16,
				    		 		0,
				    		 		NULL);
//		if(applet_name != NULL && strcmp(applet_name,"ukui") != 0){
			sprintf (text, "text%d", i);
			gtk_list_store_append (store, &iter);
			if (show_value){
				gtk_list_store_set (store, &iter, 0, icon,
							 1, applet_name,
							 2, show, -1);
			}else {
				gtk_list_store_set (store, &iter, 0, icon,
							 1, applet_name,
							 2, hide, -1);
			}
		}

//		}
	}

	store1 = gtk_list_store_new (3, 
				     G_TYPE_STRING,
				     G_TYPE_INT,
				     G_TYPE_STRING);

	gtk_list_store_append (store1, &iter);
	gtk_list_store_set (store1, &iter, 0, show,-1);
	gtk_list_store_append (store1, &iter);
	gtk_list_store_set (store1, &iter, 0, hide,-1);

	cell = gtk_cell_renderer_combo_new ();
	g_object_set (cell,
		      "model", GTK_TREE_MODEL(store1),
		      "text-column", 0,
		      "has-entry", FALSE,
		      "editable", TRUE,
		      NULL);
	g_signal_connect (cell, "edited",
			G_CALLBACK (cell_edited),store);

	gtk_tree_view_insert_column_with_attributes(notification_area_treeview,
						    -1, "", cell,
						    "text", 2,
						    NULL);
	gtk_widget_show (notification_area_treeview);

}

static void
panel_settings_dialog_manage_icons_label_activate_link (PanelPropertiesDialog *dialog,
							GtkLabel *label)
{
	GtkBuilder            *gui;
	GError                *error;
	PanelPropertiesDialog *panel_settings_dialog;
	error = NULL;
	gui = gtk_builder_new ();
	gtk_builder_set_translation_domain (gui, GETTEXT_PACKAGE);
	gtk_builder_add_from_file (gui,
				   BUILDERDIR "/notification-area.ui",
				   &error);
        if (error) {
		char *secondary;

		secondary = g_strdup_printf (_("Unable to load file '%s': %s."),
					     BUILDERDIR"/notification-area.ui",
					     error->message);
		panel_error_dialog (GTK_WINDOW (dialog->toplevel),
				    gtk_window_get_screen (GTK_WINDOW (dialog->toplevel)),
				    "cannot_display_notification_area_dialog", TRUE,
				    _("Could not notification area dialog"),
				    secondary);
		g_free (secondary);
		g_error_free (error);
		g_object_unref (gui);

//		return;
	}

	panel_settings_dialog=PANEL_GTK_BUILDER_GET 			(gui, "notification_area");
	panel_settings_dialog_setup_notifcation_area_treeview		(dialog, gui);
	gtk_widget_show (panel_settings_dialog);

	g_object_unref (gui);

}

static void
panel_settings_dialog_setup_manage_icons_label(PanelPropertiesDialog *dialog,
						 GtkBuilder            *gui)
{
	printf("panel_settingss_dialog_setup_manage_icons_label--\n");
	const gchar *text =
		_("<a href=''>Customize icons appear on the taskbar</a>");
	dialog->manage_icons_label      = PANEL_GTK_BUILDER_GET (gui,"manage_icons_label");
	gtk_label_set_markup (dialog->manage_icons_label, text);

	g_signal_connect_swapped (dialog->manage_icons_label, "activate-link",
				  G_CALLBACK (panel_settings_dialog_manage_icons_label_activate_link),
				  dialog);
}

static void
panel_properties_dialog_setup_taskbar_height_size_radios (PanelPropertiesDialog *dialog,
						 GtkBuilder            *gui)
{
	int                  profile_size;
	GtkWidget           *active_radio;

	profile_size = panel_profile_get_toplevel_size (dialog->toplevel);
	dialog->large_height_size_radio      = PANEL_GTK_BUILDER_GET (gui,"large_height_size_radio");
	dialog->medium_height_size_radio     = PANEL_GTK_BUILDER_GET (gui,"medium_height_size_radio");
	dialog->default_height_size_radio    = PANEL_GTK_BUILDER_GET (gui, "default_height_size_radio");

	switch (profile_size) {
	case DEFAULT_SIZE:
		active_radio = dialog->default_height_size_radio;
		break;
	case MEDIUM_SIZE:
		active_radio = dialog->medium_height_size_radio;
		break;
	case LARGE_SIZE:
		active_radio = dialog->large_height_size_radio;
		break;
	default:
		active_radio = NULL;
		g_assert_not_reached ();
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (active_radio), TRUE);

	g_signal_connect_swapped (dialog->large_height_size_radio, "toggled",
				  G_CALLBACK (panel_settings_dialog_height_size_toggled),
				  dialog);
	g_signal_connect_swapped (dialog->medium_height_size_radio, "toggled",
				  G_CALLBACK (panel_settings_dialog_height_size_toggled),
				  dialog);
	g_signal_connect_swapped (dialog->default_height_size_radio, "toggled",
				  G_CALLBACK (panel_settings_dialog_height_size_toggled),
				  dialog);
}

static void
panel_properties_dialog_setup_background_radios (PanelPropertiesDialog *dialog,
						 GtkBuilder            *gui)
{
	PanelBackgroundType  background_type;
	GtkWidget           *active_radio;

	dialog->default_radio     = PANEL_GTK_BUILDER_GET (gui, "default_radio");
	dialog->color_radio       = PANEL_GTK_BUILDER_GET (gui, "color_radio");
	dialog->image_radio       = PANEL_GTK_BUILDER_GET (gui, "image_radio");
	dialog->color_widgets     = PANEL_GTK_BUILDER_GET (gui, "color_widgets");
	dialog->image_widgets     = PANEL_GTK_BUILDER_GET (gui, "image_widgets");

	background_type = panel_profile_get_background_type (dialog->toplevel);
	switch (background_type) {
	case PANEL_BACK_NONE:
		active_radio = dialog->default_radio;
		break;
	case PANEL_BACK_COLOR:
		active_radio = dialog->color_radio;
		break;
	case PANEL_BACK_IMAGE:
		active_radio = dialog->image_radio;
		break;
	default:
		active_radio = NULL;
		g_assert_not_reached ();
	}

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (active_radio), TRUE);

	panel_properties_dialog_upd_sensitivity (dialog, background_type);

	g_signal_connect_swapped (dialog->default_radio, "toggled",
				  G_CALLBACK (panel_properties_dialog_background_toggled),
				  dialog);
	g_signal_connect_swapped (dialog->color_radio, "toggled",
				  G_CALLBACK (panel_properties_dialog_background_toggled),
				  dialog);
	g_signal_connect_swapped (dialog->image_radio, "toggled",
				  G_CALLBACK (panel_properties_dialog_background_toggled),
				  dialog);

	if ( ! panel_profile_background_key_is_writable (dialog->toplevel, "type")) {
		gtk_widget_set_sensitive (dialog->default_radio, FALSE);
		gtk_widget_set_sensitive (dialog->color_radio, FALSE);
		gtk_widget_set_sensitive (dialog->image_radio, FALSE);
		gtk_widget_show (dialog->writability_warn_background);
	}
}

static void
panel_properties_update_arrows_toggle_visible (PanelPropertiesDialog *dialog,
					       GtkToggleButton       *toggle)
{
	if (gtk_toggle_button_get_active (toggle))
		gtk_widget_set_sensitive (dialog->arrows_toggle,
					  panel_profile_key_is_writable (dialog->toplevel, PANEL_TOPLEVEL_ENABLE_ARROWS_KEY));
	else
		gtk_widget_set_sensitive (dialog->arrows_toggle, FALSE);
}

static void
panel_properties_dialog_response (PanelPropertiesDialog *dialog,
				  int                    response,
				  GtkWidget             *properties_dialog)
{
	printf("panel_properties_dialog_response\n");
	char *help_id;

	switch (response) {
	case GTK_RESPONSE_CLOSE:
		gtk_widget_destroy (properties_dialog);
		break;
	case GTK_RESPONSE_HELP:
		if (panel_toplevel_get_is_attached (dialog->toplevel)) {
			help_id = "gospanel-550";
		} else {
			help_id = "gospanel-28";
		}
		panel_show_help (gtk_window_get_screen (GTK_WINDOW (properties_dialog)),
				 "ukui-user-guide", help_id, NULL);
		break;
	default:
		break;
	}
}

static void
panel_properties_dialog_destroy (PanelPropertiesDialog *dialog)
{
	printf("panel_properties_dialog_destroy\n");
	panel_toplevel_pop_autohide_disabler (PANEL_TOPLEVEL (dialog->toplevel));
	g_object_set_qdata (G_OBJECT (dialog->toplevel),
			    panel_properties_dialog_quark,
			    NULL);
}

static void
panel_properties_dialog_update_orientation (PanelPropertiesDialog *dialog,
					    PanelOrientation       orientation)
{
	GtkTreeModel         *model;
	GtkTreeIter           iter;
	OrientationComboItem *item;
	int                   max_size;
	int                   spin_size;
	int                   profile_size;

	/* change the maximum size of the panel */
	//TODO: we should also do this when the monitor size changes
	max_size = panel_toplevel_get_maximum_size (dialog->toplevel);
	spin_size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (dialog->size_spin));
	profile_size = panel_profile_get_toplevel_size (dialog->toplevel);

	gtk_spin_button_set_range (GTK_SPIN_BUTTON (dialog->size_spin),
				   panel_toplevel_get_minimum_size (dialog->toplevel),
				   max_size);

	if (spin_size > max_size)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (dialog->size_spin),
					   max_size);
	else if (spin_size != profile_size)
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (dialog->size_spin),
					   MIN (profile_size, max_size));

	/* update the orientation combo box */
	model = gtk_combo_box_get_model (GTK_COMBO_BOX (dialog->orientation_combo));

	if (!gtk_tree_model_get_iter_first (model, &iter))
		return;

	do {
		gtk_tree_model_get (model, &iter, COLUMN_ITEM, &item, -1);
		if (item != NULL && item->orientation == orientation) {
			gtk_combo_box_set_active_iter (GTK_COMBO_BOX (dialog->orientation_combo),
						       &iter);
			return;
		}
	} while (gtk_tree_model_iter_next (model, &iter));
}

static void
panel_properties_dialog_update_size (PanelPropertiesDialog *dialog,
				     int size)
{
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (dialog->size_spin), size);
}

static void
panel_properties_dialog_toplevel_notify (GSettings             *settings,
					 gchar                 *key,
					 PanelPropertiesDialog *dialog)
{

#define UPDATE_TOGGLE(p, n)                                                                \
	if (!strcmp (key, p)) {                                                            \
		gboolean val = g_settings_get_boolean (settings, key);                     \
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog->n)) != val)   \
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog->n), val); \
	}

	if (!strcmp (key, "orientation"))
		panel_properties_dialog_update_orientation (dialog, g_settings_get_enum (settings, key));
	else if (!strcmp (key, "size"))
		panel_properties_dialog_update_size (dialog, g_settings_get_int (settings, key));
	else UPDATE_TOGGLE ("expand",         expand_toggle)
	else UPDATE_TOGGLE ("auto-hide",      autohide_toggle)
	else UPDATE_TOGGLE ("enable-buttons", hidebuttons_toggle)
	else UPDATE_TOGGLE ("enable-arrows",  arrows_toggle)
}

static void
panel_properties_dialog_update_background_type (PanelPropertiesDialog *dialog,
						PanelBackgroundType    background_type)
{
	GtkWidget           *active_radio;

	switch (background_type) {
	case PANEL_BACK_NONE:
		active_radio = dialog->default_radio;
		break;
	case PANEL_BACK_COLOR:
		active_radio = dialog->color_radio;
		break;
	case PANEL_BACK_IMAGE:
		active_radio = dialog->image_radio;
		break;
	default:
		active_radio = NULL;
		g_assert_not_reached ();
		break;
	}

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (active_radio), TRUE);
}

static void
panel_properties_dialog_update_background_color (PanelPropertiesDialog *dialog,
						 gchar                 *str_color)
{
	GdkRGBA new_color;
	GdkRGBA old_color;

	if (!gdk_rgba_parse (&new_color, str_color))
		return;

	gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (dialog->color_button),
				    &old_color);

	if (!gdk_rgba_equal (&old_color, &new_color))
		gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (dialog->color_button),
					    &new_color);					    
}

static void
panel_properties_dialog_update_background_image (PanelPropertiesDialog *dialog,
						 gchar                 *text)
{
	char       *old_text;

	old_text = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog->image_chooser));

	if (PANEL_GLIB_STR_EMPTY (text) && old_text)
		gtk_file_chooser_unselect_all (GTK_FILE_CHOOSER (dialog->image_chooser));
	else if (!PANEL_GLIB_STR_EMPTY (text) &&
		 (!old_text || strcmp (text, old_text)))
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog->image_chooser),
					       text);

	if (old_text)
		g_free (old_text);
}

static void
panel_properties_dialog_background_notify (GSettings             *settings,
					   gchar                 *key,
					   PanelPropertiesDialog *dialog)
{
	if (!strcmp (key, "type"))
	{
		PanelBackgroundType type = g_settings_get_enum (settings, key);
		panel_properties_dialog_update_background_type (dialog, type);
	}
	else if (!strcmp (key, "color"))
	{
		char *color = g_settings_get_string (settings, key);
		panel_properties_dialog_update_background_color (dialog, color);
		g_free (color);
	}
	else if (!strcmp (key, "image"))
	{
		char *image = g_settings_get_string (settings, key);
		panel_properties_dialog_update_background_image (dialog, image);
		g_free (image);
	}
}

static void
panel_properties_dialog_remove_orientation_combo (PanelPropertiesDialog *dialog)
{
	GtkContainer *container = GTK_CONTAINER (dialog->general_table);
	GtkGrid      *grid      = GTK_GRID (dialog->general_table);

	g_object_ref (dialog->size_label);
	g_object_ref (dialog->size_widgets);
	g_object_ref (dialog->icon_label);
	g_object_ref (dialog->icon_align);

	gtk_container_remove (container, dialog->orientation_label);
	gtk_container_remove (container, dialog->orientation_combo);
	gtk_container_remove (container, dialog->size_label);
	gtk_container_remove (container, dialog->size_widgets);
	gtk_container_remove (container, dialog->icon_label);
	gtk_container_remove (container, dialog->icon_align);

	gtk_grid_attach (grid, dialog->size_label,   0, 1, 1, 1);
	gtk_grid_attach (grid, dialog->size_widgets, 1, 1, 1, 1);
	gtk_grid_attach (grid, dialog->icon_label,   0, 2, 1, 1);
	gtk_grid_attach (grid, dialog->icon_align,   1, 2, 1, 1);

	dialog->orientation_label = NULL;
	dialog->orientation_combo = NULL;
	g_object_unref (dialog->size_label);
	g_object_unref (dialog->size_widgets);
	g_object_unref (dialog->icon_label);
	g_object_unref (dialog->icon_align);
}

static void
panel_properties_dialog_remove_icon_chooser (PanelPropertiesDialog *dialog)
{
	GtkContainer *container = GTK_CONTAINER (dialog->general_table);

	gtk_container_remove (container, dialog->icon_label);
	gtk_container_remove (container, dialog->icon_align);

	dialog->icon_label = NULL;
	dialog->icon_align = NULL;
	dialog->icon_chooser = NULL;
}

static void
panel_properties_dialog_remove_toggles (PanelPropertiesDialog *dialog)
{
	GtkContainer *container = GTK_CONTAINER (dialog->general_vbox);

	gtk_container_remove (container, dialog->autohide_toggle);
	gtk_container_remove (container, dialog->expand_toggle);

	dialog->autohide_toggle = NULL;
	dialog->expand_toggle   = NULL;
}

static void
panel_properties_dialog_update_for_attached (PanelPropertiesDialog *dialog,
					     gboolean               attached)
{
	if (!attached)
		panel_properties_dialog_remove_icon_chooser (dialog);
	else {
		gtk_window_set_title (GTK_WINDOW (dialog->properties_dialog),
				      _("Drawer Properties"));
		panel_properties_dialog_remove_toggles (dialog);
		panel_properties_dialog_remove_orientation_combo (dialog);
	}
}

static PanelPropertiesDialog *
panel_properties_dialog_new (PanelToplevel *toplevel,
			     GtkBuilder    *gui)
{
	PanelPropertiesDialog *dialog;
	char                  *toplevel_settings_path;

	dialog = g_new0 (PanelPropertiesDialog, 1);

	g_object_set_qdata_full (G_OBJECT (toplevel),
				 panel_properties_dialog_quark,
				 dialog,
				 (GDestroyNotify) panel_properties_dialog_free);

	dialog->toplevel = toplevel;

	dialog->properties_dialog = PANEL_GTK_BUILDER_GET (gui, "panel_settings_dialog");
	g_signal_connect_swapped (dialog->properties_dialog, "response",
				  G_CALLBACK (panel_properties_dialog_response), dialog);
	g_signal_connect_swapped (dialog->properties_dialog, "destroy",
				  G_CALLBACK (panel_properties_dialog_destroy), dialog);

	gtk_window_set_screen (GTK_WINDOW (dialog->properties_dialog),
			       gtk_window_get_screen (GTK_WINDOW (toplevel)));

	dialog->writability_warn_general = PANEL_GTK_BUILDER_GET (gui, "writability_warn_general");
	dialog->writability_warn_background = PANEL_GTK_BUILDER_GET (gui, "writability_warn_background");

	dialog->general_vbox  = PANEL_GTK_BUILDER_GET (gui, "general_vbox");
	dialog->general_table = PANEL_GTK_BUILDER_GET (gui, "general_table");

	panel_properties_dialog_setup_orientation_combo  (dialog, gui);
	panel_properties_dialog_setup_size_spin          (dialog, gui);
	panel_properties_dialog_setup_icon_chooser       (dialog, gui);
	panel_properties_dialog_setup_expand_toggle      (dialog, gui);
	panel_properties_dialog_setup_autohide_toggle    (dialog, gui);
	panel_properties_dialog_setup_hidebuttons_toggle (dialog, gui);
	panel_properties_dialog_setup_arrows_toggle      (dialog, gui);
	panel_properties_dialog_setup_lock_toggle        (dialog, gui);
	panel_properties_dialog_setup_hide_workspace     (dialog, gui);

	g_signal_connect_swapped (dialog->expand_toggle, "toggled",
				  G_CALLBACK (panel_properties_dialog_setup_orientation_combo_sensitivty), dialog);

	panel_properties_update_arrows_toggle_visible (
		dialog, GTK_TOGGLE_BUTTON (dialog->hidebuttons_toggle));
	g_signal_connect_swapped (dialog->hidebuttons_toggle, "toggled",
				  G_CALLBACK (panel_properties_update_arrows_toggle_visible),
				  dialog);

	g_object_get (toplevel, "settings-path", &toplevel_settings_path, NULL);
	dialog->settings = g_settings_new_with_path (PANEL_TOPLEVEL_SCHEMA,
						     toplevel_settings_path);
	gchar *toplevel_background_path;
	toplevel_background_path = g_strdup_printf ("%sbackground/", toplevel_settings_path);
	dialog->background_settings = g_settings_new_with_path (PANEL_TOPLEVEL_BACKGROUND_SCHEMA,
								toplevel_background_path);
	g_free (toplevel_background_path);

	g_signal_connect (dialog->settings,
			  "changed",
			  G_CALLBACK (panel_properties_dialog_toplevel_notify),
			  dialog);

	panel_properties_dialog_setup_color_button      		(dialog, gui);
	panel_properties_dialog_setup_image_chooser     		(dialog, gui);
	panel_properties_dialog_setup_opacity_scale     		(dialog, gui);
	panel_properties_dialog_setup_background_radios 		(dialog, gui);
	panel_properties_dialog_setup_taskbar_height_size_radios 	(dialog, gui);
	panel_settings_dialog_setup_manage_icons_label      		(dialog, gui);

	g_signal_connect (dialog->background_settings,
			  "changed",
			  G_CALLBACK (panel_properties_dialog_background_notify),
			  dialog);

	panel_properties_dialog_update_for_attached (dialog,
						     panel_toplevel_get_is_attached (dialog->toplevel));

	panel_toplevel_push_autohide_disabler (dialog->toplevel);
	panel_widget_register_open_dialog (panel_toplevel_get_panel_widget (dialog->toplevel),
					   dialog->properties_dialog);

	gtk_widget_show (dialog->properties_dialog);

	return dialog;
}

void
panel_settings_dialog_present (PanelToplevel *toplevel)
{
	PanelPropertiesDialog *dialog;
	GtkBuilder            *gui;
	GError                *error;

	if (!panel_properties_dialog_quark)
		panel_properties_dialog_quark =
			g_quark_from_static_string ("panel-settings-dialog");

	dialog = g_object_get_qdata (G_OBJECT (toplevel), panel_properties_dialog_quark);
	if (dialog) {
		gtk_window_set_screen (GTK_WINDOW (dialog->properties_dialog),
				       gtk_window_get_screen (GTK_WINDOW (toplevel)));
		gtk_window_present (GTK_WINDOW (dialog->properties_dialog));
		return;
	}

	gui = gtk_builder_new ();
	gtk_builder_set_translation_domain (gui, GETTEXT_PACKAGE);

	error = NULL;
	gtk_builder_add_from_file (gui,
				   BUILDERDIR "/panel-settings-dialog.ui",
				   &error);

        if (error) {
		char *secondary;

		secondary = g_strdup_printf (_("Unable to load file '%s': %s."),
					     BUILDERDIR"/panel-settings-dialog.ui",
					     error->message);
		panel_error_dialog (GTK_WINDOW (toplevel),
				    gtk_window_get_screen (GTK_WINDOW (toplevel)),
				    "cannot_display_properties_dialog", TRUE,
				    _("Could not display properties dialog"),
				    secondary);
		g_free (secondary);
		g_error_free (error);
		g_object_unref (gui);

		return;
	}

	dialog = panel_properties_dialog_new (toplevel, gui);

	g_object_unref (gui);
}
