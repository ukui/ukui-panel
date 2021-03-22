#!/bin/bash
## 定制版本的配置文件初始化处理

## 龙芯机器屏蔽夜间模式
grep  -nr  "Loongson"     /proc/cpuinfo
if [ $? -ne 0 ]; then
        echo "非龙芯机器"
else
	echo "龙芯机器 屏幕夜间模式"
        sed -i 's/nightmode=show/nightmode=hide/' ${HOME}/.config/ukui/panel-commission.ini
fi

## 华为990 屏蔽休眠接口
grep  -nr  "wayland"     | env
if [ $? -ne 0 ]; then
	echo "非华为990"
else
	echo "华为990"
        sed -i 's/hibernate=show/hibernate=hide/' ${HOME}/.config/ukui/panel-commission.ini
fi

## 华为990 屏蔽夜间模式
grep  -nr  "wayland"     | env
if [ $? -ne 0 ]; then
        echo "非华为990"
else
        echo "华为990"
        sed -i 's/nightmode=show/nightmode=hide/' ${HOME}/.config/ukui/panel-commission.ini
fi

