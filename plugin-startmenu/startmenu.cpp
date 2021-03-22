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
#include <QFile>
#include "QDebug"
#include "QByteArray"
#include "QFileInfo"

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
    setStyleSheet("QToolButton { margin-left: 4px; } ");
    QTimer::singleShot(5000,[this] {this->setToolTip(tr("UKui Menu")); });
    //    this->setWindowFlags(Qt::NoFocus);
    //setAttribute(Qt::WA_X11DoNotAcceptFocus, true);
    //setAttribute(Qt::WA_ShowWithoutActivating,true);
    //setFocusPolicy(Qt::NoFocus);
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

    QMenu *pUserAction=new QMenu(tr("User Action"));              //用户操作
    QMenu *pSleepHibernate=new QMenu(tr("Sleep or Hibernate"));   //重启或休眠
    QMenu *pPowerSupply=new QMenu(tr("Power Supply"));            //电源
    rightPressMenu->addMenu(pUserAction);
    rightPressMenu->addMenu(pSleepHibernate);
    rightPressMenu->addMenu(pPowerSupply);

    pUserAction->addAction(QIcon::fromTheme("system-lock-screen-symbolic"),
                           tr("Lock Screen"),
                           this, SLOT(ScreenServer())
                           );                                     //锁屏
    pUserAction->addAction(QIcon::fromTheme("stock-people-symbolic"),
                           tr("Switch User"),
                           this, SLOT(SessionSwitch())
                           );                                     //切换用户
    pUserAction->addAction(QIcon::fromTheme("system-logout-symbolic"),
                           tr("Logout"),
                           this, SLOT(SessionLogout())
                           );                                     //注销
    /*
    //社区版本 安装时未强求建立 swap分区，若未建swap分区,会导致休眠(hibernate)失败，所以在20.04上屏蔽该功能
    getOsRelease();
    if(QString::compare(version,"Ubuntu"))
    或使用!QString::compare(getCanHibernateResult(),"yes") 【目前该接口有bug】
    */

    //检测CanHibernate接口的返回值，判断是否可以执行挂起操作


    QString filename = QDir::homePath() + "/.config/ukui/panel-commission.ini";
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");

    m_settings.beginGroup("Hibernate");
    QString hibernate_action = m_settings.value("hibernate", "").toString();
    if (hibernate_action.isEmpty()) {
        hibernate_action = "show";
    }
    m_settings.endGroup();

    if(QString::compare(version,"Ubuntu") && hibernate_action != "hide"){
        pSleepHibernate->addAction(QIcon::fromTheme("kylin-sleep-symbolic"),
                                   tr("Hibernate Mode"),
                                   this, SLOT(SessionHibernate())
                                   );                              //休眠
    }
    pSleepHibernate->addAction(QIcon::fromTheme("system-sleep"),
                               tr("Sleep Mode"),
                               this, SLOT(SessionSuspend())
                               );                                   //睡眠
    pPowerSupply->addAction(QIcon::fromTheme("system-restart-symbolic"),
                            tr("Restart"),
                            this, SLOT(SessionReboot())
                            );                                      //重启
    QFileInfo file("/usr/bin/time-shutdown");
    if(file.exists())
        pPowerSupply->addAction(QIcon::fromTheme("system-restart-symbolic"),
                                tr("TimeShutdown"),
                                this, SLOT(TimeShutdown())
                                );                                  //定时开关机
    pPowerSupply->addAction(QIcon::fromTheme("system-shutdown-symbolic"),
                            tr("Power Off"),
                            this, SLOT(SessionShutdown())
                            );                                      //关机

    rightPressMenu->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), rightPressMenu->sizeHint()));
    rightPressMenu->show();
}

/*开始菜单按钮右键菜单选项，与开始菜单中电源按钮的右键功能是相同的*/
//锁屏
void UKUIStartMenuButton::ScreenServer()
{
    system("ukui-screensaver-command -l");
}

//切换用户
void UKUIStartMenuButton::SessionSwitch()
{
    QProcess::startDetached(QString("ukui-session-tools --switchuser"));
}

//注销
void UKUIStartMenuButton::SessionLogout()
{
    system("ukui-session-tools --logout");
}

//休眠 睡眠
void UKUIStartMenuButton::SessionHibernate()
{
    system("ukui-session-tools --hibernate");
}

//睡眠
void UKUIStartMenuButton::SessionSuspend()
{
    system("ukui-session-tools --suspend");
}

//重启
void UKUIStartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

//定时关机
void UKUIStartMenuButton::TimeShutdown()
{
    QProcess *process_timeshutdowm =new QProcess(this);
    process_timeshutdowm->startDetached("/usr/bin/time-shutdown");
    process_timeshutdowm->deleteLater();
}

//关机
void UKUIStartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

//获取系统版本,若为ubuntu则取消休眠功能
void UKUIStartMenuButton::getOsRelease()
{
    QFile file("/etc/lsb-release");
    if (!file.open(QIODevice::ReadOnly)) qDebug() << "Read file Failed.";
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);
        if (str.contains("DISTRIB_ID")){
            version=str.remove("DISTRIB_ID=");
            version=str.remove("\n");
        }
    }
}

//检测当前系统能否执行休眠操作
QString UKUIStartMenuButton::getCanHibernateResult()
{
    QDBusInterface interface("org.freedesktop.login1", "/org/freedesktop/login1",
                             "org.freedesktop.login1.Manager",
                             QDBusConnection::systemBus());
    if (!interface.isValid()) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
    }
    /*调用远程的 CanHibernate 方法，判断是否可以执行休眠的操作,返回值为yes为允许执行休眠，no为无法执行休眠 na为交换分区不足*/
    QDBusReply<QString> reply = interface.call("CanHibernate");
    if (reply.isValid()) {
        return reply;
    } else {
        qCritical() << "Call Dbus method failed";
    }
}

void UKUIStartMenuButton::enterEvent(QEvent *) {
    repaint();
    return;
}

void UKUIStartMenuButton::leaveEvent(QEvent *) {
    repaint();
    return;
}
