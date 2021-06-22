#ifndef STARTMENUBUTTON_H
#define STARTMENUBUTTON_H

#include <QToolButton>
#include <QTimer>
#include <QProcess>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QtDBus/QtDBus>
#include <QMouseEvent>
#include <QContextMenuEvent>

#include "../panel/iukuipanelplugin.h"
#include "../panel/customstyle.h"

class StartMenuButton : public QToolButton
{
    Q_OBJECT
public:
    StartMenuButton(IUKUIPanelPlugin *plugin,QWidget* parent = 0);
    ~StartMenuButton();
    void realign();
protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    QMenu *rightPressMenu;
    IUKUIPanelPlugin * mPlugin;
    QString version;
    QWidget *mParent;

    void getOsRelease();
    QString getCanHibernateResult();

private slots:
    void ScreenServer();
    void SessionSwitch();
    void SessionLogout();
    void SessionReboot();
    void TimeShutdown();
    void SessionShutdown();
    void SessionSuspend();
    void SessionHibernate();
};

#endif // STARTMENUBUTTON_H
