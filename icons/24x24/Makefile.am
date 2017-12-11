size = 24x24

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
