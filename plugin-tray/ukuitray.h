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
#include <QProcess>

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

    /** @brief nativeEventFilter
     * 托盘应用的事件过滤器
     * 通过继承QAbstractNativeEventFilter的类中重新实现nativeEventFilter接口:
     * 安装　：　void QCoreApplication::installNativeEventFilter(QAbstractNativeEventFilter *filterObj)
     * 或者　　　void QAbstractEventDispatcher::installNativeEventFilter(QAbstractNativeEventFilter *filterObj)
     * XCB(Linux)　对应的eventType 类型如下：
     * 事件类型(eventType)：“xcb_generic_event_t”　　　消息类型(message)：xcb_generic_event_t *	　结果类型(result)：无
    */
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
    QStringList getShowInTrayApp();
    /**
     * @brief showAndHideStorage
     * 在取消了panel的WindowDoesNotAcceptFocus属性之后，托盘栏会有点击之后的隐藏并再次弹出的操作
     */
    void showAndHideStorage(bool);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
public slots:
    /**
     * @brief storageBar
     * 点击收纳按钮的时候的槽函数
     */
    void storageBar();
    void hideStorageWidget();
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
//    void stopStorageTray();
    /**
     * @brief onIconDestroyed
     * @param icon
     * 将托盘图标从托盘栏/收纳栏中移除
     */
    void onIconDestroyed(QObject * icon);
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
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
    QGSettings *settings;

    //针对ukui桌面环境应用的特殊处理
    bool fcitx_flag = false;
    /**
     * @brief panelStartupFcitx
     * 任务栏拉起fcitx
     */
    void panelStartupFcitx();
};

#endif
