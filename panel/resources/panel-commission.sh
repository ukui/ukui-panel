#!/bin/bash
## 定制版本的配置文件初始化处理

#判断文件是否存在
commissionFile="${HOME}/.config/ukui/panel-commission.ini"
if [[ ! -f "$commissionFile" ]]; then
  echo "file not exit"
  cp /usr/share/ukui/ukui-panel/panel-commission.ini ${HOME}/.config/ukui
else
  echo "file exit"
fi

## 龙芯机器屏蔽夜间模式
grep  -nr  "Loongson"     /proc/cpuinfo
if [ $? -ne 0 ]; then
  echo "not Loongarch"
else
  echo "龙芯机器 屏幕夜间模式"
  while read line
  do
    echo $line
    if [[ $line == *nightmode* ]];then
      sed -i 's/nightmode=show/nightmode=hide/' ${HOME}/.config/ukui/panel-commission.ini
    fi
  done < ${HOME}/.config/ukui/panel-commission.ini
fi

## 华为990 屏蔽休眠接口
env | grep "XDG_SESSION_TYPE=wayland"
if [ $? -ne 0 ]; then
  echo " "
else
  echo "华为990"
  #sed -i 's/hibernate=show/hibernate=hide/' ${HOME}/.config/ukui/panel-commission.ini
fi

## 华为990 屏蔽夜间模式
env | grep "XDG_SESSION_TYPE=wayland"
if [ $? -ne 0 ]; then
  echo " "
else
  echo "华为990"
  while read line1
  do
    if [[ $line1 == *nightmode* ]];then
      sed -i 's/nightmode=show/nightmode=hide/' ${HOME}/.config/ukui/panel-commission.ini
    fi
  done < ${HOME}/.config/ukui/panel-commission.ini
fi


