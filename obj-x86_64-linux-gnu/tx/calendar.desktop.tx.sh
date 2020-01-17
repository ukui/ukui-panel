[ -f /home/kylin/work/panel/ukui-panel/plugin-calendar/resources/calendar.desktop.in ] || exit 0
echo '[ukui.calendar_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-calendar/resources/calendar.desktop.in'
echo 'file_filter = plugin-calendar/translations/calendar_<lang>.desktop'
echo ''
