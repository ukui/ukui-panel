[ -f /home/kylin/work/panel/ukui-panel/plugin-quicklaunch/resources/quicklaunch.desktop.in ] || exit 0
echo '[ukui.quicklaunch_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-quicklaunch/resources/quicklaunch.desktop.in'
echo 'file_filter = plugin-quicklaunch/translations/quicklaunch_<lang>.desktop'
echo ''
