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

UKUIStartbarPlugin::UKUIStartbarPlugin(const IUKUIPanelPluginStartupInfo &startupInfo):
    QObject(),
    IUKUIPanelPlugin(startupInfo),
    mWidget(new UKUIStartBarWidget(this))
{


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
    mStartMenuButton=new StartMenuButton(plugin,this);
    mTaskViewButton=new TaskViewButton(plugin,this);
    mLayout=new UKUi::GridLayout(this);
    mLayout->addWidget(mStartMenuButton);
    mLayout->addWidget(mTaskViewButton);

}

UKUIStartBarWidget::~UKUIStartBarWidget()
{
}

/*plugin-startmenu refresh function*/
void UKUIStartBarWidget::realign()
{
    if (mPlugin->panel()->isHorizontal()){
        mLayout->setColumnCount(mLayout->count());
        mLayout->setRowCount(0);
        this->setFixedSize(mPlugin->panel()->panelSize()*2,mPlugin->panel()->panelSize());
    }else{
        mLayout->setRowCount(mLayout->count());
        mLayout->setColumnCount(0);
        this->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize()*2);
    }
    mStartMenuButton->realign();
    mTaskViewButton->realign();
}
