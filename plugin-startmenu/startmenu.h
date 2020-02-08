#ifndef STARTMENU_H
#define STARTMENU_H

#include "../panel/iukuipanelplugin.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFrame>
#include <QFontMetrics>
#include <QLineEdit>
#include <QToolButton>
#include <XdgIcon>


#include <QMainWindow>
#include <QHBoxLayout>
#include <QDebug>
#include <QProcess>

#include "../panel/plugin.h"
#include "../panel/ukuipanel.h"
//#include "../panel/config/configpaneldialog.h"
#include "../panel/config/configpanelwidget.h"
#include <../panel/popupmenu.h>
#include <QMenu>
#include <QtDBus/QtDBus>

class  StartMenuWidget: public QFrame
{
    Q_OBJECT
public:
    StartMenuWidget(QWidget* parent = nullptr);
    ~StartMenuWidget();

    QLineEdit *lineEdit() { return &mLineEdit; }
    QToolButton *button() { return &mButton; }


protected:
    void mouseReleaseEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void captureMouse();
//    void this_customContextMenuRequested(const QPoint & pos);
    void ScreenServer();
    void SessionSwitch();
    void SessionLogout();
    void SessionReboot();
    void SessionShutdown();

private:
    QLineEdit mLineEdit;
    QToolButton mButton;
    bool mCapturing;
    QMenu *mRightMenu;
    IUKUIPanelPlugin * mPlugin;
};

class StartMenu : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    StartMenu(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~StartMenu();

    virtual QWidget *widget() { return &mWidget; }
    virtual QString themeId() const { return QStringLiteral("startmenu"); }
    void realign();
    virtual IUKUIPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
private:
    StartMenuWidget mWidget;
//    IUKUIPanelPlugin *mPlugin;
//    QToolButton mButton;
//    bool mCapturing;

//private slots:
//    void captureMouse();

};

class StartMenuLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new StartMenu(startupInfo);
    }
};


#endif
