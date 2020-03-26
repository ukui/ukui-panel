/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */


#include "nightmode.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMessageBox>
#include "../panel/customstyle.h"

#define NIGHT_MODE_LIGHT "light"
#define NIGHE_MODE_NIGHT "night"
#define NIGHT_MODE_KEY "nightmode"
#define NIGHT_MODE_CONTROL "org.ukui.control-center.panel.plugins"

NightMode::NightMode(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    realign();
    mButton.setStyle(new CustomStyle());
    mButton.paintTooltipStyle();
    mButton.setToolTip("夜间模式");
}


NightMode::~NightMode(){
}

void NightMode::realign()
{
    mButton.setFixedSize(32,32);
    mButton.setIconSize(QSize(24,24));
}

NightModeButton::NightModeButton(){
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
    const QByteArray id(NIGHT_MODE_CONTROL);
    if(QGSettings::isSchemaInstalled(id)) {
            gsettings = new QGSettings(id);
            connect(gsettings, &QGSettings::changed, this, [=] (const QString &key) {
                if (key == "nightmode") {
                    bool mode=gsettings->get(NIGHT_MODE_KEY).toBool();
                    if(mode==true){
                    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
                        this->setToolTip("夜间模式开启");
                    }
                    else{
                    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
                        this->setToolTip("夜间模式关闭");
                }

                }
            });
        }
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
}
NightModeButton::~NightModeButton(){
    delete gsettings;
}

void NightModeButton::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();
    if (Qt::LeftButton == b){
        bool mode=gsettings->get("nightmode").toBool();
        if(mode==true){
        gsettings->set("nightmode", false);
        system("killall redshift");
        }
        else{
        gsettings->set("nightmode", true);
//        system("redshift -t 5700:3600 -g 0.8 -m randr -v");
        if(QFileInfo::exists(QString("/usr/bin/redshift")))
        {
        QProcess *process =new QProcess(this);
        process->startDetached("redshift -t 5700:3600 -g 0.8 -m randr -v");
        }
        else{
            QMessageBox::information(this,"Error","请先安装redshift");
        }
        }
    }
    QWidget::mousePressEvent(event);
}

