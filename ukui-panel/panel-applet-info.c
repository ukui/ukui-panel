/*
 * panel-applet-info.c
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2010 Vincent Untz <vuntz@gnome.org>
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
 */

#include <config.h>

#include "panel-applet-info.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _UkuiPanelAppletInfo {
	gchar  *iid;

	gchar  *name;
	gchar  *comment;
	gchar  *icon;

	gchar **old_ids;
};

UkuiPanelAppletInfo *
ukui_panel_applet_info_new (const gchar  *iid,
		       const gchar  *name,
		       const gchar  *comment,
		       const gchar  *icon,
		       const gchar **old_ids)
{
	UkuiPanelAppletInfo *info;
	int len;

	info = g_slice_new0 (UkuiPanelAppletInfo);

	info->iid = g_strdup (iid);
	info->name = g_strdup (name);
	info->comment = g_strdup (comment);
	info->icon = g_strdup (icon);

	/* UkuiComponent compatibility */
	if (old_ids != NULL) {
		len = g_strv_length ((gchar **) old_ids);
		if (len > 0) {
			int i;

			info->old_ids = g_new0 (gchar *, len + 1);

			for (i = 0; i < len; i++)
				info->old_ids[i] = g_strdup (old_ids[i]);
		}
	}

	return info;
}

void
ukui_panel_applet_info_free (UkuiPanelAppletInfo *info)
{
	if (!info)
		return;

	g_free (info->iid);
	g_free (info->name);
	g_free (info->comment);
	g_free (info->icon);
	g_strfreev (info->old_ids);

	g_slice_free (UkuiPanelAppletInfo, info);
}

const gchar *
ukui_panel_applet_info_get_iid (UkuiPanelAppletInfo *info)
{
	return info->iid;
}

const gchar *
ukui_panel_applet_info_get_name (UkuiPanelAppletInfo *info)
{
	return info->name;
}

const gchar *
ukui_panel_applet_info_get_description (UkuiPanelAppletInfo *info)
{
	return info->comment;
}

const gchar *
ukui_panel_applet_info_get_icon (UkuiPanelAppletInfo *info)
{
	return info->icon;
}

const gchar * const *
ukui_panel_applet_info_get_old_ids (UkuiPanelAppletInfo *info)
{
	return (const gchar * const *) info->old_ids;
}
/*
void StrReplace (char	*strSrc, 
		 char 	*strFind, 
		 char 	*strReplace)
{
	char 	*q;
	int	i,j;
	while (*strSrc != '\0') {
		if (*strSrc == *strFind) {
			if (strncmp (strSrc,strFind,strlen (strFind)) == 0 ) {
				i = strlen (strFind);
				j = strlen (strReplace);
				q = strSrc + i;
				while ((*strSrc ++= *strReplace++) != '\0');
				while ((*strSrc ++= *q++) != '\0');
			}
			else {
				 strSrc++;
			}
		}
		else {
			strSrc++;
		}
	}
}
*/

static char * StrReplace(char const * const original, char const * const pattern, char const * const replacement) 
{
  size_t const replen = strlen(replacement);
  size_t const patlen = strlen(pattern);
  size_t const orilen = strlen(original);

  size_t patcnt = 0;
  const char * oriptr;
  const char * patloc;

  for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen)
  {
    patcnt++;
  }

  {
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );

    if (returned != NULL)
    {
      char * retptr = returned;
      for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen)
      {
        size_t const skplen = patloc - oriptr;
        strncpy(retptr, oriptr, skplen);
        retptr += skplen;
        strncpy(retptr, replacement, replen);
        retptr += replen;
      }
      strcpy(retptr, oriptr);
    }
    return returned;
  }
}

int WriteAppletInfo (char	*action,
		     char	*launcher_location)
{
	int flen;
	FILE *fp;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	char	*home,
		*Name,
		*state,
		*filename,
		*Language,
		*file_content="",
		*desktopFile,
		home_applet[100],
		config_desktop_filename[100],
		autostart_desktop_filename[100],
		applications_desktop_filename[100];
	state = action;
	filename = launcher_location;

	home = getenv ("HOME");
	keyfile = g_key_file_new ();
	Language = getenv ("GDM_LANG");

	if (filename[0] == '/') {
		desktopFile = launcher_location;
	} 
	else { 
		sprintf (autostart_desktop_filename, "/etc/xdg/autostart/%s", filename);
		sprintf (applications_desktop_filename, "/usr/share/applications/%s", filename);
		sprintf (applications_desktop_filename, "/usr/share/applications/%s", filename);
		sprintf (applications_desktop_filename, "/usr/share/applications/%s", filename);
		sprintf (config_desktop_filename, "%s/.config/ukui/panel2.d/default/launchers/%s", home,filename);
	}

	if (Language == NULL) {
		Language = "";
	}

	if (!access (applications_desktop_filename, 0)) {
		desktopFile = applications_desktop_filename;
	} else if (!access (config_desktop_filename, 0)) {
		desktopFile = config_desktop_filename;
	} else if (!access (autostart_desktop_filename,0)) {
		desktopFile = autostart_desktop_filename;
	}

	if (!g_key_file_load_from_file (keyfile, desktopFile, flags, &error)) {
		printf("g_key_file_load_from_file error!\n");
		return -1;
	}

	sprintf (home_applet, "%s/.applet", home);
	Name = g_key_file_get_locale_string (keyfile, "Desktop Entry","Name", NULL, NULL);

	if ((fp = fopen(home_applet,"r")) != NULL) {
		fseek (fp, 0L, SEEK_END);
		flen = ftell (fp);
		file_content = (char *) malloc (flen+1);
		if (file_content == NULL) {
			fclose (fp);  
			return -1;
		}  
		fseek (fp, 0L, SEEK_SET);
		fread (file_content, flen, 1, fp);
		file_content[flen] = 0; 
		fclose (fp);
	}

	if (!strcmp (state,"add")) {
		if (strstr (file_content, Name) == NULL ){
			printf("return 0\n");
			if ((fp = fopen(home_applet,"a+")) != NULL) {
				fprintf (fp, "%s", Name);
				fclose (fp);
			}
		}
		else {
			printf("return 1\n");
			return 1;
		}
	}

	if (!strcmp (state, "delete")) {
		if (strstr (file_content, Name) != NULL ){
			if ((fp = fopen(home_applet, "w")) != NULL) {
				fprintf (fp, "%s", StrReplace (file_content, Name, ""));
				fclose (fp);
			}
		}
	}
	return 0;
}
