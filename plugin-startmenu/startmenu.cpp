#include "startmenu.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QMenu>
StartMenu::StartMenu(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    realign();
//    mCapturing = false;
//    connect(&mButton, SIGNAL(clicked()), this, SLOT(captureMouse()));




}


StartMenu::~StartMenu()
{
}


void StartMenu::realign()
{
//    mButton.setFixedSize(46,panel()->panelSize());
}

StartMenuWidget::StartMenuWidget(QWidget *parent):
    QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (1);
    setLayout(layout);
    layout->addWidget (&mButton);


    //mButton.setAutoRaise(true);
    mButton.setFixedSize(46,46);

    mCapturing = false;
    connect(&mButton, SIGNAL(clicked()), this, SLOT(captureMouse()));

    mButton.setFixedSize(46,46);
    mButton.setStyleSheet(
                //正常状态样式
                "QToolButton{"
                /*"background-color:rgba(100,225,100,80%);"//背景色（也可以设置图片）*/
                "qproperty-icon:url(/usr/share/ukui-panel/plugin-startmenu/img/startmenu.svg);"
                "qproperty-iconSize:40px 40px;"
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-width:0px;"                     //边框宽度像素
                "border-radius:0px;"                   //边框圆角半径像素
                "border-color:rgba(255,255,255,30);"    //边框颜色
                "font:SimSun 14px;"                       //字体，字体大小
                "color:rgba(0,0,0,100);"                //字体颜色
                "padding:0px;"                          //填衬
                "border-bottom-style:solid"
                "}"
                //鼠标悬停样式
                "QToolButton:hover{"
                "background-color:rgba(190,216,239,20%);"
                "}"
                //鼠标按下样式
                "QToolButton:pressed{"
                "background-color:rgba(190,216,239,12%);"
                "}"
                );
}




StartMenuWidget::~StartMenuWidget()
{
}

void StartMenuWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    if (!mCapturing)
//        return;
//    WId id = QApplication::desktop()->winId();
//    qDebug()<<id<<endl;
}


void StartMenuWidget::captureMouse()
{
    if(QFileInfo::exists(QString("/usr/bin/ukui-menu")))
    {
    QProcess *process =new QProcess(this);
    process->startDetached("/usr/bin/ukui-menu");
    }
    else{qDebug()<<"not find /usr/bin/ukui-start-menu"<<endl;}
}

void StartMenuWidget::contextMenuEvent(QContextMenuEvent *event)
{
  //创建一个菜单 添加事件
    qDebug()<<"contextMenuEvent    right press event";
    PopupMenu *menuTaskview=new PopupMenu();
    menuTaskview->setAttribute(Qt::WA_DeleteOnClose);

    menuTaskview->setStyleSheet(
                         "QMenu {"
                         "background-color:rgb(21,26,30);"
                         "border: 1px solid #626c6e;"
                         //"border-color:rgba(255,255,255,30);"    //边框颜色
                         "font:SimSun 14px;"                       //字体，字体大小
                         "color:rgba(255,255,255,100);"                //字体颜色
                        "padding: 4px 2px 4px 2px;"
                        " }"
                        "QMenu::item {"
                        "width: 244px;"
                        "height: 90px;"
                        "}"
                        //鼠标悬停样式
                        "QMenu:hover{"
                        "background-color:rgba(190,216,239,30%);"
                        "}"
                        //鼠标按下样式
                        "QMenu:selected{"
                        "background-color:rgba(190,216,239,30%);"
                        "}"
                        );

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
void StartMenuWidget::ScreenServer()
{
    system("ukui-screensaver-command -l");
}
void StartMenuWidget::SessionSwitch()
{
    QProcess::startDetached(QString("ukui-session-tools --switchuser"));
}

void StartMenuWidget::SessionLogout()
{
    system("ukui-session-tools --logout");
}

void StartMenuWidget::SessionReboot()
{
    system("ukui-session-tools --reboot");
}

void StartMenuWidget::SessionShutdown()
{
    system("ukui-session-tools --shutdown");
}

