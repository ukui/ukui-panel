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
StartMenuButton::StartMenuButton()
{
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
    PopupMenu *menuTaskview=new PopupMenu();
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

    int availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
    menuTaskview->setGeometry(50,availableHeight-140,140,140);
    menuTaskview->show();
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

