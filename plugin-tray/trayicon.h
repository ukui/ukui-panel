/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QFrame>
#include <QList>
#include <QStyleOption>
#include <QGSettings>

#include <X11/X.h>
#include <X11/extensions/Xdamage.h>
#include "../panel/highlight-effect.h"
#define TRAY_ICON_SIZE_DEFAULT 16

class QWidget;
class UKUIPanel;
class IUKUIPanelPlugin;

class TrayIcon: public QFrame
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

    QSize sizeHint() const;
    IUKUIPanelPlugin *mPlugin;


public slots:
    void notifyAppFreeze();

signals:
    void notifyTray(Window);

protected:
    bool event(QEvent *event);
    void draw(QPaintEvent* event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

private:
    void init();
    QRect iconGeometry();
    void  trayButtonPress(QMouseEvent *);
    Window mIconId;
    Window mWindowId;
    QSize mIconSize;
    Damage mDamage;
    Display* mDisplay;

    static bool isXCompositeAvailable();
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
    QSize mRectSize;
    QGSettings *gsettings;
    int tray_icon_color;
    bool dark_style;

    enum TrayAppStatus{NORMAL, HOVER, PRESS};
    TrayAppStatus traystatus;

};

#endif // TRAYICON_H
