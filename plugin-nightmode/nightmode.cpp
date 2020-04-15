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
#include <QDir>
#define NIGHT_MODE_LIGHT "light"
#define NIGHE_MODE_NIGHT "night"
#define NIGHT_MODE_KEY "nightmode"
#define NIGHT_MODE_CONTROL "org.ukui.control-center.panel.plugins"
#define UKUI_STYLE         "org.ukui.style"
#define UKUI_STYLE_NAME    "style-name"
#define DEFAULT_STYLE_NAME "styleName"


NightMode::NightMode(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    mButton=new NightModeButton;
    mButton->setStyle(new CustomStyle());
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
    QString filename = QDir::homePath() +"/.config/redshift.conf";
    mqsettings = new QSettings(filename, QSettings::IniFormat);

    mqsettings->beginGroup("redshift");

    mqsettings->setValue("dawn-time", "17:54");
    mqsettings->setValue("dusk-time", "17:55");
    mqsettings->setValue("temp-day", "3500");
    mqsettings->setValue("temp-night", "3500");

    mqsettings->endGroup();
    mqsettings->sync();

    /*
     * 夜间模式的初始化
     */
    const QByteArray id(NIGHT_MODE_CONTROL);
    if(QGSettings::isSchemaInstalled(id)) {
        gsettings = new QGSettings(id);
        if(gsettings->get(NIGHT_MODE_KEY).toBool())
        {
            this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
            this->setToolTip(tr("nightmode open"));
            mode=true;
        }
        else
        {
            this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
            this->setToolTip(tr("nightmode close"));
            mode=false;
        }
    }

    /*系统主题gsettings*/
    const QByteArray styleid(UKUI_STYLE);
    if(QGSettings::isSchemaInstalled(styleid)) {
        mstyleGsettings = new QGSettings(styleid);
    }
}
NightModeButton::~NightModeButton(){
    delete gsettings;
    delete mstyleGsettings;
}

void NightModeButton::mousePressEvent(QMouseEvent *)
{
    if(mode)
    {
        setNightMode(false);
        setUkuiStyle("ukui-white");
        mode=false;
    }
    else
    {
        setNightMode(true);
        setUkuiStyle("ukui-black");
        mode=true;
    }
}

/*
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
*/

/*
 * 设置夜间模式
 * tr: set NightMode
*/
void NightModeButton::setNightMode(const bool nightMode){
    QProcess process;
    QString cmd;
    QString serverCmd;

    if(nightMode) {
        cmd = "restart";
        serverCmd = "enable";
        gsettings->set("nightmode", true);
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg"));
        this->setToolTip(tr("nightmode open"));

    } else {
        cmd = "stop";
        serverCmd = "disable";
        gsettings->set("nightmode", false);
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        this->setToolTip(tr("nightmode close"));
    }

    process.startDetached("systemctl", QStringList() << "--user" << serverCmd << "redshift.service");

    process.startDetached("systemctl", QStringList() << "--user" << cmd << "redshift.service");

}

void NightModeButton::setUkuiStyle(QString style)
{
    if(QString::compare(style,"ukui-white")==0)
    {
        if(mstyleGsettings->keys().contains(DEFAULT_STYLE_NAME) || mstyleGsettings->keys().contains(UKUI_STYLE_NAME))
        {
            mstyleGsettings->set(UKUI_STYLE_NAME,"ukui-white");
        }
        else
        {
            qWarning()<<tr("don't contains the keys style-name");
            QMessageBox::information(this,"Error",tr("please install ukui-theme first"));
        }
    }
    else
    {
        if(mstyleGsettings->keys().contains(DEFAULT_STYLE_NAME) || mstyleGsettings->keys().contains(UKUI_STYLE_NAME))
        {
            mstyleGsettings->set(UKUI_STYLE_NAME,"ukui-black");
        }
        else
        {
            qWarning()<<tr("don't contains the keys style-name");
            QMessageBox::information(this,"Error",tr("please install ukui-theme first"));
        }
    }
}
