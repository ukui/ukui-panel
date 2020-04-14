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

#include "startmenu.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMenu>
#include <QStyle>
#include "../panel/customstyle.h"

UKUIStartMenuPlugin::UKUIStartMenuPlugin(const IUKUIPanelPluginStartupInfo &startupInfo):
    QObject(),
    IUKUIPanelPlugin(startupInfo),
    mWidget(new UKUIStartMenuButton(this))
{
}

UKUIStartMenuPlugin::~UKUIStartMenuPlugin()
{
    delete mWidget;
}

QWidget *UKUIStartMenuPlugin::widget()
{
    return mWidget;
}

void UKUIStartMenuPlugin::realign()
{
    mWidget->realign();
}

UKUIStartMenuButton::UKUIStartMenuButton( IUKUIPanelPlugin *plugin, QWidget* parent ):
    QToolButton(parent),
    mPlugin(plugin)
{
    this->setIcon(QIcon("/usr/share/ukui-panel/panel/img/startmenu.svg"));
    this->setStyle(new CustomStyle());
}

UKUIStartMenuButton::~UKUIStartMenuButton()
{
}

void UKUIStartMenuButton::realign()
{
    if (mPlugin->panel()->isHorizontal())
        this->setFixedSize(mPlugin->panel()->panelSize()+14,mPlugin->panel()->panelSize());
    else
       this->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize()+14);
    this->setIconSize(QSize(mPlugin->panel()->iconSize(),mPlugin->panel()->iconSize()));
}

void UKUIStartMenuButton::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();

    if (Qt::LeftButton == b)
    {
        if(QFileInfo::exists(QString("/usr/bin/ukui-menu")))
        {
            QProcess *process =new QProcess(this);
            process->startDetached("/usr/bin/ukui-menu");
        }
        else{qDebug()<<"not find /usr/bin/ukui-start-menu"<<endl;}
    }
    QWidget::mousePressEvent(event);
}

void UKUIStartMenuButton::contextMenuEvent(QContextMenuEvent *event)
{
    rightPressMenu=new QMenu();
    rightPressMenu->setAttribute(Qt::WA_DeleteOnClose);

    rightPressMenu->addAction(XdgIcon::fromTheme(QLatin1String("system-lock-screen")),
                              tr("Lock Screen"),
                              this, SLOT(ScreenServer())
                              );
    rightPressMenu->addAction(XdgIcon::fromTheme(QLatin1String("stock-people")),
                              tr("Switch User"),
                              this, SLOT(SessionSwitch())
                              );

    rightPressMenu->addAction(XdgIcon::fromTheme(QLatin1String("system-logout")),
                              tr("LogOut"),
                              this, SLOT(SessionLogout())
                              );

    rightPressMenu->addAction(XdgIcon::fromTheme(QLatin1String("system-restart")),
                              tr("Restart"),
                              this, SLOT(SessionReboot())
                              );

    rightPressMenu->addAction(XdgIcon::fromTheme(QLatin1String("exit")),
                              tr("Power Off"),
                              this, SLOT(SessionShutdown())
                              );

    rightPressMenu->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), rightPressMenu->sizeHint()));
    rightPressMenu->show();
}

void UKUIStartMenuButton::ScreenServer()
{
    system("ukui-screensaver-command -l");
}
void UKUIStartMenuButton::SessionSwitch()
{
    QProcess::startDetached(QString("ukui-session-tools --switchuser"));
}

void UKUIStartMenuButton::SessionLogout()
{
    system("ukui-session-tools --logout");
}

void UKUIStartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

void UKUIStartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

