size = scalable

iconsdir = $(datadir)/icons/hicolor/$(size)/apps

icons_DATA =					\
	ukui-panel-clock.svg			\
	ukui-panel-drawer.svg			\
	ukui-panel-fish.svg			\
	ukui-panel-force-quit.svg		\
	ukui-panel-launcher.svg		\
	ukui-panel-notification-area.svg	\
	ukui-panel-separator.svg		\
	ukui-panel.svg				\
	ukui-panel-window-list.svg		\
	ukui-panel-window-menu.svg		\
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
	$(icons_DATA)

-include $(top_srcdir)/git.mk
