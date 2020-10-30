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
//    this->setWindowFlags(Qt::NoFocus);
    setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
    setAttribute(Qt::WA_ShowWithoutActivating,true);
    setFocusPolicy(Qt::NoFocus);
}

UKUIStartMenuButton::~UKUIStartMenuButton()
{
}

/*plugin-startmenu refresh function*/
void UKUIStartMenuButton::realign()
{
    if (mPlugin->panel()->isHorizontal())
        this->setFixedSize(mPlugin->panel()->panelSize()*1.3,mPlugin->panel()->panelSize());
    else
       this->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize()*1.3);
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
            process->deleteLater();
        }
        else{qDebug()<<"not find /usr/bin/ukui-start-menu"<<endl;}
    }
    QWidget::mousePressEvent(event);
}

void UKUIStartMenuButton::contextMenuEvent(QContextMenuEvent *event)
{
    rightPressMenu=new QMenu();
    rightPressMenu->setAttribute(Qt::WA_DeleteOnClose);

    QMenu *pUserAction=new QMenu(tr("User Action"));         //用户操作
    QMenu *pSleepHibernate=new QMenu(tr("Sleep or Hibernate"));  //重启或休眠
    QMenu *pPowerSupply=new QMenu(tr("Power Supply"));        //电源
    rightPressMenu->addMenu(pUserAction);
    rightPressMenu->addMenu(pSleepHibernate);
    rightPressMenu->addMenu(pPowerSupply);

    pUserAction->addAction(QIcon::fromTheme("system-lock-screen-symbolic"),
                              tr("Lock Screen"),
                              this, SLOT(ScreenServer())
                              );                              //锁屏
    pUserAction->addAction(QIcon::fromTheme("stock-people-symbolic"),
                              tr("Switch User"),
                              this, SLOT(SessionSwitch())
                              );                              //切换用户
    pUserAction->addAction(QIcon::fromTheme("system-logout-symbolic"),
                              tr("Logout"),
                              this, SLOT(SessionLogout())
                              );                             //注销
    pSleepHibernate->addAction(QIcon::fromTheme("system-sleep"),
                              tr("Hibernate Mode"),
                              this, SLOT(SessionHibernate())
                              );                             //休眠
    pSleepHibernate->addAction(QIcon::fromTheme("kylin-sleep-symbolic"),
                              tr("Sleep Mode"),
                              this, SLOT(SessionSleep())
                              );                             //挂起
    pPowerSupply->addAction(QIcon::fromTheme("system-restart-symbolic"),
                              tr("Restart"),
                              this, SLOT(SessionReboot())
                              );                             //重启
    pPowerSupply->addAction(QIcon::fromTheme("system-restart-symbolic"),
                              tr("TimeShutdown"),
                              this, SLOT(SessionReboot())
                              );                             //重启
    pPowerSupply->addAction(QIcon::fromTheme("system-shutdown-symbolic"),
                              tr("Power Off"),
                              this, SLOT(SessionShutdown())
                              );                             //关机

    rightPressMenu->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), rightPressMenu->sizeHint()));
    rightPressMenu->show();
}

/*开始菜单按钮右键菜单选项，与开始菜单中电源按钮的右键功能是相同的*/
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

void UKUIStartMenuButton::SessionHibernate()
{
    system("ukui-session-tools --hibernate");
}

void UKUIStartMenuButton::SessionSleep()
{
    system("ukui-session-tools --suspend");
}

void UKUIStartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

void UKUIStartMenuButton::TimeShutdown()
{
    QProcess *process_timeshutdowm =new QProcess(this);
    process_timeshutdowm->startDetached("/usr/bin/ukui-menu");
    process_timeshutdowm->deleteLater();
}

void UKUIStartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

