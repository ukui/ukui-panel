#include "taskview_button.h"


TaskViewButton::TaskViewButton(IUKUIPanelPlugin *plugin,QWidget *parent):
    m_parent(parent),
    m_plugin(plugin)
{
    this->setParent(parent);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setToolTip(tr("Show Taskview"));
    this->setStyle(new CustomStyle());
    this->setIcon(QIcon::fromTheme("taskview",QIcon("/usr/share/ukui-panel/panel/img/taskview.svg")));
    this->setIconSize(QSize(m_plugin->panel()->iconSize(),m_plugin->panel()->iconSize()));
}
TaskViewButton::~TaskViewButton(){
}

void TaskViewButton::realign()
{
    if (m_plugin->panel()->isHorizontal()) {
        this->setFixedSize(m_plugin->panel()->panelSize(),m_plugin->panel()->panelSize());
    } else {
        this->setFixedSize(m_plugin->panel()->panelSize(),m_plugin->panel()->panelSize());
    }
    this->setIconSize(QSize(m_plugin->panel()->iconSize(),m_plugin->panel()->iconSize()));
}

void TaskViewButton::mousePressEvent(QMouseEvent *event)
{
    const Qt::MouseButton b = event->button();
#if 0
    //调用dbus接口
    QString object = QString(getenv("DISPLAY"));
    object = object.trimmed().replace(":", "_").replace(".", "_").replace("-", "_");
    object = "/org/ukui/WindowSwitch/display/" + object;
    QDBusInterface interface("org.ukui.WindowSwitch", object,
                             "org.ukui.WindowSwitch",
                             QDBusConnection::sessionBus());
    if (!interface.isValid()) {
        qCritical() << QDBusConnection::sessionBus().lastError().message();
    }

    if (Qt::LeftButton == b  && interface.isValid())
    {
        /* Call binary display task view
         * system("ukui-window-switch --show-workspace");
         */

        /*调用远程的value方法*/
        QDBusReply<bool> reply = interface.call("handleWorkspace");
        if (reply.isValid()) {
            if (!reply.value())
                qWarning() << "Handle Workspace View Failed";
        } else {
            qCritical() << "Call Dbus method failed";
        }
    }
#endif

    //调用命令
    if (Qt::LeftButton == b){
        system("ukui-window-switch --show-workspace");
    }

    QWidget::mousePressEvent(event);
}
