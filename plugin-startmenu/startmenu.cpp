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
StartMenu::StartMenu(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    qDebug()<<"StartMenu::StartMenu";
    mButton =new StartMenuButton();
    mButton->setStyle(new CustomStyle());
    mButton->setIcon(QIcon("/usr/share/ukui-panel/panel/img/startmenu.svg"));
    realign();

}

StartMenu::~StartMenu()
{
}


void StartMenu::realign()
{
    mButton->setFixedSize(panel()->panelSize(),panel()->panelSize());
    mButton->setIconSize(QSize(panel()->iconSize(),panel()->iconSize()));
}
StartMenuButton::StartMenuButton(){
}

StartMenuButton::~StartMenuButton()
{
}

void StartMenuButton::mousePressEvent(QMouseEvent* event)
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

void StartMenuButton::contextMenuEvent(QContextMenuEvent *)
{
    menuTaskview=new PopupMenu();
    menuTaskview->setAttribute(Qt::WA_DeleteOnClose);


    menuTaskview->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Lock The Screen"),
                   this, SLOT(ScreenServer())
                  );
    menuTaskview->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Switch The User"),
                   this, SLOT(SessionSwitch())
                  );

    menuTaskview->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Logout"),
                   this, SLOT(SessionLogout())
                  );

    menuTaskview->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Reboot"),
                   this, SLOT(SessionReboot())
                  );

    menuTaskview->addAction(XdgIcon::fromTheme(QLatin1String("configure")),
                   tr("Shutdown"),
                   this, SLOT(SessionShutdown())
                  );

//    int availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//    moveMenu();
    QCursor::pos();
    if(QCursor::pos().x()>500 && QCursor::pos().y()<100)
        menuTaskview->setGeometry(QCursor::pos().x()-160,QCursor::pos().y(),140,160);
    else if(QCursor::pos().x()<100 && QCursor::pos().y()>500)
        menuTaskview->setGeometry(QCursor::pos().x(),QCursor::pos().y()-190,140,160);
    else
        menuTaskview->setGeometry(QCursor::pos().x(),QCursor::pos().y(),140,160);
    menuTaskview->show();
}

void StartMenuButton::moveMenu(QPointF pt)
{
//    menuTaskview->setGeometry(pt.x(),pt.y(),140,140);

}
void StartMenuButton::ScreenServer()
{
    system("ukui-screensaver-command -l");
}
void StartMenuButton::SessionSwitch()
{
    QProcess::startDetached(QString("ukui-session-tools --switchuser"));
}

void StartMenuButton::SessionLogout()
{
    system("ukui-session-tools --logout");
}

void StartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

void StartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

