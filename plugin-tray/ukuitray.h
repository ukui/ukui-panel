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
#include "ukuitraystrage.h"
#include "storagearrow.h"
#include "ukuistoragewidget.h"
class TrayIcon;
class QSize;
namespace UKUi {
class GridLayout;
}

/**
 * @brief This makes our trayplugin
 */
class UKUITrayPlugin;

class UKUITray: public QFrame, QAbstractNativeEventFilter
{
    Q_OBJECT
//    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    /**
     * @brief UKUITray
     * @param plugin
     * @param parent
     * 托盘应用共分为两个区域 Tray Storage
     * Tray区域 ：UKUITray ； Storage区域：UKUIStorageFrame
     * 图标可存放在两个区域中的任何一个区域，如果应用退出则被销毁
     */
    UKUITray(UKUITrayPlugin *plugin, QWidget* parent = 0);
    ~UKUITray();

    QSize iconSize() const { return mIconSize; }
    /**
     * @brief setIconSize
     * 目前托盘应用不使用此方式设置控件的大小而是使用setIconSize和setFixedSize来设置
     */
    void setIconSize();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
    UKUITrayPlugin *mPlugin;
    /**
     * @brief regulateIcon
     * @param mid
     * 调节图标，监听图标所在的位置
     */
    void regulateIcon(Window *mid);
    /**
     * @brief newAppDetect
     * @param wid 应用的窗口id
     * 检测到新的应用（第一次添加应用）
     */
    void newAppDetect(int wid);
//    /**
//     * @brief freezeApp
//     * 将所有的托盘应用的状态至为freeze
//     * 一般存在与在任务栏退出的时候
//     */
//    void freezeApp();
    /**
     * @brief showAndHideStorage
     * 在取消了panel的WindowDoesNotAcceptFocus属性之后，托盘栏会有点击之后的隐藏并再次弹出的操作
     */
    void showAndHideStorage(bool);

public slots:
    /**
     * @brief storageBar
     * 点击收纳按钮的时候的槽函数
     */
    void storageBar();
    /**
     * @brief realign
     * 关于设置托盘栏图标大小的方法
     * ukui采用与lxqt-panel不同的方式
     * 直接在realign函数中进行设置每个托盘应用所在的位置的大小和位置
     */
    void realign();

signals:
    void iconSizeChanged(int iconSize);
    void freezeIcon(TrayIcon *icon,Window winid);
    void positionChanged();

private slots:
    /**
     * @brief startTray
     * freedesktop systray specification
     * 托盘规范
     */
    void startTray();
    void stopTray();
    void stopStorageTray();
    /**
     * @brief onIconDestroyed
     * @param icon
     * 将托盘图标从托盘栏/收纳栏中移除
     */
    void onIconDestroyed(QObject * icon);
    /**
     * @brief freezeTrayApp
     * @param winId
     * 将托盘应用置为freeze的状态
     */
    void freezeTrayApp(Window winId);
    void trayIconSizeRefresh();
    /**
     * @brief switchButtons
     * @param button1 移动前的图标
     * @param button2 移动后的图标
     * 交换两个图标
     */
    void switchButtons(TrayIcon *button1, TrayIcon *button2);

private:
    VisualID getVisual();
    void clientMessageEvent(xcb_generic_event_t *e);
    int clientMessage(WId _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;

    /**
     * @brief addTrayIcon
     * @param id
     * 添加托盘应用到托盘栏
     */
    void addTrayIcon(Window id);
    void addStorageIcon(Window winId);
    void moveIconToStorage(Window id);
    void moveIconToTray(Window winId);
    /**
     * @brief handleStorageUi
     * 收纳栏样式
     * 根据panel的大小自动调节收纳栏
     * 未设置m_pwidget的透明属性会导致收纳栏异常（布局混乱）
     * 因此不使用QWidget，而是继承 QWidget 写一个 UKUiStorageWidget，并设置其基础样式
     */
    void handleStorageUi();
    TrayIcon* findIcon(Window trayId);
    TrayIcon* findTrayIcon(Window trayId);
    TrayIcon* findStorageIcon(Window trayId);

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    QList<TrayIcon*> mTrayIcons;
    QList<TrayIcon*> mStorageIcons;
    int mDamageEvent;
    int mDamageError;
    QSize mIconSize;
    /**
     * @brief mLayout
     * 托盘上的图标布局
     */
    UKUi::GridLayout *mLayout;
    /**
     * @brief mStorageLayout
     * 收纳栏上布局
     */
    UKUi::GridLayout *mStorageLayout;
    /**
     * @brief mStorageItemLayout
     * 收纳栏上的图标布局
     */
    UKUi::GridLayout *mStorageItemLayout;

    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display* mDisplay;
    UKUIStorageFrame *storageFrame;
    UKUiStorageWidget *m_pwidget;
    StorageArrow *mBtn;
    IUKUIPanel::Position mCurPosition;
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
    QGSettings *settings;
};

#endif
