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
#include <QTimer>
#include "nightmode.h"
#include "../panel/customstyle.h"

#define UKUI_QT_STYLE          "org.ukui.style"
#define GTK_STYLE              "org.mate.interface"
#define UKUI_QT_STYLE_NAME     "style-name"
#define DEFAULT_QT_STYLE_NAME  "styleName"
#define GTK_STYLE_NAME         "gtk-theme"
#define DEFAULT_GTK_STYLE_NAME "gtkTheme"
#define DEFAULT_STYLE          "ukui-default"
#define BLACK_STYLE            "ukui-black"
#define DARK_STYLE             "ukui-dark"
#define GTK_WHITE_STYLE        "ukui-white"

#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define SHOW_NIGHTMODE       "shownightmode"


NightMode::NightMode(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    translator();
    qDebug()<<"Plugin-NightMode :: plugin-nightmode start";
    //读取配置文件中nightmode 的值
    QString filename = QDir::homePath() + "/.config/ukui/panel-commission.ini";
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");

    m_settings.beginGroup("NightMode");
    nightmode_action = m_settings.value("nightmode", "").toString();
    if (nightmode_action.isEmpty()) {
        nightmode_action = "show";
    }
    m_settings.endGroup();

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
    qDebug()<<"Plugin-NightMode :: plugin-nightmode end";
}


NightMode::~NightMode(){
    delete gsettings;
}

void NightMode::translator(){
    m_translator = new QTranslator(this);
     QString locale = QLocale::system().name();
     if (locale == "zh_CN"){
         if (m_translator->load(QM_INSTALL))
             qApp->installTranslator(m_translator);
         else
             qDebug() <<PLUGINNAME<<"Load translations file" << locale << "failed!";
     }
}

void NightMode::realign()
{
    if(gsettings->get(SHOW_NIGHTMODE).toBool() && nightmode_action == "show"){
        mButton->setFixedSize(panel()->panelSize()*0.75,panel()->panelSize());
        mButton->setIconSize(QSize(panel()->iconSize()*0.75,panel()->iconSize()*0.75));
    }
    else{
        mButton->setFixedSize(0,0);
        mButton->setIconSize(QSize(0,0));
    }
}

NightModeButton::NightModeButton( IUKUIPanelPlugin *plugin, QWidget* parent):
    QToolButton(parent),
    m_plugin(plugin)
{  
    /*系统主题gsettings  qt+gtk*/
    const QByteArray styleid(UKUI_QT_STYLE);
    if(QGSettings::isSchemaInstalled(styleid)) {
        m_qtstyleGsettings = new QGSettings(styleid);
    }
    const QByteArray gtkstyleid(GTK_STYLE);
    if(QGSettings::isSchemaInstalled(gtkstyleid)) {
        m_gtkstyleGsettings = new QGSettings(gtkstyleid);
    }
    
    QDBusInterface iproperty("org.ukui.KWin",
                             "/ColorCorrect",
                             "org.ukui.kwin.ColorCorrect",
                             QDBusConnection::sessionBus());

    if (!iproperty.isValid()) {
        this->setVisible(false);
    }
    QDBusConnection::sessionBus().connect(QString(),
                                          QString("/ColorCorrect"),
                                          "org.ukui.kwin.ColorCorrect",
                                          "nightColorConfigChanged",
                                          this,
                                          SLOT(nightChangedSlot(QHash<QString,QVariant>)));
//    getNightModeState();
    //    controlCenterSetNightMode(mode);
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
    this->setProperty("useIconHighlightEffect", 0x2);
    QTimer::singleShot(5000,[this] { this->setToolTip(tr("night mode close")); });


    this->setEnabled(false);
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,[this] {this->setEnabled(true);});
    timer->start(50);
    connect(this,&NightModeButton::clicked,this, [this] { onClick();});
}
NightModeButton::~NightModeButton(){
    delete m_qtstyleGsettings;
    delete m_gtkstyleGsettings;
}

void NightModeButton::onClick()
{
    getNightModeState();
    if(m_mode){
        setUkuiStyle(DEFAULT_STYLE);

    }else{
        setUkuiStyle(DARK_STYLE);
    }
    setNightMode(!m_mode);
    this->setEnabled(true);
}

void NightModeButton::contextMenuEvent(QContextMenuEvent *event)
{
}

/*
 * 设置夜间模式
 * tr: set NightMode
*/
void NightModeButton::setNightMode(const bool nightMode){
    QDBusInterface iproperty("org.ukui.KWin",
                             "/ColorCorrect",
                             "org.ukui.kwin.ColorCorrect",
                             QDBusConnection::sessionBus());

    if (!iproperty.isValid()) {
        this->setVisible(false);
        return;
    }
    QHash<QString, QVariant> data;

    if(nightMode){
        data.insert("Active", true);
        data.insert("Mode", 3);
        data.insert("NightTemperature", m_colorTemperature);

        iproperty.call("setNightColorConfig", data);
        QIcon icon=QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg");
        this->setIcon(icon);
        this->setToolTip(tr("nightmode opened"));
    }
    else{
        data.insert("Active", false);
        iproperty.call("setNightColorConfig", data);
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        this->setToolTip(tr("nightmode closed"));
    }
}

void NightModeButton::controlCenterSetNightMode(const bool nightMode){
    QDBusInterface iproperty("org.ukui.KWin",
                             "/ColorCorrect",
                             "org.ukui.kwin.ColorCorrect",
                             QDBusConnection::sessionBus());

    if (!iproperty.isValid()) {
        this->setVisible(false);
        return;
    }
    QHash<QString, QVariant> data;

    if(nightMode){
        data.insert("Active", true);
        data.insert("NightTemperature", m_colorTemperature);
        iproperty.call("setNightColorConfig", data);
        QIcon icon=QIcon("/usr/share/ukui-panel/panel/img/nightmode-night.svg");
        this->setIcon(icon);
        QTimer::singleShot(5000,[this] { this->setToolTip(tr("night mode open")); });
    }
    else{
        data.insert("Active", false);
        iproperty.call("setNightColorConfig", data);
        this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/nightmode-light.svg"));
        QTimer::singleShot(5000,[this] { this->setToolTip(tr("night mode close")); });
    }
}

void NightModeButton::getNightModeState()
{
    QDBusInterface ipropertyinfo("org.ukui.KWin",
                                 "/ColorCorrect",
                                 "org.ukui.kwin.ColorCorrect",
                                 QDBusConnection::sessionBus());
    QDBusMessage msg=  ipropertyinfo.call("nightColorInfo");

    const QDBusArgument &dbusArg = msg.arguments().at( 0 ).value<QDBusArgument>();
    QMap<QString,QVariant > map;
    dbusArg >> map;
    for( QString outer_key : map.keys() ){
        QVariant innerMap = map.value( outer_key );
        if(outer_key=="NightTemperature")
            m_colorTemperature=innerMap.toInt();
        if(outer_key=="Active")
            m_mode=innerMap.toBool();
        //        if(!outer_key.contains("EveningBeginFixed"))

    }
}
void NightModeButton::nightChangedSlot(QHash<QString, QVariant> nightArg)
{
    getNightModeState();
    controlCenterSetNightMode(m_mode);
}

/*设置主题*/
void NightModeButton::setUkuiStyle(QString style)
{
    if(QString::compare(style,DEFAULT_STYLE)==0){
        if(m_qtstyleGsettings->keys().contains(DEFAULT_QT_STYLE_NAME) || m_qtstyleGsettings->keys().contains(UKUI_QT_STYLE_NAME))
            m_qtstyleGsettings->set(UKUI_QT_STYLE_NAME,DEFAULT_STYLE);
        if(m_gtkstyleGsettings->keys().contains(DEFAULT_GTK_STYLE_NAME) || m_gtkstyleGsettings->keys().contains(GTK_STYLE_NAME))
            m_gtkstyleGsettings->set(GTK_STYLE_NAME,GTK_WHITE_STYLE);

    }
    else{
        if(m_qtstyleGsettings->keys().contains(DEFAULT_QT_STYLE_NAME) || m_qtstyleGsettings->keys().contains(UKUI_QT_STYLE_NAME))
            m_qtstyleGsettings->set(UKUI_QT_STYLE_NAME,DARK_STYLE);
        if(m_gtkstyleGsettings->keys().contains(DEFAULT_GTK_STYLE_NAME) || m_gtkstyleGsettings->keys().contains(GTK_STYLE_NAME))
            m_gtkstyleGsettings->set(GTK_STYLE_NAME,BLACK_STYLE);
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
