# The module defines the following variables
#
#  UKUI_SHARE_DIR     - This allows to install and read the configs from non-standard locations
#
#  UKUI_TRANSLATIONS_DIR - The default translations directory
#
#  UKUI_ETC_XDG_DIR   - XDG standards expects system-wide configuration files in the
#                        /etc/xdg/ukui location. Unfortunately QSettings we are using internally
#                        can be overriden in the Qt compilation time to use different path for
#                        system-wide configs. (for example configure ... -sysconfdir /etc/settings ...)
#                        This path can be found calling Qt's qmake:
#                             qmake -query QT_INSTALL_CONFIGURATION
#
#  UKUI_DATA_DIR      - UKUi base directory relative to which data files should
#                       be searched.Defaults to CMAKE_INSTALL_FULL_DATADIR. It's
#                       added to XDG_DATA_DIRS by the startukui script.


set(UKUI_LIBRARY_NAME "ukui")
set(UKUI_RELATIVE_SHARE_DIR "ukui")
set(UKUI_SHARE_DIR          "/usr/share/ukui")
set(UKUI_RELATIVE_TRANSLATIONS_DIR "ukui/translations")
set(UKUI_TRANSLATIONS_DIR   "/usr/share/ukui/translations")
set(UKUI_GRAPHICS_DIR       "/usr/share/ukui/graphics")
set(UKUI_ETC_XDG_DIR        "/etc/xdg")
set(UKUI_DATA_DIR           "/usr/share")

add_definitions("-DUKUI_RELATIVE_SHARE_DIR=\"${UKUI_RELATIVE_SHARE_DIR}\"")
add_definitions("-DUKUI_SHARE_DIR=\"${UKUI_SHARE_DIR}\"")
add_definitions("-DUKUI_RELATIVE_SHARE_TRANSLATIONS_DIR=\"${UKUI_RELATIVE_TRANSLATIONS_DIR}\"")
add_definitions("-DUKUI_SHARE_TRANSLATIONS_DIR=\"${UKUI_TRANSLATIONS_DIR}\"")
add_definitions("-DUKUI_GRAPHICS_DIR=\"${UKUI_GRAPHICS_DIR}\"")
add_definitions("-DUKUI_ETC_XDG_DIR=\"${UKUI_ETC_XDG_DIR}\"")
add_definitions("-DUKUI_DATA_DIR=\"${UKUI_DATA_DIR}\"")
