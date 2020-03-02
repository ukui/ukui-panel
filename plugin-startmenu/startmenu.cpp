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
    mWidget.setFixedSize(panel()->panelSize(),panel()->panelSize());
}

StartMenuWidget::StartMenuWidget(QWidget *parent):
    QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (1);
    setLayout(layout);
    layout->addWidget (&mButton);

    mCapturing = false;
    connect(&mButton, SIGNAL(clicked()), this, SLOT(captureMouse()));

    QSize mBtnSize(92,92);
    mButton.setIconSize(mBtnSize);
    mButton.setStyleSheet(
                //正常状态样式
                "QToolButton{"
                "background-color:rgba(190,216,239,0%);"
                "qproperty-icon:url(/usr/share/ukui-panel/plugin-startmenu/img/startmenu.svg);"
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-color:rgba(190,216,239,0%);"    //边框颜色
                "border-width:2px;"                     //边框宽度像素
                "border-radius:6px;"                   //边框圆角半径像素
                "padding:2px;"                          //填衬
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
    PopupMenu *menuTaskview=new PopupMenu();
    menuTaskview->setAttribute(Qt::WA_DeleteOnClose);

//    menuTaskview->setStyleSheet(
//                         "QMenu {"
//                         "background-color:rgb(21,26,30);"
//                         "border: 1px solid #626c6e;"
//                         //"border-color:rgba(255,255,255,30);"    //边框颜色
//                         "font:SimSun 14px;"                       //字体，字体大小
//                         "color:rgba(255,255,255,100);"                //字体颜色
//                        "padding: 4px 2px 4px 2px;"
//                        " }"
//                        "QMenu::item {"
//                        "width: 244px;"
//                        "height: 90px;"
//                        "}"
//                        //鼠标悬停样式
//                        "QMenu:hover{"
//                        "background-color:rgba(190,216,239,30%);"
//                        "}"
//                        //鼠标按下样式
//                        "QMenu:selected{"
//                        "background-color:rgba(190,216,239,30%);"
//                        "}"
//                        );

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
    //this way was error
//    mPlugin->willShowWindow(menuTaskview);
//    menuTaskview->popup(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal({0, 0}), menuTaskview->sizeHint()).topLeft());
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

