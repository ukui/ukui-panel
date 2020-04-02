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
private:
    explicit MainController();
    void init();
    void creatDBusService();
    int IsNotRunning();
private:
    static MainController *mSelf;
    MainWindow *m_DiskWindow;
};

#endif //_MAINCONTROL_H_
