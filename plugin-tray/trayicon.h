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


#ifndef TRAYICON_H
#define TRAYICON_H
#include "../panel/iukuipanelplugin.h"
//Qt
#include <QObject>
#include <QFrame>
#include <QList>
#include <QStyleOption>
#include <QMimeData>
#include <QToolButton>
#include <QMenu>
#include <QGSettings>
//X11
#include <X11/X.h>
#include <X11/extensions/Xdamage.h>
//panel
#include "../panel/highlight-effect.h"

#define TRAY_ICON_SIZE_DEFAULT 16

class QWidget;
class UKUIPanel;
class IUKUIPanelPlugin;

/**
 * @brief The TrayIcon class
 * 承载托盘应用
 */
class TrayIcon: public QToolButton
{
    Q_OBJECT

    /*
     * 负责与ukui桌面环境托盘应用通信的dbus
     * 在点击托盘图标外部托盘按钮内部的区域时发给其他托盘应用 点击信号
     * 托盘应用收到此信号应实现　show/hide 主界面的操作
　　　*/
    Q_CLASSINFO("D-Bus Interface", "com.ukui.panel.plugins.tray")

    enum EffectMode {
        HighlightOnly,
        BothDefaultAndHighlit
    };
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

public:
    TrayIcon(Window iconId, QSize const & iconSize, QWidget* parent);
    virtual ~TrayIcon();

    Window iconId() { return mIconId; }
    Window windowId() { return mWindowId; }
    void windowDestroyed(Window w);

    QSize iconSize() const { return mIconSize; }
    void setIconSize(QSize iconSize);
    /**
     * @brief moveMenu  右键菜单，移动图标至任务栏/收纳
     */
    void moveMenu();

    QSize sizeHint() const;

    static QString mimeDataFormat() { return QLatin1String("x-ukui/tray-button"); }


public slots:
    void notifyAppFreeze();
    void emitIconId();

signals:
    void notifyTray(Window);
    void iconIsMoving(Window);
    void switchButtons(TrayIcon *from, TrayIcon *to);

protected:
    bool event(QEvent *event);
    void draw(QPaintEvent* event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

    //拖拽相关
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent * e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    virtual QMimeData * mimeData();

private:
    void init();
    QRect iconGeometry();
    void  trayButtonPress(QMouseEvent *);
    bool needReDraw();
    /**
     * @brief caculateMenuWindowPos
     * 计算右键菜单的位置
     * @param absolutePos
     * @param windowSize menu的sizeHint
     * @return 返回值是QRect，可直接使用，无需转换
     */
    QRect caculateMenuWindowPos(QPoint const & absolutePos, QSize const & windowSize);
    QPoint mDragStart;

    IUKUIPanelPlugin *mPlugin;
    Window mIconId;
    Window mWindowId;
    QSize mIconSize;
    Damage mDamage;
    Display* mDisplay;
    QMenu *menu;

    static bool isXCompositeAvailable();
    QSize mRectSize;

    enum TrayAppStatus{NORMAL, HOVER, PRESS};
    TrayAppStatus traystatus;

    QGSettings *scaling_settings;
};

/**
 * @brief The TrayButtonMimeData class
 * 拖拽托盘按钮的时候用到的TrayButtonMimeData
 */
class TrayButtonMimeData: public QMimeData
{
    Q_OBJECT
public:
    TrayButtonMimeData():
        QMimeData(),
        mButton(0)
    {
    }

    TrayIcon *button() const { return mButton; }
    void setButton(TrayIcon *button) { mButton = button; }

private:
    TrayIcon *mButton;
};

#endif // TRAYICON_H
