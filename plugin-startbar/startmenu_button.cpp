#include "startmenu_button.h"

StartMenuButton::StartMenuButton(IUKUIPanelPlugin *plugin, QWidget* parent ):
    QToolButton(parent),
    m_parent(parent),
    m_plugin(plugin)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setIcon(QIcon::fromTheme("kylin-startmenu",QIcon("/usr/share/ukui-panel/panel/img/startmenu.svg")));
    this->setStyle(new CustomStyle());
    QTimer::singleShot(5000,[this] {this->setToolTip(tr("UKUI Menu")); });
    this->setIconSize(QSize(m_plugin->panel()->iconSize(),m_plugin->panel()->iconSize()));
}

StartMenuButton::~StartMenuButton()
{
}

/*plugin-startmenu refresh function*/
void StartMenuButton::realign()
{
    if (m_plugin->panel()->isHorizontal())
        this->setFixedSize(m_plugin->panel()->panelSize()*1.3,m_plugin->panel()->panelSize());
    else
        this->setFixedSize(m_plugin->panel()->panelSize(),m_plugin->panel()->panelSize()*1.3);
    this->setIconSize(QSize(m_plugin->panel()->iconSize(),m_plugin->panel()->iconSize()));
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
            process->deleteLater();
        }
        else{qDebug()<<"not find /usr/bin/ukui-start-menu"<<endl;}
    }
    QWidget::mousePressEvent(event);
}

void StartMenuButton::contextMenuEvent(QContextMenuEvent *event)
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
    if (hasMultipleUsers()) {
        pUserAction->addAction(QIcon::fromTheme("stock-people-symbolic"),
                               tr("Switch User"),
                               this, SLOT(SessionSwitch())
                               );                                     //切换用户
    }
    pUserAction->addAction(QIcon::fromTheme("ukui-system-logout-symbolic"),
                           tr("Logout"),
                           this, SLOT(SessionLogout())
                           );                                     //注销

    if(QString::compare(getCanHibernateResult(),"yes") == 0){
        pSleepHibernate->addAction(QIcon::fromTheme("kylin-sleep-symbolic"),
                                   tr("Hibernate Mode"),
                                   this, SLOT(SessionHibernate())
                                   );                              //休眠
    }
    pSleepHibernate->addAction(QIcon::fromTheme("ukui-hebernate-symbolic"),
                               tr("Sleep Mode"),
                               this, SLOT(SessionSuspend())
                               );                                   //睡眠
    pPowerSupply->addAction(QIcon::fromTheme("ukui-system-restart-symbolic"),
                            tr("Restart"),
                            this, SLOT(SessionReboot())
                            );                                      //重启
    QFileInfo file("/usr/bin/time-shutdown");
    if(file.exists())
        pPowerSupply->addAction(QIcon::fromTheme("ukui-shutdown-timer-symbolic"),
                                tr("TimeShutdown"),
                                this, SLOT(TimeShutdown())
                                );                                  //定时开关机
    pPowerSupply->addAction(QIcon::fromTheme("system-shutdown-symbolic"),
                            tr("Power Off"),
                            this, SLOT(SessionShutdown())
                            );                                      //关机

    rightPressMenu->setGeometry(m_plugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), rightPressMenu->sizeHint()));
    rightPressMenu->show();
}

/*开始菜单按钮右键菜单选项，与开始菜单中电源按钮的右键功能是相同的*/
//锁屏
void StartMenuButton::ScreenServer()
{
    system("ukui-screensaver-command -l");
}

//切换用户
void StartMenuButton::SessionSwitch()
{
    QProcess::startDetached(QString("ukui-session-tools --switchuser"));
}

//注销
void StartMenuButton::SessionLogout()
{
    system("ukui-session-tools --logout");
}

//休眠 睡眠
void StartMenuButton::SessionHibernate()
{
    system("ukui-session-tools --hibernate");
}

//睡眠
void StartMenuButton::SessionSuspend()
{
    system("ukui-session-tools --suspend");
}

//重启
void StartMenuButton::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

//定时关机
void StartMenuButton::TimeShutdown()
{
    QProcess *process_timeshutdowm =new QProcess(this);
    process_timeshutdowm->startDetached("/usr/bin/time-shutdown");
    process_timeshutdowm->deleteLater();
}

//关机
void StartMenuButton::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

//获取系统版本,若为ubuntu则取消休眠功能
void StartMenuButton::getOsRelease()
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
QString StartMenuButton::getCanHibernateResult()
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

bool StartMenuButton::hasMultipleUsers()
{
    QDBusInterface interface("org.freedesktop.Accounts",
                             "/org/freedesktop/Accounts",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    if (!interface.isValid()) {
        qCritical() << QDBusConnection::systemBus().lastError().message();
        return false;
    }

    QDBusReply<QVariant> reply = interface.call("Get","org.freedesktop.Accounts","HasMultipleUsers");
    return reply.value().toBool();
}

void StartMenuButton::enterEvent(QEvent *) {
    repaint();
    return;
}

void StartMenuButton::leaveEvent(QEvent *) {
    repaint();
    return;
}
