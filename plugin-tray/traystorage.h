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

/*
 * old ukui-panel plugin-tray storatge pattern
 * but after ukui-panel-2.0.3-1
 * new show storage bar is class UKUIStorageFrame
 * i will delete the file in the near future
*/

#ifndef TRAYSTORAGE_H
#define TRAYSTORAGE_H

#include <QFrame>
#include <QAbstractNativeEventFilter>
#include "../panel/iukuipanel.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb_event.h>
#include <QHBoxLayout>
#include "fixx11h.h"
#include <QPainter>
#include <QStyleOption>
class TrayIcon;
class QSize;

namespace UKUi {
class GridLayout;
}
enum TrayStorageStatus{HIDE,HOVER};

/**
 * @brief This makes our trayplugin
 */
class IUKUIPanelPlugin;

class TrayStorage: public QFrame, QAbstractNativeEventFilter
{
    Q_OBJECT
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    TrayStorage(QWidget* parent = 0);
    ~TrayStorage();
    friend class UKUITray;


    QSize iconSize() const { return mIconSize; }
    void setIconSize(QSize iconSize);

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

    void mouseReleaseEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent *);
//    QWidget *horizontalLayoutWidget;
//    QHBoxLayout *horizontalLayout;


signals:
    void iconSizeChanged(int iconSize);

public slots:
    void onIconDestroyed(QObject * icon);

private slots:
    void stopTrayStorage();

protected:
    bool event(QEvent *event);//重写窗口事件
    void leaveEvent(QEvent *);

private:
    VisualID getVisual();

    int clientMessage(WId _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    int mDamageEvent;
    int mDamageError;
    QSize mIconSize;
    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display* mDisplay;
    UKUi::GridLayout *mLayout;
};

#endif
