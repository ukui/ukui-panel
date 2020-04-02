/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2019 Kylin team
 * Authors:
 *   hepuyao <hepuyao@kylinos.cn>
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

#ifndef UKUITRAY_H
#define UKUITRAY_H

#include <QFrame>
#include <QAbstractNativeEventFilter>
#include "../panel/iukuipanel.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb_event.h>
#include "fixx11h.h"
#include <QScreen>
#include "traystorage.h"
#include "../panel/customstyle.h"
#include <QGSettings>
#include <string.h>
#include <string>
class TrayIcon;
class QSize;
//class CustomStyle;
namespace UKUi {
class GridLayout;
}

/**
 * @brief This makes our trayplugin
 */
class IUKUIPanelPlugin;

class UKUITray: public QFrame, QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    UKUITray(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~UKUITray();

    QSize iconSize() const { return mIconSize; }
    void setIconSize(QSize iconSize);

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

    void realign();
    IUKUIPanelPlugin *mPlugin;
    TrayStorage *tys;

    //control app show in tray/traystorege  by ukui-control-center
    QList<char *> listExistsPath();
    QString findFreePath();

    void regulateIcon(Window *mid);
    void freezeApp();

public slots:
    void storageBar();
    void storageAddIcon(Window winId);

signals:
    void iconSizeChanged(int iconSize);
    void freezeIcon(TrayIcon *icon,Window winid);

private slots:
    void startTray();
    void stopTray();
    void stopStorageTray();
    void onIconDestroyed(QObject * icon);
    void freezeTrayApp(Window winId);
    void freezeIconSlot(TrayIcon *icon,Window winid);
private:
    VisualID getVisual();
    void clientMessageEvent(xcb_generic_event_t *e);
    int clientMessage(WId _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;
    void addIcon(Window id);
    void moveIcon(Window id);
    void storageMoveIcon(Window winId);
    TrayIcon* findIcon(Window trayId);
    TrayIcon* findStorageIcon(Window trayId);

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    QList<TrayIcon*> mStorageIcons;
    int mDamageEvent;
    int mDamageError;
    QSize mIconSize;
    UKUi::GridLayout *mLayout;
    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display* mDisplay;
//    QGSettings *settings;
};
#endif
