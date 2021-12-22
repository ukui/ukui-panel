#include "snixembedproxy.h"
#include <QApplication>
#include <QSessionManager>
#include <ukui-log4qt.h>
#include "xcbutilss.h"

namespace Xcb {
    Xcb::Atoms* atoms;
}

int main(int argc, char *argv[])
{
    initUkuiLog4qt("sni-xembed-proxy");

    qputenv("QT_QPA_PLATFORM", "xcb");

    QGuiApplication::setDesktopSettingsAware(false);

    QGuiApplication app(argc, argv);

    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };
    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);

    app.setQuitOnLastWindowClosed(false);


    Xcb::atoms = new Xcb::Atoms();

    SniXembedProxy proxy;

    auto rc = app.exec();

    delete Xcb::atoms;
    return rc;

}
