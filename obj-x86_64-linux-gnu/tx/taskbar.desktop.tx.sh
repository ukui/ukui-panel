[ -f /home/kylin/work/panel/ukui-panel/plugin-taskbar/resources/taskbar.desktop.in ] || exit 0
echo '[ukui.taskbar_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-taskbar/resources/taskbar.desktop.in'
echo 'file_filter = plugin-taskbar/translations/taskbar_<lang>.desktop'
echo ''
