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

#define DESKTOP_FILE_PATH          "/usr/share/applications/"
#define ANDROID_DESKTOP_FILE_PATH  "/.local/share/applications/"

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

    Qt::ToolButtonStyle buttonStyle() const { return m_buttonStyle; }
    int buttonWidth() const { return m_buttonWidth; }
    bool closeOnMiddleClick() const { return m_closeOnMiddleClick; }
    bool raiseOnCurrentDesktop() const { return m_raiseOnCurrentDesktop; }
    bool isShowOnlyOneDesktopTasks() const { return m_showOnlyOneDesktopTasks; }
    int showDesktopNum() const { return m_showDesktopNum; }
    bool getCpuInfoFlg() const { return m_cpuInfoFlg; }
    bool isShowOnlyCurrentScreenTasks() const { return m_showOnlyCurrentScreenTasks; }
    bool isShowOnlyMinimizedTasks() const { return m_showOnlyMinimizedTasks; }
    bool isAutoRotate() const { return m_autoRotate; }
    bool isGroupingEnabled() const { return m_groupingEnabled; }
    bool isShowGroupOnHover() const { return m_showGroupOnHover; }
    bool isIconByClass() const { return m_iconByClass; }
    void setShowGroupOnHover(bool bFlag);
    inline IUKUIPanel * panel() const { return m_plugin->panel(); }
    inline IUKUIPanelPlugin * plugin() const { return m_plugin; }
    inline UKUITaskBarIcon* fetchIcon()const{return m_taskbarIcon;}
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
    QStringList m_ignoreWindow;


signals:
    void buttonRotationRefreshed(bool autoRotate, IUKUIPanel::Position position);
    void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
    void refreshIconGeometry();
    void showOnlySettingChanged();
    void iconByClassChanged();
    void popupShown(UKUITaskGroup* sender);
    void sendToUkuiDEApp(void);
//quicklaunch
    void setSizeOfTaskbarButton(int _size);

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

    void addToTaskbar(QString arg);

    void wlKwinSigHandler(quint32 wl_winId, int opNo, QString wl_iconName, QString wl_caption);

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
    TaskStatus m_taskStatus;

    ////////////////////////////////////
    /// quicklaunch parameter

    QVector<UKUITaskGroup*> m_vBtn;
    QGSettings *m_settings;

    QWidget *m_tmpWidget;

private:
    QMap<WId, UKUITaskGroup*> m_knownWindows; //!< Ids of known windows (mapping to buttons/groups)
    QList <WId> m_swid;
    UKUi::GridLayout *m_layout;
//    QList<GlobalKeyShortcut::Action*> mKeys;
    QSignalMapper *m_signalMapper;

    // Settings
    Qt::ToolButtonStyle m_buttonStyle;
    int m_buttonWidth;
    int m_buttonHeight;

    bool m_cpuInfoFlg = true;
    bool m_closeOnMiddleClick;
    bool m_raiseOnCurrentDesktop;
    bool m_showOnlyOneDesktopTasks;
    int m_showDesktopNum;
    bool m_showOnlyCurrentScreenTasks;
    bool m_showOnlyMinimizedTasks;
    bool m_autoRotate;
    bool m_groupingEnabled;
    bool m_showGroupOnHover;
    bool m_iconByClass;
    bool m_cycleOnWheelScroll; //!< flag for processing the wheelEvent

    bool acceptWindow(WId window) const;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);
    void settingsChanged();
    QList<QMap<QString, QVariant> > copyQuicklaunchConfig();

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);
    void directoryUpdated(const QString &path);

    QFileSystemWatcher *m_fsWatcher;
    QMap<QString, QStringList> m_currentContentsMap; // 当前每个监控的内容目录列表
    QString m_desfktopFilePath =DESKTOP_FILE_PATH;
    QString m_androidDesktopFilePath =QDir::homePath()+ANDROID_DESKTOP_FILE_PATH;

    IUKUIPanelPlugin *m_plugin;
    LeftAlignedTextStyle *m_style;
    UKUITaskBarIcon *m_taskbarIcon;
    QWidget *m_allFrame;
    QWidget *m_placeHolder;
    QGSettings *m_changeTheme;
    QHash<QString,QString> m_androidIconHash;

    QHash<QString,QString> matchAndroidIcon();
    QString captionExchange(QString str);
    void addWindow_wl(QString iconName, QString caption, WId window);

public slots:
    void WindowAddtoTaskBar(QString arg);
    void WindowRemovefromTaskBar(QString arg);

};

#endif // UKUITASKBAR_H
