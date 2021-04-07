#!/bin/bash
## 保证任务栏稳定性
## 在配置文件异常的情况下 使用默认配置文件

grep  -nr  "plugins"     ${HOME}/.config/ukui/panel.conf
if [ $? -ne 0 ]; then
        echo "配置文件异常"
	rm ${HOME}/.config/ukui/panel.conf
	cp /usr/share/ukui/panel.conf ${HOME}/.config/ukui/
else
	echo "配置文件正常"
fi

plugin_conf=""
plugin_conf_backup=""
while read line
do
  if [[ $line == *plugin* ]];then
	  echo $line
	  plugin_conf=$line
  fi
done < ${HOME}/.config/ukui/panel.conf

while read line
do
  if [[ $line == *plugin* ]];then
          echo $line
          plugin_conf_backup=$line
  fi
done < /usr/share/ukui/panel.conf

echo $plugin_conf
echo $plugin_conf_backup
