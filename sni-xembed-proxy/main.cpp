/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

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

    QApplication app(argc, argv);

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
