lib_LTLIBRARIES = libukui-panel-applet-4.la
noinst_PROGRAMS = test-dbus-applet

AM_CPPFLAGS =							\
	$(LIBUKUI_PANEL_APPLET_CFLAGS)				\
	-I$(top_builddir)/libukui-panel-applet			\
	-DUKUILOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"	\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

libukui_panel_appletincludedir = $(includedir)/ukui-panel-4.0/libukui-panel-applet
libukui_panel_appletinclude_HEADERS =	\
	ukui-panel-applet.h			\
	ukui-panel-applet-gsettings.h		\
	ukui-panel-applet-enums.h

libukui_panel_applet_4_la_SOURCES =		\
	$(BUILT_SOURCES)		\
	ukui-panel-applet.h			\
	ukui-panel-applet.c			\
	ukui-panel-applet-factory.h		\
	ukui-panel-applet-factory.c		\
	ukui-panel-applet-gsettings.c		\
	ukui-panel-applet-gsettings.h       \
	panel-plug.c			\
	panel-plug-private.h    \
	panel-applet-private.h

libukui_panel_applet_4_la_LIBADD  = \
	$(LIBUKUI_PANEL_APPLET_LIBS) \
	$(X_LIBS)

libukui_panel_applet_4_la_LDFLAGS = \
	-version-info $(LIB_UKUI_PANEL_APPLET_LT_VERSION)	\
	-no-undefined

test_dbus_applet_LDADD =	\
	$(LIBUKUI_PANEL_APPLET_LIBS)	\
	libukui-panel-applet-4.la

$(libukui_panel_applet_4_la_OBJECTS) $(test_dbus_applet_OBJECTS): $(BUILT_SOURCES)

ukui-panel-applet-marshal.h: ukui-panel-applet-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --header --prefix=ukui_panel_applet_marshal > $@

ukui-panel-applet-marshal.c: ukui-panel-applet-marshal.list $(GLIB_GENMARSHAL)
	$(AM_V_GEN)echo "#include \"ukui-panel-applet-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=ukui_panel_applet_marshal >> $@

ukui_panel_applet_enum_headers = $(top_srcdir)/libukui-panel-applet/ukui-panel-applet.h

ukui-panel-applet-enums.c: @REBUILD@ $(ukui_panel_applet_enum_headers)
	$(AM_V_GEN)glib-mkenums \
			--fhead "#include <glib-object.h>\n" \
			--fhead "#include \"ukui-panel-applet-enums.h\"\n\n" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--fprod "\n#include \"@filename@\"\n" \
			--vhead "static const G@Type@Value _@enum_name@_values[] = {" \
			--vprod "  { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "  { 0, NULL, NULL }\n};\n\n" \
			--vtail "GType\n@enum_name@_get_type (void)\n{\n" \
			--vtail "  static GType type = 0;\n\n" \
			--vtail "  if (!type)\n" \
			--vtail "    type = g_@type@_register_static (\"@EnumName@\", _@enum_name@_values);\n\n" \
			--vtail "  return type;\n}\n\n" \
		$(ukui_panel_applet_enum_headers) > $@

ukui-panel-applet-enums.h: @REBUILD@ $(ukui_panel_applet_enum_headers)
	$(AM_V_GEN)glib-mkenums \
			--fhead "#ifndef __UKUI_PANEL_APPLET_ENUMS_H__\n" \
			--fhead "#define __UKUI_PANEL_APPLET_ENUMS_H__\n\n" \
			--fhead "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n" \
			--ftail "#ifdef __cplusplus\n}\n#endif\n\n" \
			--ftail "#endif /* __UKUI_PANEL_APPLET_ENUMS_H__ */\n" \
			--fprod "\n/* --- @filename@ --- */" \
			--eprod "#define PANEL_TYPE_@ENUMNAME@ @enum_name@_get_type()\n" \
			--eprod "GType @enum_name@_get_type (void);\n" \
		$(ukui_panel_applet_enum_headers) > $@

BUILT_SOURCES = \
	ukui-panel-applet-enums.c			\
	ukui-panel-applet-enums.h			\
	ukui-panel-applet-marshal.c			\
	ukui-panel-applet-marshal.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libukuipanelapplet-4.0.pc

appletdir       = $(datadir)/ukui-panel/applets
applet_in_files = org.ukui.panel.TestApplet.ukui-panel-applet.in
noinst_DATA     = $(applet_in_files:.ukui-panel-applet.in=.ukui-panel-applet)

@PANEL_INTLTOOL_UKUI_PANEL_APPLET_RULE@

EXTRA_DIST =						\
	org.ukui.panel.TestApplet.ukui-panel-applet.in	\
	ukui-panel-applet-marshal.list			\
	libukuipanelapplet-4.0.pc.in			\
	libukuipanelapplet-4.0-uninstalled.pc.in

CLEANFILES = $(BUILT_SOURCES) $(noinst_DATA)

###############################
# Introspection generation

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --add-include-path=$(srcdir) --add-include-path=${prefix}/share/gir-1.0
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = 	\
	ukui-panel-applet.c		\
	ukui-panel-applet.h		\
	ukui-panel-applet-gsettings.c	\
	ukui-panel-applet-gsettings.h
built_introspection_sources =	\
	ukui-panel-applet-enums.c	\
	ukui-panel-applet-enums.h

UkuiPanelApplet-4.0.gir: libukui-panel-applet-4.la
UkuiPanelApplet_4_0_gir_INCLUDES = GObject-2.0 Gtk-3.0 Gio-2.0
UkuiPanelApplet_4_0_gir_CFLAGS = $(AM_CPPFLAGS) -I$(srcdir)
UkuiPanelApplet_4_0_gir_LIBS = libukui-panel-applet-4.la
UkuiPanelApplet_4_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources)) $(addprefix $(builddir)/,$(built_introspection_sources))
# When requiring gobject-introspection 0.9.5, use this:
#UkuiPanelApplet_4_0_gir_SCANNERFLAGS = --identifier-prefix=Panel --symbol-prefix=panel --pkg-export=libukuipanelapplet-4.0
UkuiPanelApplet_4_0_gir_SCANNERFLAGS = --strip-prefix=UkuiPanel --pkg-export=libukuipanelapplet-4.0
INTROSPECTION_GIRS += UkuiPanelApplet-4.0.gir

girdir = $(INTROSPECTION_GIRDIR)
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(INTROSPECTION_TYPELIBDIR)
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

################################

dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)

-include $(top_srcdir)/git.mk
