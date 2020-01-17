[ -f /home/kylin/work/panel/ukui-panel/plugin-taskview/resources/taskview.desktop.in ] || exit 0
echo '[ukui.taskview_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-taskview/resources/taskview.desktop.in'
echo 'file_filter = plugin-taskview/translations/taskview_<lang>.desktop'
echo ''
