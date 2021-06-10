/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#ifndef UKUITASKBAR_H
#define UKUITASKBAR_H

#include "../panel/iukuipanel.h"
#include "../panel/iukuipanelplugin.h"
#include "ukuitaskgroup.h"
#include "ukuitaskbutton.h"

#include <QFrame>
#include <QBoxLayout>
#include <QMap>
//#include <lxqt-globalkeys.h>
#include "../panel/iukuipanel.h"
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/NETWM>
#include <QStyleOption>
#include <QPainter>
#include <QtDBus/QtDBus>

class QSignalMapper;
class UKUITaskButton;
class ElidedButtonStyle;
class UKUITaskBarIcon;

namespace UKUi {
class GridLayout;
}

class UKUITaskBar : public QFrame
{
    Q_OBJECT

    /*
     * 负责与ukui桌面环境应用通信的dbus
     * 在点击任务栏的时候发给其他UKUI DE APP 点击信号
     * 以适应特殊的设计需求以及
　　　*/
    Q_CLASSINFO("D-Bus Interface", "com.ukui.panel.plugins.taskbar")

public:
    explicit UKUITaskBar(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    virtual ~UKUITaskBar();

    /**
     * @brief realign
     *
     * it's use to realign the position of taskbar, the size and the position of taskgroup buttons and refresh the style of
     *
     * the icon in buttons.
     *
     */
    void realign();

    /**
     * @brief buttonStyle
     *
     * To get the style of the group buttons, the parameter in this class called mButtonStyle.
     *
     * @return Qt::ToolButtonStyle
     */
    Qt::ToolButtonStyle buttonStyle() const { return mButtonStyle; }

    /**
     * @brief buttonWidth
     *
     * To get the width of the buttons, the parameter in this class called mButtonWidth.
     *
     * @return int
     */
    int buttonWidth() const { return mButtonWidth; }

    /**
     * @brief closeOnMiddleClick
     *
     * To get the press of the mid-button on mouse, the parameter in this class called mCloseOnMiddleClick.
     *
     * @return bool
     */
    bool closeOnMiddleClick() const { return mCloseOnMiddleClick; }

    /**
     * @brief raiseOnCurrentDesktop
     *
     * To get the window is on current desktop or not, to judge wether show the button.
     *
     * The parameter in this class called mRaiseOnCurrentDesktop.
     *
     * @return bool
     */
    bool raiseOnCurrentDesktop() const { return mRaiseOnCurrentDesktop; }

    /**
     * @brief isShowOnlyOneDesktopTasks
     *
     * To get the group button contains just one window or more on one desktop.
     *
     * The parameter in this class called mShowOnlyOneDesktopTasks.
     *
     * @return bool
     */
    bool isShowOnlyOneDesktopTasks() const { return mShowOnlyOneDesktopTasks; }

    /**
     * @brief showDesktopNum
     *
     * To get the number of the desktop which group buttons are showed.
     *
     * The parameter in this class called mshowDesktopNum.
     *
     * @return int
     */
    int showDesktopNum() const { return mShowDesktopNum; }

    /**
     * @brief getCpuInfoFlg
     *
     * To get wether the type of the cpu is Loonson.
     *
     * @return
     */
    bool getCpuInfoFlg() const { return CpuInfoFlg; }

    /**
     * @brief isShowOnlyCurrentScreenTasks
     *
     * To get wether the task window is only show on current screen.
     *
     * @return
     */
    bool isShowOnlyCurrentScreenTasks() const { return mShowOnlyCurrentScreenTasks; }

    /**
     * @brief isShowOnlyMinimizedTasks
     *
     * To get wether the task window is only show in minimaized.
     *
     * @return
     */
    bool isShowOnlyMinimizedTasks() const { return mShowOnlyMinimizedTasks; }

    /**
     * @brief isAutoRotate
     *
     * To get wether the auto rotation is on.
     *
     * @return
     */
    bool isAutoRotate() const { return mAutoRotate; }

    /**
     * @brief isGroupingEnabled
     *
     * To get wether the window grouping is enable or not.
     *
     * @return
     */
    bool isGroupingEnabled() const { return mGroupingEnabled; }

    /**
     * @brief isShowGroupOnHover
     *
     * To get wether the button is on hover by mouse or others.
     *
     * @return
     */
    bool isShowGroupOnHover() const { return mShowGroupOnHover; }

    /**
     * @brief isIconByClass
     *
     * To get wether the group icon is in same class of the window or not.
     *
     * @return
     */
    bool isIconByClass() const { return mIconByClass; }

    /**
     * @brief setShowGroupOnHover
     *
     * To set the flag of group button on hover.
     *
     * @param bFlag
     */
    void setShowGroupOnHover(bool bFlag);
    inline IUKUIPanel * panel() const { return mPlugin->panel(); }
    inline IUKUIPanelPlugin * plugin() const { return mPlugin; }
    inline UKUITaskBarIcon* fetchIcon()const{return mpTaskBarIcon;}
    bool ignoreSymbolCMP(QString filename,QString groupname);

signals:
    void buttonRotationRefreshed(bool autoRotate, IUKUIPanel::Position position);
    void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
    void refreshIconGeometry();
    void showOnlySettingChanged();
    void iconByClassChanged();
    void popupShown(UKUITaskGroup* sender);
    void sendToUkuiDEApp(void);

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private slots:
    void refreshTaskList();
    void refreshButtonRotation();
    void refreshPlaceholderVisibility();
    void groupBecomeEmptySlot();
    void onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);
    void onWindowAdded(WId window);
    void onWindowRemoved(WId window);
    void registerShortcuts();
    void shortcutRegistered();
    void activateTask(int pos);
    void wl_kwinSigHandler(quint32 wl_winId, int opNo, QString wl_iconName, QString wl_caption);

private:
    typedef QMap<WId, UKUITaskGroup*> windowMap_t;

private:
    void addWindow(WId window);
    void addWindow_wl(QString iconName, QString caption, WId window);
    QHash<QString,QString> matchAndroidIcon();
    windowMap_t::iterator removeWindow(windowMap_t::iterator pos);
    void buttonMove(UKUITaskGroup * dst, UKUITaskGroup * src, QPoint const & pos);

    QString captionExchange(QString str);

    enum TaskStatus{NORMAL, HOVER, PRESS};
    TaskStatus taskstatus;

private:
    QMap<WId, UKUITaskGroup*> mKnownWindows; //!< Ids of known windows (mapping to buttons/groups)
    QHash<QString,QString> mAndroidIconHash;
    UKUi::GridLayout *mLayout;
//    QList<GlobalKeyShortcut::Action*> mKeys;
    QSignalMapper *mSignalMapper;

    // Settings
    Qt::ToolButtonStyle mButtonStyle;
    int mButtonWidth;
    int mButtonHeight;
    bool CpuInfoFlg = true;
    bool mCloseOnMiddleClick;
    bool mRaiseOnCurrentDesktop;
    bool mShowOnlyOneDesktopTasks;
    int mShowDesktopNum;
    bool mShowOnlyCurrentScreenTasks;
    bool mShowOnlyMinimizedTasks;
    bool mAutoRotate;
    bool mGroupingEnabled;
    bool mShowGroupOnHover;
    bool mIconByClass;
    bool mCycleOnWheelScroll; //!< flag for processing the wheelEvent

    bool acceptWindow(WId window) const;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);
    void settingsChanged();

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);

    IUKUIPanelPlugin *mPlugin;
    QWidget *mPlaceHolder;
    LeftAlignedTextStyle *mStyle;
    UKUITaskBarIcon *mpTaskBarIcon;

    QGSettings *changeTheme;
};

#endif // UKUITASKBAR_H
