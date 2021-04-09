#!/bin/bash
#for((i=0;i<20;i++))
for i in `seq 30`
do
	#echo $i
	s="org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/";
	#gsettings get  "$s" name
	gsettingGetAppName=$(gsettings get  "$s" name);
	trayAppName=\'$1\'
	#echo $gsettingGetAppName
	#echo $trayAppName
	if [ $gsettingGetAppName = $trayAppName ] 
	then
		gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ record $2
		gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ action $2
	fi
	#if [ $trayAppName = "all" ]
	#then
		echo $trayAppName
		gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ record $2
                gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ action $2

#	fi
done

