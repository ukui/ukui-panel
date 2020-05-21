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

/*plugin-startmenu refresh function*/
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

    rightPressMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("system-lock-screen-symbolic").pixmap(24,24).toImage()))),
                              tr("Lock Screen"),
                              this, SLOT(ScreenServer())
                              );
    rightPressMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("stock-people-symbolic").pixmap(24,24).toImage()))),
                              tr("Switch User"),
                              this, SLOT(SessionSwitch())
                              );

    rightPressMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("system-logout-symbolic").pixmap(24,24).toImage()))),
                              tr("LogOut"),
                              this, SLOT(SessionLogout())
                              );

    rightPressMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("system-restart-symbolic").pixmap(24,24).toImage()))),
                              tr("Restart"),
                              this, SLOT(SessionReboot())
                              );

    rightPressMenu->addAction(QIcon(HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("exit_symbolic").pixmap(24,24).toImage()))),
                              tr("Power Off"),
                              this, SLOT(SessionShutdown())
                              );

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

void UKUIStartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

void UKUIStartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

