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
#include <QHash>
#include <QString>
#include <QVector>
#include "qlayout.h"
#include "qlayoutitem.h"
#include "qlayoutitem.h"
#include "qgridlayout.h"
#include <QFileSystemWatcher>
#include <QtCore/QObject>
#include <QPushButton>
#include <QToolButton>
#include <QScrollArea>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

class XdgDesktopFile;
class QuickLaunchAction;
class QSettings;
class QLabel;
class QSignalMapper;
class UKUITaskButton;
class ElidedButtonStyle;
class UKUITaskBarIcon;

namespace UKUi {
class GridLayout;
}

class UKUITaskBar : public QScrollArea
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

    void realign();

    Qt::ToolButtonStyle buttonStyle() const { return mButtonStyle; }
    int buttonWidth() const { return mButtonWidth; }
    bool closeOnMiddleClick() const { return mCloseOnMiddleClick; }
    bool raiseOnCurrentDesktop() const { return mRaiseOnCurrentDesktop; }
    bool isShowOnlyOneDesktopTasks() const { return mShowOnlyOneDesktopTasks; }
    int showDesktopNum() const { return mShowDesktopNum; }
    bool getCpuInfoFlg() const { return CpuInfoFlg; }
    bool isShowOnlyCurrentScreenTasks() const { return mShowOnlyCurrentScreenTasks; }
    bool isShowOnlyMinimizedTasks() const { return mShowOnlyMinimizedTasks; }
    bool isAutoRotate() const { return mAutoRotate; }
    bool isGroupingEnabled() const { return mGroupingEnabled; }
    bool isShowGroupOnHover() const { return mShowGroupOnHover; }
    bool isIconByClass() const { return mIconByClass; }
    void setShowGroupOnHover(bool bFlag);
    inline IUKUIPanel * panel() const { return mPlugin->panel(); }
    inline IUKUIPanelPlugin * plugin() const { return mPlugin; }
    inline UKUITaskBarIcon* fetchIcon()const{return mpTaskBarIcon;}
    void pubAddButton(QuickLaunchAction* action) { addButton(action); }
    void pubSaveSettings() { saveSettings(); }


    ////////////////////////////////////////////////
    /// \brief quicklaunch func
    ///

    int indexOfButton(UKUITaskGroup *button) const;
    int countOfButtons() const;
    //virtual QLayoutItem *takeAt(int index) = 0;
    void saveSettings();
    void refreshQuickLaunch();
    friend class FilectrlAdaptor;
    QStringList mIgnoreWindow;


signals:
    void buttonRotationRefreshed(bool autoRotate, IUKUIPanel::Position position);
    void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
    void refreshIconGeometry();
    void showOnlySettingChanged();
    void iconByClassChanged();
    void popupShown(UKUITaskGroup* sender);
    void sendToUkuiDEApp(void);
//quicklaunch
    void setsizeoftaskbarbutton(int _size);

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void dropEvent(QDropEvent *e);

private slots:

    void refreshTaskList();
    void refreshButtonRotation();
    void refreshPlaceholderVisibility();
    void groupBecomeEmptySlot();
    void saveSettingsSlot();
    void onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);
    void onWindowAdded(WId window);
    void onWindowRemoved(WId window);
    void activateTask(int pos);
    void DosaveSettings() { saveSettings(); }
    void onDesktopChanged();

    ////////////////////////////
    /// quicklaunch slots
    ///
    bool checkButton(QuickLaunchAction* action);
    void removeButton(QuickLaunchAction* action);
    void removeButton(QString exec);
    void buttonDeleted();
    void removeFromTaskbar(QString arg);
    void switchButtons(UKUITaskGroup *dst_button, UKUITaskGroup *src_button);
    QString readFile(const QString &filename);

    void _AddToTaskbar(QString arg);

    void wl_kwinSigHandler(quint32 wl_winId, int opNo, QString wl_iconName, QString wl_caption);

    inline bool isFileExit(const QString &filename);

private:
    typedef QMap<WId, UKUITaskGroup*> windowMap_t;

private:
    void addWindow(WId window);
    void addButton(QuickLaunchAction* action);
    windowMap_t::iterator removeWindow(windowMap_t::iterator pos);
    void buttonMove(UKUITaskGroup * dst, UKUITaskGroup * src, QPoint const & pos);
    void doInitGroupButton(QString sname);
    void initRelationship();


    enum TaskStatus{NORMAL, HOVER, PRESS};
    TaskStatus taskstatus;

    ////////////////////////////////////
    /// quicklaunch parameter

    QVector<UKUITaskGroup*> mVBtn;
    QGSettings *settings;

    QToolButton *pageup;
    QToolButton *pagedown;
    QWidget *tmpwidget;
    QVector <UKUITaskGroup*> mBtnAll;
    QVector <int> mBtncvd;

private:
    QMap<WId, UKUITaskGroup*> mKnownWindows; //!< Ids of known windows (mapping to buttons/groups)
    QList <WId> swid;
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
    QList<QMap<QString, QVariant> > copyQuicklaunchConfig();

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);

    IUKUIPanelPlugin *mPlugin;
    LeftAlignedTextStyle *mStyle;
    UKUITaskBarIcon *mpTaskBarIcon;
    QWidget *mAllFrame;
    QWidget *mPlaceHolder;

    QGSettings *changeTheme;
    QHash<QString,QString> mAndroidIconHash;

    QHash<QString,QString> matchAndroidIcon();
    QString captionExchange(QString str);
    void addWindow_wl(QString iconName, QString caption, WId window);

public slots:
    void WindowAddtoTaskBar(QString arg);
    void WindowRemovefromTaskBar(QString arg);

};

#endif // UKUITASKBAR_H
