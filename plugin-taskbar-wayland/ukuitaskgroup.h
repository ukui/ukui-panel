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

#ifndef UKUITASKGROUP_H
#define UKUITASKGROUP_H

#include "../panel/iukuipanel.h"
#include "../panel/iukuipanelplugin.h"
#include "ukuitaskbar.h"
#include "ukuigrouppopup.h"
#include "ukuitaskwidget.h"
#include "ukuitaskbutton.h"
#include <KF5/KWindowSystem/kwindowsystem.h>
#include <QTimer>
#include <QScrollArea>
#include "../panel/ukuipanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"
#include "../plugin-quicklaunch/quicklaunchaction.h"
#include <QAbstractButton>
//#include <Xlib.h>

#define DEKSTOP_FILE_PATH                   "/usr/share/applications/"
#define GET_DESKTOP_EXEC_NAME_MAIN          "cat %s | awk '{if($1~\"Exec=\")if($2~\"\%\"){print $1} else print}' | cut -d '=' -f 2"
#define GET_DESKTOP_EXEC_NAME_BACK          "cat %s | awk '{if($1~\"StartupWMClass=\")print $1}' | cut -d '=' -f 2"
#define GET_DESKTOP_ICON                    "cat %s | awk '{if($1~\"Icon=\")print $1}' | cut -d '=' -f 2"
#define GET_PROCESS_EXEC_NAME_MAIN          "ps -aux | sed 's/ \\+/ /g' |awk '{if($2~\"%d\")print}'| cut -d ' ' -f 11-"

#define USR_SHARE_APP_CURRENT   "/usr/share/applications/."
#define USR_SHARE_APP_UPER      "/usr/share/applications/.."
#define PEONY_TRASH             "/usr/share/applications/peony-trash.desktop"
#define PEONY_COMUTER           "/usr/share/applications/peony-computer.desktop"
#define PEONY_HOME              "/usr/share/applications/peony-home.desktop"
#define PEONY_MAIN              "/usr/share/applications/peony.desktop"

#define WAYLAND_GROUP_HIDE     0
#define WAYLAND_GROUP_ACTIVATE 1
#define WAYLAND_GROUP_CLOSE    2

class QVBoxLayout;
class IUKUIPanelPlugin;

class UKUIGroupPopup;
class UKUiMasterPopup;

class UKUITaskGroup: public UKUITaskButton
{
    Q_OBJECT

public:
    UKUITaskGroup(const QString & groupName, WId window, UKUITaskBar * parent);
    UKUITaskGroup(const QString & iconName, const QString & caption, WId window, UKUITaskBar *parent = 0);
    virtual ~UKUITaskGroup();
    QString groupName() const { return mGroupName; }

    int buttonsCount() const;
    int visibleButtonsCount() const;
    void initVisibleHash();

    QWidget * addWindow(WId id);
    QWidget * checkedButton() const;
    QWidget * wl_addWindow(WId id);

    // Returns the next or the previous button in the popup
    // if circular is true, then it will go around the list of buttons
    QWidget * getNextPrevChildButton(bool next, bool circular);

    bool onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);
    void setAutoRotation(bool value, IUKUIPanel::Position position);
    Qt::ToolButtonStyle popupButtonStyle() const;
    void setToolButtonsStyle(Qt::ToolButtonStyle style);

    void setPopupVisible(bool visible = true, bool fast = false);

    void removeWidget();
    void removeSrollWidget();
    bool isSetMaxWindow();
    void showPreview();
    int calcAverageWidth();
    int calcAverageHeight();
    void showAllWindowByList();//when number of window is more than 30,need show all window of app by a list
    void showAllWindowByThumbnail();//when number of window is no more than 30,need show all window of app by a thumbnail
    void singleWindowClick();
    void VisibleWndRemoved(WId window);
    void setActivateState_wl(bool _state);
    void wl_widgetUpdateTitle(QString caption);

public slots:
    void onWindowRemoved(WId window);
    void timeout();

protected:
    QMimeData * mimeData();

    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void dragEnterEvent(QDragEnterEvent * event);
    void dragLeaveEvent(QDragLeaveEvent * event);
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
//    void paintEvent(QPaintEvent *);
    int recalculateFrameHeight() const;
    int recalculateFrameWidth() const;
    void setLayOutForPostion();

    void draggingTimerTimeout();

private slots:
    void onClicked(bool checked);
    void onChildButtonClicked();
    void onActiveWindowChanged(WId window);
    void onDesktopChanged(int number);

    void closeGroup();
    void refreshIconsGeometry();
    void refreshVisibility();
    void groupPopupShown(UKUITaskGroup* sender);
    void handleSavedEvent();

signals:
    void groupBecomeEmpty(QString name);
    void visibilityChanged(bool visible);
    void popupShown(UKUITaskGroup* sender);

private:
    bool isWaylandGroup;
    void changeTaskButtonStyle();
    QString mGroupName;
    UKUIGroupPopup * mPopup;
    QVBoxLayout *VLayout;
    UKUITaskButtonHash mButtonHash;
    UKUITaskButtonHash mVisibleHash;
    bool mPreventPopup;
    bool mSingleButton; //!< flag if this group should act as a "standard" button (no groupping or only one "shown" window in group)
    enum TaskGroupStatus{NORMAL, HOVER, PRESS};
    enum TaskGroupEvent{ENTEREVENT, LEAVEEVENT, OTHEREVENT};
    TaskGroupStatus taskgroupStatus;
    TaskGroupEvent  mTaskGroupEvent;
    QWidget *mpWidget;
    QScrollArea *mpScrollArea;
    QEvent * mEvent;
    QTimer *mTimer;
    QSize recalculateFrameSize();
    QPoint recalculateFramePosition();
    void recalculateFrameIfVisible();
    void adjustPopWindowSize(int width, int height);
    void v_adjustPopWindowSize(int width, int height, int v_all);
    void regroup();


    QString file_name;
    QuickLaunchAction *mAct;
    void initActionsInRightButtonMenu();
    void initDesktopFileName(WId window);
    void badBackFunctionToFindDesktop();

    void winClickActivate_wl(bool _getActive);
    void closeGroup_wl();
};

#endif // UKUITASKGROUP_H
