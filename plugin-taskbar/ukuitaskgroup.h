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
#include <QAbstractButton>
//#include <Xlib.h>

#define PREVIEW_WIDTH		468
#define PREVIEW_HEIGHT		428
#define SPACE_WIDTH			8
#define SPACE_HEIGHT		8
#define THUMBNAIL_WIDTH		(PREVIEW_WIDTH - SPACE_WIDTH)
#define THUMBNAIL_HEIGHT	(PREVIEW_HEIGHT - SPACE_HEIGHT)
#define ICON_WIDTH			48
#define ICON_HEIGHT			48
#define MAX_SIZE_OF_Thumb   16777215

#define SCREEN_MAX_WIDTH_SIZE     1400
#define SCREEN_MAX_HEIGHT_SIZE    1050

#define SCREEN_MIN_WIDTH_SIZE    800
#define SCREEN_MIN_HEIGHT_SIZE   600

#define SCREEN_MID_WIDTH_SIZE    1600

#define PREVIEW_WIDGET_MAX_WIDTH            352
#define PREVIEW_WIDGET_MAX_HEIGHT           264

#define PREVIEW_WIDGET_MIN_WIDTH            276
#define PREVIEW_WIDGET_MIN_HEIGHT           200
#define DEKSTOP_FILE_PATH "/usr/share/applications/"
#define GET_DESKTOP_EXEC_NAME_MAIN "cat %s | awk '{if($1~\"Exec=\")if($2~\"\%\"){print $1} else print}' | cut -d '=' -f 2"
#define GET_DESKTOP_EXEC_NAME_BACK "cat %s | awk '{if($1~\"StartupWMClass=\")print $1}' | cut -d '=' -f 2"
#define GET_DESKTOP_ICON "cat %s | awk '{if($1~\"Icon=\")print $1}' | cut -d '=' -f 2"
#define GET_PROCESS_EXEC_NAME_MAIN "ps -aux | sed 's/ \\+/ /g' |awk '{if($2~\"%d\")print}'| cut -d ' ' -f 11-"


class QVBoxLayout;
class IUKUIPanelPlugin;

class UKUIGroupPopup;
class UKUiMasterPopup;

class UKUITaskGroup: public UKUITaskButton
{
    Q_OBJECT

public:
    bool statFlag = true;
    bool existSameQckBtn = false;
    int QckBtnIndex = -1;
    bool hasbeenSaved = false;
    UKUITaskGroup(const QString & groupName, WId window, UKUITaskBar * parent);
    UKUITaskGroup(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, UKUITaskBar *parent);
    virtual ~UKUITaskGroup();
    QString groupName() const { return mGroupName; }

    int buttonsCount() const;
    int visibleButtonsCount() const;
    void initVisibleHash();

    QWidget * addWindow(WId id);
    QWidget * checkedButton() const;

    // Returns the next or the previous button in the popup
    // if circular is true, then it will go around the list of buttons
    QWidget * getNextPrevChildButton(bool next, bool circular);

    bool onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);
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
    void setAutoRotation(bool value, IUKUIPanel::Position position);
    void setQckLchBtn(UKUITaskGroup *utgp) { if(statFlag) mpQckLchBtn = utgp; }
    UKUITaskGroup* getQckLchBtn() { return mpQckLchBtn; }

public slots:
    void onWindowRemoved(WId window);
    void timeout();
    void toDothis_customContextMenuRequested(const QPoint &pos);

protected:
    QMimeData * mimeData();

    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void dragEnterEvent(QDragEnterEvent * event);
    void dragLeaveEvent(QDragLeaveEvent * event);
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dropEvent(QDropEvent *event);
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
    void AddtoTaskBar();
    void RemovefromTaskBar();

signals:
    void groupBecomeEmpty(QString name);
    void groupHidden(QString name);
    void groupVisible(QString name, bool will);
    void visibilityChanged(bool visible);
    void popupShown(UKUITaskGroup* sender);
    void t_saveSettings();
    void WindowAddtoTaskBar(QString arg);
    void WindowRemovefromTaskBar(QString arg);

private:
    //bool isDesktopFile(QString urlName);
    UKUITaskBar * mParent;
    UKUITaskGroup *mpQckLchBtn;
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
    QString isComputerOrTrash(QString urlName);
    void initDesktopFileName(WId window);
    void initActionsInRightButtonMenu();
    void badBackFunctionToFindDesktop();
    void setBackIcon();

    ///////////////////////////////
    // quicklaunch button
    QuickLaunchAction *mAct;
    IUKUIPanelPlugin * mPlugin;
    QAction *mDeleteAct;
    QuicklaunchMenu *mMenu;
    QPoint mDragStart;
    TaskGroupStatus quicklanuchstatus;
    CustomStyle toolbuttonstyle;
    QGSettings *mgsettings;

};

#endif // UKUITASKGROUP_H
