#include "taskview_button.h"

TaskViewButton::TaskViewButton(IUKUIPanelPlugin *plugin,QWidget *parent):
    mParent(parent)
{
    this->setParent(parent);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyle(new CustomStyle());
    this->setIcon(QIcon::fromTheme("taskview",QIcon("/usr/share/ukui-panel/panel/img/taskview.svg")));
    this->setIconSize(QSize(mParent->height()*0.6,mParent->height()*0.6));
}
TaskViewButton::~TaskViewButton(){
}

void TaskViewButton::realign()
{
    this->setIconSize(QSize(mParent->height()*0.6,mParent->height()*0.6));
}

void TaskViewButton::mousePressEvent(QMouseEvent *event)
{
    const Qt::MouseButton b = event->button();
    //调用dbus接口
#if 0
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
