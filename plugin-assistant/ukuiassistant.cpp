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

#include "ukuiassistant.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMenu>

#include <QStyle>
#include "../panel/customstyle.h"
#include "searchwindow.h"
#include "ui_searchwindow.h"
UKUIAssistantPlugin::UKUIAssistantPlugin(const IUKUIPanelPluginStartupInfo &startupInfo):
    QObject(),
    IUKUIPanelPlugin(startupInfo),
    mWidget(new UKUIAssistantButton(this))
{


}

UKUIAssistantPlugin::~UKUIAssistantPlugin()
{
    delete mWidget;
}

QWidget *UKUIAssistantPlugin::widget()
{
    return mWidget;
}

void UKUIAssistantPlugin::realign()
{
    mWidget->realign();
}

UKUIAssistantButton::UKUIAssistantButton( IUKUIPanelPlugin *plugin, QWidget* parent ):
    QToolButton(parent),
    mPlugin(plugin)
{

 //   this->setIcon(QIcon::fromTheme("docviewer-app-symbolic"));
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/search.png"));
    this->setStyle(new CustomStyle());
//    this->setWindowFlags(Qt::NoFocus);
    setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
    setAttribute(Qt::WA_ShowWithoutActivating,true);
    setFocusPolicy(Qt::NoFocus);

    win=new SearchWindow();
    QPalette pal(win->palette());

    //设置背景黑色
    pal.setColor(QPalette::Background, Qt::white);
    win->setAutoFillBackground(true);
    win->setPalette(pal);


            QPalette pal1(win->button1->palette());
            pal1.setColor(QPalette::Button, Qt::gray);
            pal1.setColor(QPalette::ButtonText, Qt::black);
            win->button1->setPalette(pal1);

            QPalette pal2(win->button2->palette());
            pal2.setColor(QPalette::Button, Qt::gray);
            pal2.setColor(QPalette::ButtonText, Qt::black);
            win->button2->setPalette(pal2);

            QPalette pal3(win->button3->palette());
            pal3.setColor(QPalette::Button, Qt::gray);
            pal3.setColor(QPalette::ButtonText, Qt::black);
            win->button3->setPalette(pal3);

            QPalette pal4(win->lineedit->palette());
            pal4.setColor(QPalette::Base, Qt::white);
            pal4.setColor(QPalette::Text, Qt::black);
            win->lineedit->setPalette(pal4);

}

UKUIAssistantButton::~UKUIAssistantButton()
{
}

/*plugin-startmenu refresh function*/
void UKUIAssistantButton::realign()
{
    if (mPlugin->panel()->isHorizontal())
        this->setFixedSize(mPlugin->panel()->panelSize()+14,mPlugin->panel()->panelSize());
    else
       this->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize()+14);
    this->setIconSize(QSize(mPlugin->panel()->iconSize(),mPlugin->panel()->iconSize()));
}

void UKUIAssistantButton::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();



    if (Qt::LeftButton == b)
    {
        win->show();
    }
    QWidget::mousePressEvent(event);
}

void UKUIAssistantButton::contextMenuEvent(QContextMenuEvent *event)
{

}
