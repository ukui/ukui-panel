#!/bin/bash
#for((i=0;i<20;i++))
for i in `seq 30`
do
	#echo $i
	s="org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/";
	#gsettings get  "$s" name
	gsettingGetAppName=$(gsettings get  "$s" name);
	trayAppName=\'explorer.exe\'
	#echo $gsettingGetAppName
	#echo $trayAppName
	if [ $gsettingGetAppName = $trayAppName ] 
	then
		gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ record tray
		gsettings set org.ukui.panel.tray:/org/ukui/tray/keybindings/custom${i}/ action tray
	fi
done

