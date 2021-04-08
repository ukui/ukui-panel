#!/bin/bash

## 主要用于ukui3.0->ukui3.1 过渡阶段任务栏的插件配置
echo $1
echo $2

plugin_name=$1
plugin_config=$2

if [ "$plugin_name" = "calendar" ]; then
	if [ "$plugin_config" = "new" ]; then
		sed -i 's/CalendarVersion=old/CalendarVersion=new/' ${HOME}/.config/ukui/panel-commission.ini
	else
		sed -i 's/CalendarVersion=new/CalendarVersion=old/' ${HOME}/.config/ukui/panel-commission.ini
	fi
elif [ "$plugin_name" = "tray" ]; then
	echo "tray"
fi

