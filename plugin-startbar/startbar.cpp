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
    mWidget(new UKUIStartBarWidget(this))
{

    mWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

UKUIStartbarPlugin::~UKUIStartbarPlugin()
{
    delete mWidget;
}

QWidget *UKUIStartbarPlugin::widget()
{
    return mWidget;
}

void UKUIStartbarPlugin::realign()
{
    mWidget->realign();
}

UKUIStartBarWidget::UKUIStartBarWidget( IUKUIPanelPlugin *plugin, QWidget* parent ):
    mPlugin(plugin)
{
    translator();
    mStartMenuButton=new StartMenuButton(plugin,this);
    mTaskViewButton=new TaskViewButton(plugin,this);
    mLayout=new UKUi::GridLayout(this);
    mLayout->addWidget(mStartMenuButton);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        mGsettings = new QGSettings(id);
    connect(mGsettings, &QGSettings::changed, this, [=] (const QString &key){
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
}

/*plugin-startmenu refresh function*/
void UKUIStartBarWidget::realign()
{
    if(mGsettings->get(SHOW_TASKVIEW).toBool()){
        if (mLayout->count() == 1) {
            mLayout->addWidget(mTaskViewButton);
        }
    } else {
        mLayout->removeWidget(mTaskViewButton);
    }
    if (mPlugin->panel()->isHorizontal()){
        mLayout->setColumnCount(mLayout->count());
        mLayout->setRowCount(0);
//        this->setFixedSize(mPlugin->panel()->panelSize()*2.3,mPlugin->panel()->panelSize());
    }else{
        mLayout->setRowCount(mLayout->count());
        mLayout->setColumnCount(0);
//        this->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize()*2.3);
    }
    mStartMenuButton->realign();
    mTaskViewButton->realign();
}
