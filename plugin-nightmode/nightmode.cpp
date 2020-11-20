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

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include "nightmode.h"
#include "../panel/customstyle.h"

#define NIGHT_MODE_KEY        "nightmodestatus"
#define NIGHT_MODE_LIGHT      "light"
#define NIGHE_MODE_NIGHT      "night"
#define NIGHT_MODE_CONTROL    "org.ukui.control-center.panel.plugins"

#define UKUI_QT_STYLE          "org.ukui.style"
#define GTK_STYLE              "org.mate.interface"
#define UKUI_QT_STYLE_NAME     "style-name"
#define DEFAULT_QT_STYLE_NAME  "styleName"
#define GTK_STYLE_NAME         "gtk-theme"
#define DEFAULT_GTK_STYLE_NAME "gtkTheme"
#define DEFAULT_STYLE          "ukui-default"
#define BLACK_STYLE            "ukui-black"
#define GTK_WHITE_STYLE        "ukui-white"

#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define SHOW_NIGHTMODE       "shownightmode"


NightMode::NightMode(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    mButton=new NightModeButton(this);
    mButton->setStyle(new CustomStyle());

    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        gsettings = new QGSettings(id);
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==SHOW_NIGHTMODE)
            realign();
    });
    realign();
}


NightMode::~NightMode(){
}

void NightMode::realign()
{
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    mButton->setFixedSize(0,0);
    mButton->setIconSize(QSize(0,0));
#else
    if(gsettings->get(SHOW_NIGHTMODE).toBool()){
        mButton->setFixedSize(panel()->panelSize()*0.75,panel()->panelSize()*0.75);
        mButton->setIconSize(QSize(panel()->iconSize()*0.75,panel()->iconSize()*0.75));
    }
    else{
        mButton->setFixedSize(0,0);
        mButton->setIconSize(QSize(0,0));
    }
#endif
}

NightModeButton::NightModeButton( IUKUIPanelPlugin *plugin, QWidget* parent):
    QToolButton(parent),
    mPlugin(plugin)
{  
    /*redshift的配置文件*/
    QString filename = QDir::homePath() +"/.config/redshift.conf";
    mqsettings = new QSettings(filename, QSettings::IniFormat);

    /*
     * 夜间模式的初始化
     */
    const QByteArray id(NIGHT_MODE_CONTROL);
    if(QGSettings::isSchemaInstalled(id)) {
        gsettings = new QGSettings(id);
        if(gsettings->keys().contains(NIGHT_MODE_KEY)){
            setNightMode(gsettings->get(NIGHT_MODE_KEY).toBool());
        }
        else
            qDebug()<<"nightmodestatus is not exit";

        connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key==NIGHT_MODE_KEY){
                setNightMode(gsettings->get(NIGHT_MODE_KEY).toBool());
            }
        });

        /*系统主题gsettings  qt+gtk*/
        const QByteArray styleid(UKUI_QT_STYLE);
        if(QGSettings::isSchemaInstalled(styleid)) {
            mqtstyleGsettings = new QGSettings(styleid);
        }
        const QByteArray gtkstyleid(GTK_STYLE);
        if(QGSettings::isSchemaInstalled(gtkstyleid)) {
            mgtkstyleGsettings = new QGSettings(gtkstyleid);
        }
    }
    else
    {
        QIcon icon=QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg");
        this->setIcon(icon);
    }
}
NightModeButton::~NightModeButton(){
    delete gsettings;
    delete mqtstyleGsettings;
    delete mgtkstyleGsettings;
}

/*NOTE:目前夜间模式的点击按钮实现的是　设置夜间模式＋切换主题*/
void NightModeButton::mousePressEvent(QMouseEvent *event)
{
    if(!QGSettings::isSchemaInstalled(NIGHT_MODE_CONTROL) || !gsettings->keys().contains(NIGHT_MODE_KEY))
        return;
    if(event->button()==Qt::LeftButton){
        if(gsettings->get(NIGHT_MODE_KEY).toBool()){
            gsettings->set(NIGHT_MODE_KEY, false);
            setNightMode(false);
            setUkuiStyle(DEFAULT_STYLE);
            qDebug()<<"gsettings->get(NIGHT_MODE_KEY).toBool():"<<gsettings->get(NIGHT_MODE_KEY).toBool();
        }else{
            gsettings->set(NIGHT_MODE_KEY, true);
            setNightMode(true);
            setUkuiStyle(BLACK_STYLE);
            qDebug()<<"gsettings->get(NIGHT_MODE_KEY).toBool()2:"<<gsettings->get(NIGHT_MODE_KEY).toBool();
        }
    }
}

/*夜间模式右键菜单*/
void NightModeButton::contextMenuEvent(QContextMenuEvent *event)
{
    nightModeMenu=new QMenu();
    nightModeMenu->setAttribute(Qt::WA_DeleteOnClose);
    nightModeMenu->setWindowOpacity(0.7);

    QAction * opennightmode = nightModeMenu->addAction(tr("Turn On NightMode"));

    opennightmode->setCheckable(true);
    opennightmode->setChecked(gsettings->get(NIGHT_MODE_KEY).toBool());
    connect(opennightmode, &QAction::triggered, [this] { turnNightMode(); });

    nightModeMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("document-page-setup").pixmap(24,24).toImage()))),
                             tr("Set Up NightMode"),
                             this, SLOT(setUpNightMode())
                             );
    nightModeMenu->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), nightModeMenu->sizeHint()));
    nightModeMenu->show();
}

void NightModeButton::turnNightMode()
{
    if(QGSettings::isSchemaInstalled(NIGHT_MODE_CONTROL))
    {
        if(mode){
            if(gsettings->keys().contains(NIGHT_MODE_KEY)){
                gsettings->set(NIGHT_MODE_KEY, true);
                setNightMode(true);
                setUkuiStyle(BLACK_STYLE);
                mode=false;
            }
        }
        else{
            if(gsettings->keys().contains(NIGHT_MODE_KEY)){
                gsettings->set(NIGHT_MODE_KEY, false);
                setNightMode(false);
                setUkuiStyle(DEFAULT_STYLE);
                mode=true;
            }
        }
    }
    else
        QMessageBox::information(this,"Error",tr("please install new ukui-control-center first"));
}

/*右键菜单选项　设置任务栏*/
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
        /* 控制面板不是常驻应用，装机成功后未启动控制面板设置夜间模式的情况下
         * /.config/redshift.conf　未设置
         * 需要任务栏设置初始化夜间模式的时间及色温
         * 其他情况下任务栏应读取控制面板设置的参数
         */
        mqsettings->beginGroup("redshift");
        if(mqsettings->value("temp-day", "").toString().isEmpty()){
            mqsettings->setValue("dawn-time", "17:54");
            mqsettings->setValue("dusk-time", "17:55");
            mqsettings->setValue("temp-day", "3500");
            mqsettings->setValue("temp-night", "3500");
        }
        else{
            mqsettings->setValue("dawn-time" , mqsettings->value("dawn-time", "").toString());
            mqsettings->setValue("dusk-time" , mqsettings->value("dusk-time", "").toString());
            mqsettings->setValue("temp-day"  , mqsettings->value("temp-day", "").toString());
            mqsettings->setValue("temp-night", mqsettings->value("temp-night", "").toString());
        }
        mqsettings->endGroup();
        mqsettings->sync();

        cmd = "restart";
        serverCmd = "enable";
        QIcon icon=QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg");
        this->setIcon(icon);
        this->setToolTip(tr("nightmode open"));
    }
    else{
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
    if(QString::compare(style,DEFAULT_STYLE)==0){
        if(mqtstyleGsettings->keys().contains(DEFAULT_QT_STYLE_NAME) || mqtstyleGsettings->keys().contains(UKUI_QT_STYLE_NAME))
            mqtstyleGsettings->set(UKUI_QT_STYLE_NAME,DEFAULT_STYLE);
        else
            qWarning()<<tr("don't contains the keys style-name");

        if(mgtkstyleGsettings->keys().contains(DEFAULT_GTK_STYLE_NAME) || mgtkstyleGsettings->keys().contains(GTK_STYLE_NAME))
            mgtkstyleGsettings->set(GTK_STYLE_NAME,GTK_WHITE_STYLE);
        else
            qWarning()<<tr("don't contains the keys style-name");

    }
    else{
        if(mqtstyleGsettings->keys().contains(DEFAULT_QT_STYLE_NAME) || mqtstyleGsettings->keys().contains(UKUI_QT_STYLE_NAME))
            mqtstyleGsettings->set(UKUI_QT_STYLE_NAME,BLACK_STYLE);
        else
            qWarning()<<tr("don't contains the keys style-name");
        if(mgtkstyleGsettings->keys().contains(DEFAULT_GTK_STYLE_NAME) || mgtkstyleGsettings->keys().contains(GTK_STYLE_NAME))
            mgtkstyleGsettings->set(GTK_STYLE_NAME,BLACK_STYLE);
        else
            qWarning()<<tr("don't contains the keys style-name");

    }
}

void NightModeButton::enterEvent(QEvent *) {
    repaint();
    return;
}

void NightModeButton::leaveEvent(QEvent *) {
    repaint();
    return;
}
