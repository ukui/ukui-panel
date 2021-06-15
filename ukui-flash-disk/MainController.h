#ifndef _MAINCONTROL_H_
#define _MAINCONTROL_H_

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>

#include "mainwindow.h"
#include "MacroFile.h"

class MainController : public QObject
{
    Q_OBJECT
public:
    static MainController* self();
    virtual ~MainController();
    int init();
private:
    explicit MainController();

public:
Q_SIGNALS:
    void notifyWnd(QObject* obj, QEvent *event);
    
private:
    static MainController *mSelf;
    MainWindow *m_DiskWindow;
};

#endif //_MAINCONTROL_H_
