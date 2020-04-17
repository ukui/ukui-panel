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
    mButton=new NightModeButton(this);
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

NightModeButton::NightModeButton( IUKUIPanelPlugin *plugin, QWidget* parent):
    QToolButton(parent),
    mPlugin(plugin)
{  
    /*redshift的配置文件*/
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
        if(gsettings->keys().contains(NIGHT_MODE_KEY))
        {
            if(gsettings->get(NIGHT_MODE_KEY).toBool())
            {
                setNightMode(true);
            }
            else
            {
                setNightMode(false);
            }
        }
        else
        {
            QMessageBox::information(this,"Error",tr("please install Newest ukui-control-center first"));
        }

        connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key==NIGHT_MODE_KEY)
            {
                if(gsettings->get(NIGHT_MODE_KEY).toBool())
                {
                    setNightMode(true);
                }
                else
                {
                    setNightMode(false);
                }
            }
        });

        /*系统主题gsettings*/
        const QByteArray styleid(UKUI_STYLE);
        if(QGSettings::isSchemaInstalled(styleid)) {
            mstyleGsettings = new QGSettings(styleid);
        }
    }
}
NightModeButton::~NightModeButton(){
    delete gsettings;
    delete mstyleGsettings;
}

/*NOTE:目前夜间模式的点击按钮实现的是　设置夜间模式＋切换主题*/
void NightModeButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(mode)
        {
            if(gsettings->keys().contains(NIGHT_MODE_KEY))
            {
                gsettings->set("nightmode", true);
                setNightMode(true);
                setUkuiStyle("ukui-black");
                mode=false;
            }

        }
        else
        {
            if(gsettings->keys().contains(NIGHT_MODE_KEY))
            {
                gsettings->set("nightmode", false);
                setNightMode(false);
                setUkuiStyle("ukui-white");
                mode=true;
            }

        }
    }
}
void NightModeButton::contextMenuEvent(QContextMenuEvent *event)
{
    nightModeMenu=new QMenu();
    nightModeMenu->setAttribute(Qt::WA_DeleteOnClose);

    nightModeMenu->addAction(QIcon::fromTheme("document-page-setup"),
                             tr("Turn On NightMode"),
                             this, SLOT(turnNightMode())
                             );
    nightModeMenu->addAction(QIcon::fromTheme("document-page-setup"),
                             tr("Set Up NightMode"),
                             this, SLOT(setUpNightMode())
                             );
    nightModeMenu->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), nightModeMenu->sizeHint()));
    nightModeMenu->show();
}

void NightModeButton::turnNightMode()
{
    setNightMode(true);
    setUkuiStyle("ukui-black");
    mode=true;
}

void NightModeButton::setUpNightMode()
{
    QProcess *process =new QProcess(this);
    process->startDetached("ukui-control-center -m");
    process->deleteLater();
}

/*
 * 设置夜间模式
 * tr: set NightMode
*/
void NightModeButton::setNightMode(const bool nightMode){
    QProcess process;
    QString cmd;
    QString serverCmd;

    if(nightMode)
    {
        cmd = "restart";
        serverCmd = "enable";
        QIcon icon=QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg");
        this->setIcon(icon);
        this->setToolTip(tr("nightmode open"));
    }
    else
    {
        cmd = "stop";
        serverCmd = "disable";
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        this->setToolTip(tr("nightmode close"));
    }

    process.startDetached("systemctl", QStringList() << "--user" << serverCmd << "redshift.service");
    process.startDetached("systemctl", QStringList() << "--user" << cmd << "redshift.service");

}

/*设置主题*/
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
