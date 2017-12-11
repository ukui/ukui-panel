noinst_LTLIBRARIES =			\
	libukui-panel-applet-private.la	\
	libukui-panel-applet-private-mini.la

AM_CPPFLAGS =							\
	$(PANEL_CFLAGS)						\
	-I.							\
	-I$(srcdir)						\
	-I$(top_srcdir)/ukui-panel				\
	-I$(top_builddir)/ukui-panel				\
	-I$(top_builddir)/ukui-panel/libukui-panel-applets-private	\
	-I$(top_builddir)/ukui-panel/libpanel-util		\
	-DDATADIR=\""$(datadir)"\"				\
	-DUKUI_PANEL_APPLETS_DIR=\"$(appletsdir)\"			\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

libukui_panel_applet_private_la_SOURCES =	\
	panel-applets-manager-dbus.c	\
	panel-applets-manager-dbus.h	\
	panel-applet-container.c	\
	panel-applet-container.h	\
	panel-applet-frame-dbus.c	\
	panel-applet-frame-dbus.h

libukui_panel_applet_private_mini_la_SOURCES =\
	panel-applet-mini.c		\
	panel-applets-manager-dbus.c	\
	panel-applets-manager-dbus.h	\
	panel-applet-container.c	\
	panel-applet-container.h

-include $(top_srcdir)/git.mk
