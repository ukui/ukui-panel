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

#include <QApplication>
#include <QDebug>
#include <QSignalMapper>
#include <QToolButton>
#include <QSettings>
#include <QList>
#include <QMimeData>
#include <QWheelEvent>
#include <QFlag>
#include <QtX11Extras/QX11Info>
#include <QDebug>
#include <QTimer>
#include <XdgDesktopFile>
#include <XdgIcon>
#include <QMessageBox>
#include <QPainterPath>
#include <unistd.h>
#include "../panel/common/ukuigridlayout.h"

#include "ukuitaskbar.h"
#include "ukuitaskgroup.h"
#include "quicklaunchaction.h"
#define PANEL_SETTINGS     "org.ukui.panel.settings"
#define PANEL_LINES        "panellines"
#define PANEL_CONFIG_PATH  "/usr/share/ukui/ukui-panel/panel-commission.ini"
#define PANEL_POSITION_KEY  "panelposition"
using namespace UKUi;
/************************************************

************************************************/
UKUITaskBar::UKUITaskBar(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QScrollArea(parent),
    m_signalMapper(new QSignalMapper(this)),
    m_buttonStyle(Qt::ToolButtonIconOnly),
    m_buttonWidth(400),
    m_buttonHeight(100),
    m_closeOnMiddleClick(true),
    m_raiseOnCurrentDesktop(true),
    m_showOnlyOneDesktopTasks(true),
    m_showDesktopNum(0),
    m_showOnlyCurrentScreenTasks(false),
    m_showOnlyMinimizedTasks(false),
    m_autoRotate(true),
    m_groupingEnabled(true),
    m_showGroupOnHover(true),
    m_iconByClass(false),
    m_cycleOnWheelScroll(true),
    m_plugin(plugin),
    m_ignoreWindow(),
    m_placeHolder(new QWidget(this)),
    m_style(new LeftAlignedTextStyle())
{
    m_allFrame=new QWidget(this);
    m_allFrame->setAttribute(Qt::WA_TranslucentBackground);

    this->setWidget(m_allFrame);
    this->horizontalScrollBar()->setVisible(false);
    this->verticalScrollBar()->setVisible(false);
    this->setFrameShape(QFrame::NoFrame);//去掉边框
    this->setWidgetResizable(true);
    //临时方案解决任务栏出现滚动时有滑动条区域遮挡图标,待滚动提示样式确认后再进行替换
    horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, QColor(Qt::transparent));
    this->setPalette(pal);

    m_taskStatus=NORMAL;

    m_layout = new UKUi::GridLayout(m_allFrame);
    m_allFrame->setLayout(m_layout);
    m_layout->setMargin(0);
    m_layout->setStretch(UKUi::GridLayout::StretchHorizontal | UKUi::GridLayout::StretchVertical);

    m_placeHolder->setMinimumSize(1,1);
    m_placeHolder->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    m_placeHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    QString filename = QString::fromLocal8Bit(PANEL_CONFIG_PATH);
    QSettings settings(filename, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("IgnoreWindow");
    m_ignoreWindow = settings.value("ignoreWindow", "").toStringList();
    settings.endGroup();

    //往任务栏中加入快速启动按钮
    refreshQuickLaunch();
    realign();

    settingsChanged();
    setAcceptDrops(true);

    const QByteArray id(PANEL_SETTINGS);
    if (QGSettings::isSchemaInstalled(id)) {
        m_settings=new QGSettings(id);
    }

    m_androidIconHash=matchAndroidIcon();

    const QByteArray id_Theme("org.ukui.style");
    if(QGSettings::isSchemaInstalled(id_Theme)){
        m_changeTheme = new QGSettings(id_Theme);
    }
    connect(m_changeTheme, &QGSettings::changed, this, [=] (const QString &key){
        if(key=="iconThemeName"){
            sleep(1);
            for(auto it= m_knownWindows.begin(); it != m_knownWindows.end();it++)
            {
                UKUITaskGroup *group = it.value();
                group->updateIcon();
            }
        }
        if(key == PANEL_POSITION_KEY) {
            realign();
        }
    });

    connect(m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &UKUITaskBar::activateTask);

    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &UKUITaskBar::onWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &UKUITaskBar::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &UKUITaskBar::onWindowRemoved);
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(onDesktopChanged()));

    saveSettings();

    /**/
    QDBusConnection::sessionBus().unregisterService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerObject("/taskbar/click", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/quicklaunch"), "org.ukui.panel.taskbar", "AddToTaskbar", this, SLOT(addToTaskbar(QString)));
    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/quicklaunch"), "org.ukui.panel.taskbar", "RemoveFromTaskbar", this, SLOT(removeFromTaskbar(QString)));

    /*监听系统应用的目录以及安卓兼容应用的目录*/
    m_fsWatcher=new QFileSystemWatcher(this);
    m_fsWatcher->addPath(m_desfktopFilePath);
    m_fsWatcher->addPath(m_androidDesktopFilePath);
    directoryUpdated(m_desfktopFilePath);
    directoryUpdated(m_androidDesktopFilePath);
    connect(m_fsWatcher,&QFileSystemWatcher::directoryChanged,[this](){
        directoryUpdated(m_desfktopFilePath);
        directoryUpdated(m_androidDesktopFilePath);
    });

    if (m_layout->count() == 0) {
        m_layout->addWidget(m_placeHolder);
    } else {
        m_placeHolder->setFixedSize(0,0);
    }

    QDBusConnection::sessionBus().connect(QString(), QString("/"), "com.ukui.panel", "event", this, SLOT(wlKwinSigHandler(quint32,int, QString, QString)));
}

/************************************************

 ************************************************/
UKUITaskBar::~UKUITaskBar()
{
    for(auto it = m_vBtn.begin(); it != m_vBtn.end();)
    {
        (*it)->deleteLater();
        m_vBtn.erase(it);
    }
    m_vBtn.clear();
}

QString UKUITaskBar::readFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

void UKUITaskBar::onDesktopChanged() {
    for (auto i = m_knownWindows.begin(); m_knownWindows.end() != i; ++i)
    {
        (*i)->onDesktopChanged();
        if ((*i)->m_existSameQckBtn) {
            UKUITaskGroup* btn = (*i)->getOwnQckBtn();
            if (m_vBtn.contains(btn))
                btn->setVisible((*i)->isHidden());
        }
    }
}

void UKUITaskBar::refreshQuickLaunch(){
    for(auto it = m_vBtn.begin(); it != m_vBtn.end();)
    {
        (*it)->deleteLater();
        m_vBtn.erase(it);
    }

    QString desktop;
    QString file;

    //gsetting的方式读取写入 apps
    QList<QMap<QString, QVariant> > apps = m_plugin->settings()->readArray("apps");
    QString filename = QDir::homePath() + "/.config/ukui/panel.conf";
    QSettings user_qsettings(filename,QSettings::IniFormat);
    QStringList groupname = user_qsettings.childGroups();
    //为了兼容3.0版本和3.1版本，后续版本考虑删除
    if (apps.isEmpty() && groupname.contains("quicklaunch")) {
        apps = copyQuicklaunchConfig();
    } else if (groupname.contains("quicklaunch")) {
            user_qsettings.remove("quicklaunch");
    }

    for (const QMap<QString, QVariant> &app : apps)
    {
        desktop = app.value("desktop", "").toString();
        qDebug()<<"desktop  ******"<<desktop;

        file = app.value("file", "").toString();
        if (!desktop.isEmpty())
        {
            XdgDesktopFile xdg;
            if (xdg.load(desktop))
                addButton(new QuickLaunchAction(&xdg, this));
        }else{
            qDebug()<<"error desktop file";
        }
    }
}

QList<QMap<QString, QVariant> > UKUITaskBar::copyQuicklaunchConfig()
{
    QString filename = QDir::homePath() + "/.config/ukui/panel.conf";
    //若taskbar中没有apps，则把quicklaunch中的内容复制到taskbar
    qDebug()<<"Taskbar is empty, read apps from quicklaunch";
    QSettings user_qsettings(filename,QSettings::IniFormat);
    user_qsettings.beginGroup("quicklaunch");
    QList<QMap<QString, QVariant> > array;
    int size = user_qsettings.beginReadArray("apps");
    for (int i = 0; i < size; ++i)
    {
        user_qsettings.setArrayIndex(i);
        QMap<QString, QVariant> map;
        map["desktop"] = user_qsettings.value("desktop");
        if (array.contains(map)) {
            continue;
        } else {
            array << map;
        }
    }
    user_qsettings.endArray();
    user_qsettings.endGroup();
    user_qsettings.remove("quicklaunch");
    user_qsettings.sync();
    return array;
}

bool UKUITaskBar::acceptWindow(WId window) const
{
    QFlags<NET::WindowTypeMask> ignoreList;
    ignoreList |= NET::DesktopMask;
    ignoreList |= NET::DockMask;
    ignoreList |= NET::SplashMask;
    ignoreList |= NET::ToolbarMask;
    ignoreList |= NET::MenuMask;
    ignoreList |= NET::PopupMenuMask;
    ignoreList |= NET::NotificationMask;

    KWindowInfo info(window, NET::WMWindowType | NET::WMState, NET::WM2TransientFor);
    if (!info.valid())
        return false;

    if (NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
        return false;

    if (info.state() & NET::SkipTaskbar)
        return false;

    // WM_TRANSIENT_FOR hint not set - normal window
    WId transFor = info.transientFor();
    if (transFor == 0 || transFor == window || transFor == (WId) QX11Info::appRootWindow())
        return true;

    info = KWindowInfo(transFor, NET::WMWindowType);

    QFlags<NET::WindowTypeMask> normalFlag;
    normalFlag |= NET::NormalMask;
    normalFlag |= NET::DialogMask;
    normalFlag |= NET::UtilityMask;

    return !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), normalFlag);
}

void UKUITaskBar::dragEnterEvent(QDragEnterEvent* event)
{
}

void UKUITaskBar::dragMoveEvent(QDragMoveEvent * event)
{
}

void UKUITaskBar::dropEvent(QDropEvent *e)
{
}

void UKUITaskBar::buttonMove(UKUITaskGroup * dst, UKUITaskGroup * src, QPoint const & pos)
{
    int src_index;
    if (!src || -1 == (src_index = m_layout->indexOf(src)))
    {
        return;
    }

    const int size = m_layout->count();
    Q_ASSERT(0 < size);
    //dst is nullptr in case the drop occured on empty space in taskbar
    int dst_index;
    if (nullptr == dst)
    {
        //moving based on taskbar (not signaled by button)
        QRect occupied = m_layout->occupiedGeometry();
        QRect last_empty_row{occupied};
        const QRect last_item_geometry = m_layout->itemAt(size - 1)->geometry();
        if (m_plugin->panel()->isHorizontal())
        {
            if (isRightToLeft())
            {
                last_empty_row.setTopRight(last_item_geometry.topLeft());
            } else
            {
                last_empty_row.setTopLeft(last_item_geometry.topRight());
            }
        } else
        {
            if (isRightToLeft())
            {
                last_empty_row.setTopRight(last_item_geometry.topRight());
            } else
            {
                last_empty_row.setTopLeft(last_item_geometry.topLeft());
            }
        }
        if (occupied.contains(pos) && !last_empty_row.contains(pos)) {
            return;
        }

        dst_index = size;
    } else
    {
        //moving based on signal from child button
        dst_index = m_layout->indexOf(dst);
    }

    //moving lower index to higher one => consider as the QList::move => insert(to, takeAt(from))
    if (src_index < dst_index)
    {
        if (size == dst_index
                || src_index + 1 != dst_index)
        {
            --dst_index;
        } else
        {
            //switching positions of next standing
            const int tmp_index = src_index;
            src_index = dst_index;
            dst_index = tmp_index;
        }
    }

    if (dst_index == src_index
            || m_layout->animatedMoveInProgress()
       ) {
        return;
    }

    m_layout->moveItem(src_index, dst_index, true);
}

void UKUITaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    UKUITaskGroup * const group = qobject_cast<UKUITaskGroup*>(sender());
    Q_ASSERT(group);

    for (auto i = m_knownWindows.begin(); m_knownWindows.end() != i; )
    {
        if (group == *i) {
            m_swid.removeOne(i.key());
            i = m_knownWindows.erase(i);
            break;
        }
        else
            ++i;
    }
    for (auto it = m_vBtn.begin(); it!=m_vBtn.end(); ++it)
    {
        UKUITaskGroup *pQuickBtn = *it;
        if(pQuickBtn->m_fileName == group->m_fileName
           &&(m_layout->indexOf(pQuickBtn) >= 0 ))
        {
            pQuickBtn->setHidden(false);
            m_layout->moveItem(m_layout->indexOf(pQuickBtn), m_layout->indexOf(group));
            pQuickBtn->m_existSameQckBtn = false;
            break;
        }
    }
    m_layout->removeWidget(group);
    group->deleteLater();
}

void UKUITaskBar::addWindow(WId window)
{
    // If grouping disabled group behaves like regular button
    const QString group_id = m_groupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
    if (m_ignoreWindow.contains(group_id)) {
        return;
    }

    UKUITaskGroup *group = nullptr;
    bool isNeedAddNewWidget = true;
    auto i_group = m_knownWindows.find(window);
    if (m_knownWindows.end() != i_group)
    {
        if ((*i_group)->groupName() == group_id)
            group = *i_group;
        else
            (*i_group)->onWindowRemoved(window);
    }

    /*check if window belongs to some existing group
     * 安卓兼容应用的组名为kydroid-display-window
     * 需要将安卓兼容目录的分组特性关闭
    */

    QStringList andriod_window_list;
    andriod_window_list<<"kydroid-display-window"<<"kylin-kmre-window"<<"";
    if (!group && m_groupingEnabled && !andriod_window_list.contains(group_id))
    {
        for (auto i = m_knownWindows.cbegin(), i_e = m_knownWindows.cend(); i != i_e; ++i)
        {
            if ((*i)->groupName() == group_id)
            {
                group = *i;
                break;
            }
        }
    }
    if (!group)
    {
        group = new UKUITaskGroup(group_id, window, this);
        connect(group, SIGNAL(groupBecomeEmpty(QString)), this, SLOT(groupBecomeEmptySlot()));
        connect(group, SIGNAL(t_saveSettings()), this, SLOT(saveSettingsSlot()));
        connect(group, SIGNAL(WindowAddtoTaskBar(QString)), this, SLOT(WindowAddtoTaskBar(QString)));
        connect(group, SIGNAL(WindowRemovefromTaskBar(QString)), this, SLOT(WindowRemovefromTaskBar(QString)));
        connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &UKUITaskGroup::popupShown, this, &UKUITaskBar::popupShown);
        connect(group, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
            switchButtons(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource));//, pos);
        });
        for (auto it = m_vBtn.begin(); it!=m_vBtn.end(); ++it)
        {
            UKUITaskGroup *pQuickBtn = *it;
            if(pQuickBtn->m_fileName == group->m_fileName
               &&(m_layout->indexOf(pQuickBtn) >= 0 ))
            {
                m_layout->addWidget(group);
                m_layout->moveItem(m_layout->indexOf(group), m_layout->indexOf(pQuickBtn));
                pQuickBtn->setHidden(true);
                isNeedAddNewWidget = false;
                group->m_existSameQckBtn = true;
                pQuickBtn->m_existSameQckBtn = true;
                group->setQckLchBtn(pQuickBtn);
                break;
            }
        }
        if(isNeedAddNewWidget)
        {
            m_layout->addWidget(group);
        }
        group->setToolButtonsStyle(m_buttonStyle);
    }
    m_knownWindows[window] = group;
    m_swid.push_back(window);

    group->addWindow(window);
    group->groupName();
    group->updateIcon();
}

auto UKUITaskBar::removeWindow(windowMap_t::iterator pos) -> windowMap_t::iterator
{
    WId const window = pos.key();
    UKUITaskGroup * const group = *pos;
    m_swid.removeOne(window);
    auto ret = m_knownWindows.erase(pos);
    group->onWindowRemoved(window);
    //if (countOfButtons() <= 32) m_tmpWidget->setHidden(true);
    return ret;
}

void UKUITaskBar::refreshTaskList()
{
    QList<WId> new_list;
    // Just add new windows to groups, deleting is up to the groups
    const auto wnds = KWindowSystem::stackingOrder();
    for (auto const wnd: wnds)
    {
        if (acceptWindow(wnd))
        {
            new_list << wnd;
            addWindow(wnd);
        }
    }
  //  m_layout->addWidget(m_tmpWidget);

    //emulate windowRemoved if known window not reported by KWindowSystem
    for (auto i = m_knownWindows.begin(), i_e = m_knownWindows.end(); i != i_e; )
    {
        if (0 > new_list.indexOf(i.key()))
        {
            i = removeWindow(i);
        } else
            ++i;
    }

    refreshPlaceholderVisibility();
}


void UKUITaskBar::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{

    auto i = m_knownWindows.find(window);
    if (m_knownWindows.end() != i)
    {
        if (!(*i)->onWindowChanged(window, prop, prop2) && acceptWindow(window))
        { // window is removed from a group because of class change, so we should add it again
            addWindow(window);

        }
    }
}

void UKUITaskBar::onWindowAdded(WId window)
{
    qDebug()<<"window    is :  ******"<<window;
    auto const pos = m_knownWindows.find(window);
    if (m_knownWindows.end() == pos && acceptWindow(window))
        addWindow(window);
}

void UKUITaskBar::onWindowRemoved(WId window)
{
    auto const pos = m_knownWindows.find(window);
    if (m_knownWindows.end() != pos)
    {
        removeWindow(pos);
    }
}

void UKUITaskBar::refreshButtonRotation()
{
    bool autoRotate = m_autoRotate && (m_buttonStyle != Qt::ToolButtonIconOnly);

    IUKUIPanel::Position panelPosition = m_plugin->panel()->position();
   // emit buttonRotationRefreshed(autoRotate, panelPosition);
}

/************************************************

 ************************************************/
void UKUITaskBar::refreshPlaceholderVisibility()
{
    bool haveVisibleWindow = false;
    for (auto i = m_knownWindows.cbegin(), i_e = m_knownWindows.cend(); i_e != i; ++i)
    {
        if ((*i)->isVisible())
        {
            haveVisibleWindow = true;
            break;
        }
    }
    m_placeHolder->setVisible(!haveVisibleWindow);
    if (haveVisibleWindow || m_layout->count() != 0) {
        m_placeHolder->setFixedSize(0,0);
     } else {
        m_placeHolder->setMinimumSize(1,1);
        m_placeHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
}

/************************************************

 ************************************************/
void UKUITaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    emit buttonStyleRefreshed(m_buttonStyle);
}

void UKUITaskBar::settingsChanged()
{
    bool groupingEnabledOld = m_groupingEnabled;
    bool showOnlyOneDesktopTasksOld = m_showOnlyOneDesktopTasks;
    const int showDesktopNumOld = m_showDesktopNum;
    bool showOnlyCurrentScreenTasksOld = m_showOnlyCurrentScreenTasks;
    bool showOnlyMinimizedTasksOld = m_showOnlyMinimizedTasks;
    const bool iconByClassOld = m_iconByClass;

    m_buttonWidth = m_plugin->settings()->value("buttonWidth", 400).toInt();
    m_buttonHeight = m_plugin->settings()->value("buttonHeight", 100).toInt();
    QString s = m_plugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
        setButtonStyle(Qt::ToolButtonIconOnly);
    else if (s == "TEXT")
        setButtonStyle(Qt::ToolButtonTextOnly);
    else
        setButtonStyle(Qt::ToolButtonIconOnly);

    m_showOnlyOneDesktopTasks = m_plugin->settings()->value("showOnlyOneDesktopTasks", m_showOnlyOneDesktopTasks).toBool();
    m_showDesktopNum = m_plugin->settings()->value("showDesktopNum", m_showDesktopNum).toInt();
    m_showOnlyCurrentScreenTasks = m_plugin->settings()->value("showOnlyCurrentScreenTasks", m_showOnlyCurrentScreenTasks).toBool();
    m_showOnlyMinimizedTasks = m_plugin->settings()->value("showOnlyMinimizedTasks", m_showOnlyMinimizedTasks).toBool();
    m_autoRotate = m_plugin->settings()->value("autoRotate", true).toBool();
    m_closeOnMiddleClick = m_plugin->settings()->value("closeOnMiddleClick", true).toBool();
    m_raiseOnCurrentDesktop = m_plugin->settings()->value("raiseOnCurrentDesktop", false).toBool();
    m_groupingEnabled = m_plugin->settings()->value("groupingEnabled",true).toBool();
    m_showGroupOnHover = m_plugin->settings()->value("showGroupOnHover",true).toBool();
    m_iconByClass = m_plugin->settings()->value("iconByClass", false).toBool();
    m_cycleOnWheelScroll = m_plugin->settings()->value("cycleOnWheelScroll", true).toBool();

    // Delete all groups if grouping feature toggled and start over
    if (groupingEnabledOld != m_groupingEnabled)
    {
        for (int i = m_knownWindows.size() - 1; 0 <= i; --i)
        {
            UKUITaskGroup * group = m_knownWindows.value(m_swid.value(i));
            if (nullptr != group)
            {
                m_layout->takeAt(i);
                group->deleteLater();
            }
        }
        m_knownWindows.clear();
        m_swid.clear();
    }

    if (showOnlyOneDesktopTasksOld != m_showOnlyOneDesktopTasks
            || (m_showOnlyOneDesktopTasks && showDesktopNumOld != m_showDesktopNum)
            || showOnlyCurrentScreenTasksOld != m_showOnlyCurrentScreenTasks
            || showOnlyMinimizedTasksOld != m_showOnlyMinimizedTasks
            )
        emit showOnlySettingChanged();
    if (iconByClassOld != m_iconByClass)
        emit iconByClassChanged();

    refreshTaskList();
}

void UKUITaskBar::setShowGroupOnHover(bool bFlag)
{
    m_showGroupOnHover = bFlag;
}

int i = 0;
void UKUITaskBar::realign()
{
    m_layout->setEnabled(false);
    refreshButtonRotation();

    IUKUIPanel *panel = m_plugin->panel();
    QSize maxSize = QSize(m_plugin->panel()->panelSize(), m_plugin->panel()->panelSize());
    QSize minSize = QSize(m_plugin->panel()->iconSize()/2, m_plugin->panel()->iconSize()/2);
    int iconsize = panel->iconSize();

    bool rotated = false;
    if (panel->isHorizontal())
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_allFrame->setMinimumSize(QSize((m_layout->count()+5)*panel->panelSize(),panel->panelSize()));
        if (m_allFrame->width() < this->width()) {
            m_allFrame->setFixedWidth(this->width());
        }

        m_layout->setRowCount(panel->lineCount());
        m_layout->setColumnCount(0);
    }
    else
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_allFrame->setMinimumSize(QSize(panel->panelSize(),(m_layout->count()+3)*panel->panelSize()));
        if (m_allFrame->height() < this->height()) {
            m_allFrame->setFixedHeight(this->height());
        }

        m_layout->setRowCount(0);

        if (m_buttonStyle == Qt::ToolButtonIconOnly)
        {
            // Vertical + Icons
            m_layout->setColumnCount(panel->lineCount());
        }
        else
        {
            rotated = m_autoRotate && (panel->position() == IUKUIPanel::PositionLeft || panel->position() == IUKUIPanel::PositionRight);

            // Vertical + Text
            if (rotated)
            {
                maxSize.rwidth()  = m_buttonHeight;
                maxSize.rheight() = m_buttonWidth;

                m_layout->setColumnCount(panel->lineCount());
            }
            else
            {
                m_layout->setColumnCount(1);
            }
        }
    }

    for(auto it= m_knownWindows.begin(); it != m_knownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
        group->setIconSize(QSize(iconsize,iconsize));
    }
    for (int i = 0; i < m_vBtn.size(); i++) {
        UKUITaskGroup * quicklaunch = m_vBtn.value(i);
        quicklaunch->setIconSize(QSize(iconsize, iconsize));
    }
    m_layout->setCellMinimumSize(minSize);
    m_layout->setCellMaximumSize(maxSize);
    m_layout->setDirection(rotated ? UKUi::GridLayout::TopToBottom : UKUi::GridLayout::LeftToRight);
    m_layout->setEnabled(true);
    //our placement on screen could have been changed
    emit showOnlySettingChanged();
    emit refreshIconGeometry();

    horizontalScrollBar()->setMaximum(m_allFrame->width() - this->width());
    verticalScrollBar()->setMaximum(m_allFrame->height() - this->height());
}

void UKUITaskBar::wheelEvent(QWheelEvent* event)
{
    if (this->verticalScrollBarPolicy()==Qt::ScrollBarAlwaysOff) {
        if (event->delta()>=0) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()-40);
            qDebug()<<"-40-horizontalScrollBar()->value()"<<horizontalScrollBar()->value();
        } else {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()+40);
            if (horizontalScrollBar()->value()>m_allFrame->width()) {
                horizontalScrollBar()->setValue(m_allFrame->width());
            }
            qDebug()<<"+40+horizontalScrollBar()->value()"<<horizontalScrollBar()->value();
        }
    } else {
        if (event->delta()>=0) {
            verticalScrollBar()->setValue(verticalScrollBar()->value()-40);
        } else {
            verticalScrollBar()->setValue(verticalScrollBar()->value()+40);
        }
    }
}

void UKUITaskBar::resizeEvent(QResizeEvent* event)
{
    emit refreshIconGeometry();
    return QWidget::resizeEvent(event);
}

void UKUITaskBar::changeEvent(QEvent* event)
{
    // if current style is changed, reset the base style of the proxy style
    // so we can apply the new style correctly to task buttons.
    if(event->type() == QEvent::StyleChange)
        m_style->setBaseStyle(NULL);

    QFrame::changeEvent(event);
}


void UKUITaskBar::activateTask(int pos)
{
    for (int i = 0; i < m_knownWindows.size(); ++i)
    {
        UKUITaskGroup * g = m_knownWindows.value(m_swid.value(i));
        if (g && g->isVisible())
        {
            pos--;
            if (pos == 0)
            {
                g->raiseApplication();
                break;
            }
        }
    }
}

void UKUITaskBar::enterEvent(QEvent *)
{
    m_taskStatus=HOVER;
    update();
}

void UKUITaskBar::leaveEvent(QEvent *)
{
    m_taskStatus=NORMAL;
    update();
}

void UKUITaskBar::paintEvent(QPaintEvent *)
{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);

        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath rectPath;
        rectPath.addRoundedRect(this->rect(),6,6);
        // 画一个黑底
        QPixmap pixmap(this->rect().size());
        pixmap.fill(Qt::transparent);
        QPainter pixmapPainter(&pixmap);
        pixmapPainter.setRenderHint(QPainter::Antialiasing);

        pixmapPainter.drawPath(rectPath);
        pixmapPainter.end();

        // 模糊这个黑底
        extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
        QImage img = pixmap.toImage();
        qt_blurImage(img, 10, false, false);

        switch(m_taskStatus)
          {
          case NORMAL:
              {
                  p.setPen(Qt::NoPen);
                  break;
              }
          case HOVER:
              {
                  p.setPen(Qt::NoPen);
                  break;
              }
          case PRESS:
              {
                  p.setPen(Qt::NoPen);
                  break;
              }
          }

        QPalette pal = this->palette();
        pal.setBrush(QPalette::Base, QColor(0,0,0,0));        //背景透明
        this->viewport()->setPalette(pal);
        this->setPalette(pal);
        m_allFrame->setPalette(pal);

        p.setRenderHint(QPainter::Antialiasing);
        p.drawRoundedRect(opt.rect,6,6);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UKUITaskBar::mouseMoveEvent(QMouseEvent *e) {
    QFrame::mouseMoveEvent(e);
}


void UKUITaskBar::mousePressEvent(QMouseEvent *)
{
    QDBusMessage message =QDBusMessage::createSignal("/taskbar/click", "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp");
    QDBusConnection::sessionBus().send(message);
}

///////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/// About Quick Launch Functions
///
///

void UKUITaskBar::addButton(QuickLaunchAction* action)
{
    bool isNeedAddNewWidget = true;
    m_layout->setEnabled(false);
    UKUITaskGroup *btn = new UKUITaskGroup(action, m_plugin, this);
    btn->setArrowType(Qt::NoArrow);
    /*＠bug
     * 快速启动栏右键菜单原本的样式有对于不可选项有置灰效果，
     * 后跟随主题框架之后置灰效果消失，可能与此属性相关
     */
    //        btn->setMenu(Qt::InstantPopup);
    for (auto it = m_knownWindows.begin(); it!=m_knownWindows.end(); ++it)
    {
        UKUITaskGroup *group = *it;
        if(btn->m_fileName == group->m_fileName
           &&(m_layout->indexOf(group) >= 0))
        {
            m_layout->addWidget(btn);
            m_layout->moveItem(m_layout->indexOf(btn), m_layout->indexOf(group));
            isNeedAddNewWidget = false;
            group->m_existSameQckBtn = true;
            btn->m_existSameQckBtn = true;
            m_vBtn.push_back(btn);
            group->setQckLchBtn(btn);
            btn->setHidden(group->isVisible());
            break;
        }
    }
    if (isNeedAddNewWidget) {
        m_layout->addWidget(btn);
        btn->setIconSize(QSize(m_plugin->panel()->iconSize(),m_plugin->panel()->iconSize()));
        m_vBtn.push_back(btn);
        m_layout->moveItem(m_layout->indexOf(btn), countOfButtons() - 1);
    }
    connect(btn, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
        switchButtons(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource));//, pos);
    });
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(t_saveSettings()), this, SLOT(saveSettingsSlot()));
    m_layout->setEnabled(true);
}

void UKUITaskBar::switchButtons(UKUITaskGroup *dst_button, UKUITaskGroup *src_button)
{
    if (dst_button == src_button)
        return;
    if (!dst_button || !src_button)
        return;
    int dst = m_layout->indexOf(dst_button);
    int src = m_layout->indexOf(src_button);
    if (dst == src
            || m_layout->animatedMoveInProgress()
       ) {
        return;
    }
    m_layout->moveItem(src, dst, true);
    saveSettings();
}

bool UKUITaskBar::checkButton(QuickLaunchAction* action)
{
    bool checkresult;
    UKUITaskGroup* btn = new UKUITaskGroup(action, m_plugin, this);
    int i = 0;
    int counts = m_vBtn.size();
    if(m_vBtn.size()>0){
        while (i != counts) {
            UKUITaskGroup *b = m_vBtn.value(i);
            qDebug()<<"m_layout->itemAt("<<i<<") ";
            if(b->m_fileName == btn->m_fileName) {
                checkresult=true;
                break;
            } else {
                checkresult=false;
                ++i;
            }

        }
        delete btn;
        return checkresult;
    }
    else{
        qDebug()<<"countOfButtons =  "<<countOfButtons();
        delete btn;
        return false;
    }
}

/* 以xdg的方式删除　button*/
void UKUITaskBar::removeButton(QuickLaunchAction* action)
{
    int i = 0;
    UKUITaskGroup* btn = new UKUITaskGroup(action, m_plugin, this);
    if (!btn)
        return;
    while (i < m_vBtn.size()) {
        UKUITaskGroup *tmp = m_vBtn.value(i);
        if (QString::compare(btn->m_fileName, tmp->m_fileName) == 0) {
            doInitGroupButton(tmp->m_fileName);
            tmp->deleteLater();
            m_layout->removeWidget(tmp);
            m_vBtn.remove(i);
            break;
        }
        ++i;
    }

    btn->deleteLater();
    saveSettings();
}

void UKUITaskBar::removeButton(QString file)
{
    int i = 0;
    while (i < m_vBtn.size()) {
        UKUITaskGroup *tmp = m_vBtn.value(i);
        if (QString::compare(file, tmp->m_fileName) == 0) {
            doInitGroupButton(tmp->m_fileName);
            tmp->deleteLater();
            m_layout->removeWidget(tmp);
            m_vBtn.remove(i);
            break;
        }
        ++i;
    }
}

void UKUITaskBar::WindowAddtoTaskBar(QString arg) {
    for(auto it= m_knownWindows.begin(); it != m_knownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
            if (arg.compare(group->groupName()) == 0) {
                addToTaskbar(group->m_fileName);
                break;
        }
    }
}

void UKUITaskBar::WindowRemovefromTaskBar(QString arg) {
    for (auto it = m_vBtn.begin(); it!=m_vBtn.end(); ++it)
    {
        UKUITaskGroup *pQuickBtn = *it;
        if(pQuickBtn->m_fileName == arg
           && (m_layout->indexOf(pQuickBtn) >= 0 ))
        {
            doInitGroupButton(pQuickBtn->m_fileName);
            m_vBtn.removeOne(pQuickBtn);
            pQuickBtn->deleteLater();
            m_layout->removeWidget(pQuickBtn);
            saveSettings();
            break;
        }
    }
}

// 只要任何监控的目录更新（添加、删除、重命名），就会调用。
void UKUITaskBar::directoryUpdated(const QString &path)
{
    // 比较最新的内容和保存的内容找出区别(变化)
    QStringList currentrylist = m_currentContentsMap[path];
    const QDir dir(path);
    QStringList newentrylist = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    QSet<QString> newdirset = QSet<QString>::fromList(newentrylist);
    QSet<QString> currentdirset = QSet<QString>::fromList(currentrylist);

    // 添加了文件
    QSet<QString> newfiles = newdirset - currentdirset;
    QStringList newfile = newfiles.toList();

    // 文件已被移除
    QSet<QString> deletedfiles = currentdirset - newdirset;
    QStringList deletefile = deletedfiles.toList();

    // 更新当前设置
    m_currentContentsMap[path] = newentrylist;

    if (!newfile.isEmpty() && !deletefile.isEmpty()) {
        // 文件/目录重命名
        if ((newfile.count() == 1) && (deletefile.count() == 1)) {
//            qDebug() << QString("File Renamed from %1 to %2").arg(deleteFile.first()).arg(newFile.first());
        }
    } else {
        // 从Dir中删除文件/目录
        if (!deletefile.isEmpty()) {
            foreach(QString file, deletefile) {
                removeButton(path+file);
            }
        }
    }
}


void UKUITaskBar::addToTaskbar(QString arg) {
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    if (xdg.load(fileName)){
        if(!checkButton(new QuickLaunchAction(&xdg, this))){
            addButton(new QuickLaunchAction(&xdg, this));
            m_placeHolder->hide();
        }
    }else{
        qWarning() << "XdgDesktopFile" << fileName << "is not valid";
        QMessageBox::information(this, tr("Drop Error"),
                                 tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                                 );
    }
    saveSettings();
}

void UKUITaskBar::removeFromTaskbar(QString arg)
{
    XdgDesktopFile xdg;
    xdg.load(arg);
    removeButton(new QuickLaunchAction(&xdg, this));
}

void UKUITaskBar::doInitGroupButton(QString sname) {
    for(auto it= m_knownWindows.begin(); it != m_knownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
        if (group->m_existSameQckBtn) {
            if (sname == group->m_fileName) {
                    group->m_existSameQckBtn = false;
                    group->setQckLchBtn(NULL);
                    break;
            }
        }
    }
}

void UKUITaskBar::buttonDeleted()
{
    UKUITaskGroup *btn = qobject_cast<UKUITaskGroup*>(sender());
    if (!btn)
        return;
    for(auto it = m_vBtn.begin();it != m_vBtn.end();it++)
    {
        if(*it == btn)
        {
            for(auto it= m_knownWindows.begin(); it != m_knownWindows.end();it++)
            {
                UKUITaskGroup *group = it.value();
                if (group->m_existSameQckBtn) {
                    if (btn->m_fileName == group->m_fileName) {
                            group->m_existSameQckBtn = false;
                            group->setQckLchBtn(NULL);
                    }
                }
            }
            m_vBtn.erase(it);
            break;
        }
    }
    m_layout->removeWidget(btn);
    btn->deleteLater();
    saveSettings();
}

void UKUITaskBar::saveSettingsSlot() {
    saveSettings();
}

void UKUITaskBar::saveSettings()
{
    PluginSettings *settings = m_plugin->settings();
    settings->remove("apps");
    QList<QMap<QString, QVariant> > hashList;
    int size = m_layout->count();
    for (int j = 0; j < size; ++j)
    {
        UKUITaskGroup *b = qobject_cast<UKUITaskGroup*>(m_layout->itemAt(j)->widget());
        if (!(m_vBtn.contains(b) || m_knownWindows.contains(m_knownWindows.key(b)))) continue;
        if (!b->m_statFlag && b->m_existSameQckBtn) continue;
        if (!b) continue;
        if (b->m_statFlag && b->m_existSameQckBtn){
            b = b->getQckLchBtn();
        }
        if (!b || b->m_statFlag)
            continue;
        // convert QHash<QString, QString> to QMap<QString, QVariant>
        QMap<QString, QVariant> map;
        QHashIterator<QString, QString> it(b->settingsMap());
        while (it.hasNext())
        {
            it.next();
            map[it.key()] = it.value();
        }
        hashList << map;
    }
    settings->setArray("apps", hashList);
}

int UKUITaskBar::indexOfButton(UKUITaskGroup* button) const
{
    return m_layout->indexOf(button);
}

int UKUITaskBar::countOfButtons() const
{
    return m_layout->count();
}

void UKUITaskBar::wlKwinSigHandler(quint32 wl_winId, int opNo, QString wl_iconName, QString wl_caption) {
    qDebug()<<"UKUITaskBar::wlKwinSigHandler"<<wl_winId<<opNo<<wl_iconName<<wl_caption;
    if (!opNo) {
//        addWindow_wl(wl_iconName, wl_caption, wl_winId);
    }
    switch (opNo) {
    case 1:
        m_knownWindows.find(wl_winId).value()->setActivateState_wl(false);
        break;
    case 2:
        onWindowRemoved(wl_winId);
        break;
    case 3:
        m_knownWindows.find(wl_winId).value()->setActivateState_wl(true);
        break;
    case 4:
        addWindow_wl(wl_iconName, wl_caption, wl_winId);
        m_knownWindows.find(wl_winId).value()->wl_widgetUpdateTitle(wl_caption);
        break;
    }
}

void UKUITaskBar::addWindow_wl(QString iconName, QString caption, WId window)
{
    // If grouping disabled group behaves like regular button
//    QString temp_group_id=caption;
//    QStringList strList = temp_group_id.split(" ");

    const QString group_id = captionExchange(caption);
    if (QIcon::fromTheme(group_id).isNull()) {
        iconName = QDir::homePath() + "/.local/share/icons/" + group_id + ".svg";
        if (!QFile(iconName).exists()) {
            iconName = QDir::homePath() + "/.local/share/icons/" + group_id + ".png";
            if (!QFile(iconName).exists()) {
                iconName = group_id;
            }
        }
    } else {
        iconName = group_id;
    }
    UKUITaskGroup *group = nullptr;
    auto i_group = m_knownWindows.find(window);
    if (m_knownWindows.end() != i_group)
    {
        if ((*i_group)->groupName() == group_id)
            group = *i_group;
        else
            (*i_group)->onWindowRemoved(window);
    }

    if (!group && m_groupingEnabled && group_id.compare("kylin-video"))
    {
        for (auto i = m_knownWindows.cbegin(), i_e = m_knownWindows.cend(); i != i_e; ++i)
        {
            if ((*i)->groupName() == group_id)
            {
                group = *i;
                break;
            }
        }
    }

    if (!group)
    {
        group = new UKUITaskGroup(iconName, caption, window, this);
        m_placeHolder->hide();
        connect(group, SIGNAL(groupBecomeEmpty(QString)), this, SLOT(groupBecomeEmptySlot()));
        connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &UKUITaskGroup::popupShown, this, &UKUITaskBar::popupShown);
        connect(group, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
            buttonMove(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource), pos);
        });

        //wayland临时图标适配主题代码处理
        /*********************************************/
        if(QIcon::fromTheme(group_id).hasThemeIcon(group_id)){
            group->setIcon(QIcon::fromTheme(group_id));
        }else{
            group->setIcon(QIcon::fromTheme(iconName));
        }

        connect(m_changeTheme, &QGSettings::changed, this, [=] (const QString &key){
            if(key=="iconThemeName"){
                sleep(1);
                if(QIcon::fromTheme(group_id).hasThemeIcon(group_id)){
                    group->setIcon(QIcon::fromTheme(group_id));
                }else{
                    group->setIcon(QIcon::fromTheme(iconName));
                }
            }
        });
        /*********************************************/

//        group->setFixedSize(panel()->panelSize(),panel()->panelSize());
        //group->setFixedSize(40,40);
        QString groupDesktopName = "/usr/share/applications/" + group_id + ".desktop";
        bool isNeedAddNewWidget = true;
        for (auto it = m_vBtn.begin(); it!=m_vBtn.end(); ++it) {
            UKUITaskGroup *pQuickBtn = *it;
            if(pQuickBtn->m_fileName == groupDesktopName
               &&(m_layout->indexOf(pQuickBtn) >= 0 )) {
                m_layout->addWidget(group);
                m_layout->moveItem(m_layout->indexOf(group), m_layout->indexOf(pQuickBtn));
                pQuickBtn->setHidden(true);
                isNeedAddNewWidget = false;
                group->m_existSameQckBtn = true;
                pQuickBtn->m_existSameQckBtn = true;
                group->wl_widgetUpdateTitle(caption);
                group->setStyle(new CustomStyle("taskbutton"));
                group->setToolButtonsStyle(m_buttonStyle);
                group->setQckLchBtn(pQuickBtn);
                group->m_fileName = groupDesktopName;
                break;
            }
        }

        if (isNeedAddNewWidget) {
            m_layout->addWidget(group);
            group->wl_widgetUpdateTitle(caption);
            group->setStyle(new CustomStyle("taskbutton"));
            group->setToolButtonsStyle(m_buttonStyle);
        }
    }

    m_knownWindows[window] = group;
    group->wl_addWindow(window);
}


QString UKUITaskBar::captionExchange(QString str)
{
    QString temp_group_id=str;
    QStringList strList = temp_group_id.split(" ");
    QString group_id = strList[0];
    QStringList video_list;
    if(m_androidIconHash.keys().contains(temp_group_id)){
        group_id=m_androidIconHash.value(temp_group_id);
    }else{
        video_list<<"影音"<<"Video";
        if(video_list.contains(group_id)) group_id ="kylin-video";
        else group_id="application-x-desktop";
    }
    return group_id;
}

QHash<QString,QString> UKUITaskBar::matchAndroidIcon()
{
    QHash<QString,QString> hash;
    printf("*************\n");
    QFile file("/usr/share/ukui/ukui-panel/plugin-taskbar/name-icon.match");
    if(!file.open(QIODevice::ReadOnly))  qDebug()<<"Read FIle failed";
    while (!file.atEnd()){
        QByteArray line= file.readLine();
        QString str=file.readLine();
        str.section('picture',1,1).trimmed().toStdString();
        str.simplified();
        QString str_name = str.section(QRegExp("[;]"),0,0);
        str_name = str_name.simplified();
        str_name = str_name.remove("name=");

        QString str_icon = str.section(QRegExp("[;]"),1,1);
        str_icon = str_icon.simplified();
        str_icon = str_icon.remove("icon=");

        hash.insert(str_name,str_icon);
    }

    return hash;
}
