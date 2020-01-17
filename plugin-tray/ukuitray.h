/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2019 Kylin team
 * Authors:
 *   hepuyao <hepuyao@kylinos.cn>
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

#ifndef UKUITRAY_H
#define UKUITRAY_H

#include <QFrame>
#include <QAbstractNativeEventFilter>
#include "../panel/iukuipanel.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb_event.h>
#include "fixx11h.h"

class TrayIcon;
class QSize;

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

signals:
    void iconSizeChanged(int iconSize);

private slots:
    void startTray();
    void stopTray();
    void onIconDestroyed(QObject * icon);

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
    TrayIcon* findIcon(Window trayId);

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    int mDamageEvent;
    int mDamageError;
    QSize mIconSize;
    UKUi::GridLayout *mLayout;
    IUKUIPanelPlugin *mPlugin;
    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display* mDisplay;
};


#endif
