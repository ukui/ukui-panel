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
#include "ukuiquicklaunch.h"
#include <QMessageBox>
#include <XdgDesktopFile>
#include <QFileInfo>

/*传入参数为三个字段*/
QuickLaunchAction::QuickLaunchAction(const QString & name,
                                     const QString & exec,
                                     const QString & icon,
                                     QWidget * parent)
    : QAction(name, parent),
      m_valid(true)
{
    m_type = ActionLegacy;

    m_settingsMap["name"] = name;
    m_settingsMap["exec"] = exec;
    m_settingsMap["icon"] = icon;

    if (icon == "" || icon.isNull())
        setIcon(XdgIcon::defaultApplicationIcon());
    else
        setIcon(QIcon(icon));

    setData(exec);
    connect(this, &QAction::triggered, this, [this] { execAction(); });
}

/*用xdg的方式解析*/
QuickLaunchAction::QuickLaunchAction(const XdgDesktopFile * xdg,
                                     QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionXdg;

    m_settingsMap["desktop"] = xdg->fileName();

    QString title(xdg->localizedValue("Name").toString());
    QString icon(xdg->localizedValue("Icon").toString());
    setText(title);

    setIcon(QIcon::fromTheme(icon));

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

QuickLaunchAction::QuickLaunchAction(const QString & fileName, QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionFile;
    setText(fileName);
    setData(fileName);

    m_settingsMap["file"] = fileName;

    QFileInfo fi(fileName);
    if (fi.isDir())
    {
        QFileIconProvider ip;
        setIcon(ip.icon(fi));
    }
    else
    {
        QMimeDatabase db;
        XdgMimeType mi(db.mimeTypeForFile(fi));
        setIcon(mi.icon());
    }

    connect(this, &QAction::triggered, this, [this] { execAction(); });
}

/*解析Exec字段*/
void QuickLaunchAction::execAction(QString additionalAction)
{
    UKUIQuickLaunch *uqk = qobject_cast<UKUIQuickLaunch*>(parent());
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
            if(xdg.load(exec))
            {
                if(exec.contains("ubuntu-kylin-software-center",Qt::CaseSensitive)){
                    //无法打开麒麟应用商店，因此改为gio的方式加载
                    QByteArray ba = exec.toLatin1();
                    char * filepath=ba.data();
                    GDesktopAppInfo * appinfo=g_desktop_app_info_new_from_filename(filepath);
                    if (!g_app_info_launch(G_APP_INFO(appinfo),nullptr, nullptr, nullptr))
                        showQMessage =true;
                    g_object_unref(appinfo);
                 } else {
                    //xdg 的方式实现点击打开应用，可正确读取转义的字符
                    if (!additionalAction.isEmpty()){
                        if (!xdg.startDetached())
                            showQMessage =true;
                    } else {
                        if (!xdg.actionActivate(additionalAction, QStringList{}))
                            showQMessage =true;
                    }
                }
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
