[ -f /home/kylin/work/panel/ukui-panel/plugin-segmentation/resources/segmentation.desktop.in ] || exit 0
echo '[ukui.segmentation_desktop]'
echo 'type = DESKTOP'
echo 'source_lang = en'
echo 'source_file = plugin-segmentation/resources/segmentation.desktop.in'
echo 'file_filter = plugin-segmentation/translations/segmentation_<lang>.desktop'
echo ''
