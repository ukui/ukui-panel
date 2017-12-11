size = 16x16

iconsdir = $(datadir)/icons/hicolor/$(size)/apps

icons_DATA =					\
	ukui-panel-clock.png			\
	ukui-panel-drawer.png			\
	ukui-panel-fish.png			\
	ukui-panel-force-quit.png		\
	ukui-panel-launcher.png		\
	ukui-panel-notification-area.png	\
	ukui-panel-separator.png		\
	ukui-panel.png				\
	ukui-panel-window-list.png		\
	ukui-panel-window-menu.png		\
	ukui-panel-workspace-switcher.png

icons_SOURCE =					\
	ukui-panel-clock.xcf.bz2		\
	ukui-panel-drawer.svg			\
	ukui-panel-fish.xcf.bz2		\
	ukui-panel-force-quit.svg		\
	ukui-panel-launcher.svg		\
	ukui-panel-notification-area.svg	\
	ukui-panel-separator.svg		\
	ukui-panel.svg				\
	ukui-panel-window-list.xcf.bz2		\
	ukui-panel-window-menu.xcf.bz2		\
	ukui-panel-workspace-switcher.svg

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated. After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST =		\
	$(icons_DATA)	\
	$(icons_SOURCE)

-include $(top_srcdir)/git.mk
