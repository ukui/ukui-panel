/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * Copyright: 2019-2020 UKUI team
 * Modified by  hepuyao <hepuyao@kylinos.cn>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "ukuipanelapplication.h"
#include <QTranslator>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
/*! The ukui-panel is the panel of UKUI.
  Usage: ukui-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.config/ukui/panel.conf
                   (default main)
 */

int main(int argc, char *argv[])
{
    UKUIPanelApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    //Singleton
    int fd = open("/tmp/ukui-panel-lock", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) { exit(1); }
    if (lockf(fd, F_TLOCK, 0)) {
        syslog(LOG_ERR, "Can't lock single file, kylin-network-manager is already running!");
        qDebug()<<"Can't lock single file, kylin-network-manager is already running!";
        exit(0);
    }

    //tanslate
    QString locale = QLocale::system().name();
    QTranslator translator;
    if (locale == "zh_CN"){
        if (translator.load("ukui-panel_zh_CN.qm", "/usr/share/ukui-panel/plugin-startmenu/img"))
            app.installTranslator(&translator);
        else
            qDebug() << "Load translations file" << locale << "failed!";
    }

    return app.exec();
}
