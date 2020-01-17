[ -f /home/kylin/work/panel/ukui-panel/plugin-startmenu/resources/startmenu.desktop.in ] || exit 0
echo '[ukui.startmenu_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-startmenu/resources/startmenu.desktop.in'
echo 'file_filter = plugin-startmenu/translations/startmenu_<lang>.desktop'
echo ''
