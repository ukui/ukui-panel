[ -f /home/kylin/work/panel/ukui-panel/plugin-showdesktop/resources/showdesktop.desktop.in ] || exit 0
echo '[ukui.showdesktop_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-showdesktop/resources/showdesktop.desktop.in'
echo 'file_filter = plugin-showdesktop/translations/showdesktop_<lang>.desktop'
echo ''
