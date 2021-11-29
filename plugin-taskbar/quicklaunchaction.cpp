/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
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

#include <gio/gdesktopappinfo.h>
#include "quicklaunchaction.h"
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <XdgDesktopFile>
#include <XdgIcon>
#include <XdgMimeType>
#include "ukuitaskbar.h"
#include <QMessageBox>
#include <XdgDesktopFile>
#include <QFileInfo>
#include <QtX11Extras/QX11Info>
#include <kstartupinfo.h>

#define USE_STARTUP_INFO true

/*用xdg的方式解析*/
QuickLaunchAction::QuickLaunchAction(const XdgDesktopFile * xdg,
                                     QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionXdg;

    m_settingsMap["desktop"] = xdg->fileName();

    QString title(xdg->localizedValue("Name").toString());
    QIcon icon=QIcon::fromTheme(xdg->localizedValue("Icon").toString());
    //add special path search /use/share/pixmaps
    if (icon.isNull())
    {
        QString path = QString("/usr/share/pixmaps/%1.%2").arg(xdg->localizedValue("Icon").toString()).arg("png");
        QString path_svg = QString("/usr/share/pixmaps/%1.%2").arg(xdg->localizedValue("Icon").toString()).arg("svg");
        //qDebug() << "createDesktopFileThumbnail path:" <<path;
        if(QFile::exists(path)){
            icon=QIcon(path);
        }
        else if(QFile::exists(path_svg)){
            icon=QIcon(path_svg);
        }
    }
    if (icon.isNull())
        icon = xdg->icon();
    setText(title);

    setIcon(icon);

    setData(xdg->fileName());
    connect(this, &QAction::triggered, this, [this] { execAction(); });

    // populate the additional actions
    for (auto const & action : const_cast<const QStringList &&>(xdg->actions()))
    {
        QAction * act = new QAction{xdg->actionIcon(action), xdg->actionName(action), this};
        act->setData(action);
        connect(act, &QAction::triggered, [this, act] { execAction(act->data().toString()); });
        m_addtitionalActions.push_back(act);
    }
}

#if USE_STARTUP_INFO
void pid_callback(GDesktopAppInfo *appinfo, GPid pid, gpointer user_data) {
    KStartupInfoId* startInfoId = static_cast<KStartupInfoId*>(user_data);
    KStartupInfoData data;
    data.addPid(pid);
    data.setIconGeometry(QRect(0, 0, 1, 1));  // ugly

    KStartupInfo::sendChange(*startInfoId, data);
    KStartupInfo::resetStartupEnv();

    g_object_unref(appinfo);
    delete startInfoId;
}
#endif

/*解析Exec字段*/
void QuickLaunchAction::execAction(QString additionalAction)
{
    UKUITaskBar *uqk = qobject_cast<UKUITaskBar*>(parent());
    QString exec(data().toString());
    bool showQMessage = false;
    switch (m_type)
    {
        case ActionLegacy:
            if (!QProcess::startDetached(exec))
                showQMessage =true;
            break;
        case ActionXdg: {
            XdgDesktopFile xdg;
            if (xdg.load(exec))
            {
                if (additionalAction.isEmpty()) {
#if USE_STARTUP_INFO
                    bool needCleanup = true;
                    QWidget * pw = static_cast<QWidget*>(parent());
                    QRect rect = pw->geometry();
                    rect.moveTo(pw->mapToGlobal(QPoint(0, 0)));

                    quint32 timeStamp = QX11Info::isPlatformX11() ? QX11Info::appUserTime() : 0;
                    KStartupInfoId* startInfoId = new KStartupInfoId();
                    startInfoId->initId(KStartupInfo::createNewStartupIdForTimestamp(timeStamp));
                    startInfoId->setupStartupEnv();
                    KStartupInfoData data;
                    data.setHostname();
                    data.setName(exec);
                    data.setIconGeometry(rect);
                    data.setLaunchedBy(getpid());
                    data.setDescription("Launch by ukui-panel");
                    KStartupInfo::sendStartup(*startInfoId, data);

                    GDesktopAppInfo * appinfo=g_desktop_app_info_new_from_filename(xdg.fileName().toStdString().data());
                    needCleanup = !g_desktop_app_info_launch_uris_as_manager(appinfo, nullptr, nullptr, 
                                                                             GSpawnFlags::G_SPAWN_DEFAULT, nullptr, nullptr, 
                                                                             pid_callback, (gpointer)startInfoId, nullptr);
                    if (needCleanup) {
                        showQMessage =true;
                        delete startInfoId;
                        g_object_unref(appinfo);
                    }
#else
                    GDesktopAppInfo * appinfo=g_desktop_app_info_new_from_filename(xdg.fileName().toStdString().data());
                    if (!g_app_info_launch_uris(G_APP_INFO(appinfo),nullptr, nullptr, nullptr))
                        showQMessage =true;
                    g_object_unref(appinfo);
#endif
                } else {
                    if (!xdg.actionActivate(additionalAction, QStringList{}))
                        showQMessage =true;
                }
#if 0
                 } else {
                    //xdg 的方式实现点击打开应用，可正确读取转义的字符
                    if (additionalAction.isEmpty()){
                        if (!xdg.startDetached())
                            showQMessage =true;
                    } else {
                        if (!xdg.actionActivate(additionalAction, QStringList{}))
                            showQMessage =true;
                    }
                }
#endif
            } else
                showQMessage =true;
        }
            break;
        case ActionFile:
            QFileInfo fileinfo(exec);
            QString openfile = exec;
            if (fileinfo.isSymLink()) {
                openfile = fileinfo.symLinkTarget();
            }
            if (fileinfo.exists()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(openfile));
            } else {
                showQMessage =true;
            }
            break;
    }
    if (showQMessage) {
        qWarning() << "XdgDesktopFile" << exec << "is not valid";
        QMessageBox::information(uqk, tr("Error Path"),
                                 tr("File/URL cannot be opened cause invalid path.")
                                 );
    }
}

QIcon QuickLaunchAction::getIconfromAction() {
     return this->icon();
}
