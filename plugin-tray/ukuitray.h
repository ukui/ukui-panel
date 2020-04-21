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
#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb_event.h>
#include "fixx11h.h"
#include <QScreen>
#include <QGSettings>
#include <string.h>
#include <string>
#include <QToolButton>
#include <QMap>

#include "../panel/iukuipanel.h"
#include "../panel/customstyle.h"
#include "../panel/ukuicontrolstyle.h"
#include "traystorage.h"

class TrayIcon;
class QSize;
namespace UKUi {
class GridLayout;
}
class UKUiStorageWidget;
/**
 * @brief This makes our trayplugin
 */
class UKUITrayPlugin;
enum storageBarStatus{ST_HIDE,ST_SHOW};
/**
 * @brief This makes our storage
 */
class UKUIStorageFrame:public QWidget
{
    Q_OBJECT
public:
    UKUIStorageFrame(QWidget* parent =0);
    ~UKUIStorageFrame();
protected:
//    bool event(QEvent *);
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *event)override;
private:
    Atom _NET_SYSTEM_TRAY_OPCODE;
};

class UKUITray: public QFrame, QAbstractNativeEventFilter
{
    Q_OBJECT
//    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    UKUITray(UKUITrayPlugin *plugin, QWidget* parent = 0);
    ~UKUITray();

    QSize iconSize() const { return mIconSize; }
    void setIconSize();
    void setStorageBar(TrayStorage *pTys);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
    void realign();
    UKUITrayPlugin *mPlugin;

    //control app show in tray/traystorege  by ukui-control-center
    QList<char *> listExistsPath();
    QString findFreePath();
    void regulateIcon(Window *mid);
    void freezeApp();
    void showAndHideStorage(bool);

public slots:
    void storageBar();
    void changeIcon();

signals:
    void iconSizeChanged(int iconSize);
    void freezeIcon(TrayIcon *icon,Window winid);
    void positionChanged();

private slots:
    void startTray();
    void stopTray();
    void stopStorageTray();
    void onIconDestroyed(QObject * icon);
    void freezeTrayApp(Window winId);
private:
    VisualID getVisual();
    void clientMessageEvent(xcb_generic_event_t *e);
    int clientMessage(WId _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;

    /*
     * @brief Dynamic mobile tray application
     */
    void addTrayIcon(Window id);
    void addStorageIcon(Window winId);
    void addHideIcon(Window winId);
    void moveIconToStorage(Window id);
    void moveIconToTray(Window winId);
    void moveIconToHide(Window winId);
    void handleStorageUi();
    TrayIcon* findIcon(Window trayId);
    TrayIcon* findTrayIcon(Window trayId);
    TrayIcon* findStorageIcon(Window trayId);
    TrayIcon* findHideIcon(Window trayId);
    void createIconMap();

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    QList<TrayIcon*> mTrayIcons;
    QList<TrayIcon*> mStorageIcons;
    QList<TrayIcon*> mHideIcons;
    int mDamageEvent;
    int mDamageError;
    QSize mIconSize;

    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display* mDisplay;
    UKUIStorageFrame *storageFrame;
    UKUiStorageWidget *m_pwidget;
    QToolButton *mBtn;
    IUKUIPanel::Position mCurPosition;
    QMap<IUKUIPanel::Position, QIcon> mMapIcon;
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
};

class UKUiStorageWidget:public QWidget
{
public:
    UKUiStorageWidget();
    ~UKUiStorageWidget();
protected:
    void paintEvent(QPaintEvent*);
};
#endif
