/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
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

#include "startbar.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMenu>

#include <QStyle>
#include "../panel/customstyle.h"
#define THEME_QT_SCHEMA                     "org.ukui.style"
#define THEME_Style_Name                    "styleName"
#define UKUI_PANEL_SETTINGS                 "org.ukui.panel.settings"
#define SHOW_TASKVIEW                       "showtaskview"

UKUIStartbarPlugin::UKUIStartbarPlugin(const IUKUIPanelPluginStartupInfo &startupInfo):
    QObject(),
    IUKUIPanelPlugin(startupInfo),
    m_widget(new UKUIStartBarWidget(this))
{

    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

UKUIStartbarPlugin::~UKUIStartbarPlugin()
{
    delete m_widget;
}

QWidget *UKUIStartbarPlugin::widget()
{
    return m_widget;
}

void UKUIStartbarPlugin::realign()
{
    m_widget->realign();
}

UKUIStartBarWidget::UKUIStartBarWidget( IUKUIPanelPlugin *plugin, QWidget* parent ):
    m_plugin(plugin)
{
    translator();
    m_startMenuButton=new StartMenuButton(plugin,this);
    m_layout=new UKUi::GridLayout(this);
    m_layout->addWidget(m_startMenuButton);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id);
    }
    connect(m_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==SHOW_TASKVIEW)
            realign();
    });

    realign();
}

void UKUIStartBarWidget::translator(){
    m_translator = new QTranslator(this);
     QString locale = QLocale::system().name();
     if (locale == "zh_CN"){
         if (m_translator->load(QM_INSTALL))
             qApp->installTranslator(m_translator);
         else
             qDebug() <<PLUGINNAME<<"Load translations file" << locale << "failed!";
     }
}

UKUIStartBarWidget::~UKUIStartBarWidget()
{
    m_startMenuButton->deleteLater();
    m_taskViewButton->deleteLater();
}

/*plugin-startmenu refresh function*/
void UKUIStartBarWidget::realign()
{
    if(m_gsettings->get(SHOW_TASKVIEW).toBool()){
        if (!this->findChild<TaskViewButton *>("TaskViewButton")) {
            m_taskViewButton=new TaskViewButton(m_plugin,this);
            m_taskViewButton->setObjectName("TaskViewButton");
            m_layout->addWidget(m_taskViewButton);
        }
    } else {
        if (this->findChild<TaskViewButton *>("TaskViewButton")) {
            if (m_taskViewButton != nullptr) {
                m_layout->removeWidget(m_taskViewButton);
                m_taskViewButton->deleteLater();
            }
        } else {
            m_startMenuButton->realign();
            return;
        }
    }
    if (m_plugin->panel()->isHorizontal()){
        m_layout->setColumnCount(m_layout->count());
        m_layout->setRowCount(0);
//        this->setFixedSize(m_plugin->panel()->panelSize()*2.3,m_plugin->panel()->panelSize());
    }else{
        m_layout->setRowCount(m_layout->count());
        m_layout->setColumnCount(0);
//        this->setFixedSize(m_plugin->panel()->panelSize(),m_plugin->panel()->panelSize()*2.3);
    }
    m_startMenuButton->realign();
    m_taskViewButton->realign();
}
