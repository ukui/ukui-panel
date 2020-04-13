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
    mButton=new NightModeButton;
    mButton->setStyle(new CustomStyle());
    mButton->paintTooltipStyle();
    mButton->setToolTip(tr("nightmode"));
    realign();
}


NightMode::~NightMode(){
}

void NightMode::realign()
{
    mButton->setFixedSize(32,32);
    mButton->setIconSize(QSize(24,24));
}

NightModeButton::NightModeButton(){
//    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));

    const QByteArray id(NIGHT_MODE_CONTROL);
    if(QGSettings::isSchemaInstalled(id)) {
            gsettings = new QGSettings(id);
            mode=gsettings->get(NIGHT_MODE_KEY).toBool();
            nightModeChange(mode);
            connect(gsettings, &QGSettings::changed, this, [=] (const QString &key) {
                if (key == "nightmode") {
                    mode=gsettings->get(NIGHT_MODE_KEY).toBool();
                    nightModeChange(mode);
                }
            });
        }
}
NightModeButton::~NightModeButton(){
    delete gsettings;
}

void NightModeButton::mousePressEvent(QMouseEvent *)
{
    if(mode)
    {
        gsettings->set("nightmode", false);
    }
    else
    {
        gsettings->set("nightmode", true);
    }
}

void NightModeButton::nightModeChange(bool mode)
{
    if(mode)
    {
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        this->setToolTip(tr("nightmode open"));
        QProcess *nightprocess =new QProcess(this);
        nightprocess->startDetached("redshift -O 3600");
        nightprocess->deleteLater();
    }
    else
    {
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
        this->setToolTip(tr("nightmode close"));
        QProcess *lightprocess =new QProcess(this);
        lightprocess->startDetached("redshift -x");
        lightprocess->deleteLater();
    }
}
