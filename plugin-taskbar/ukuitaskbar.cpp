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
#include "../panel/common/ukuigridlayout.h"

#include "ukuitaskbar.h"
#include "ukuitaskgroup.h"
#include "quicklaunchaction.h"
#define PANEL_SETTINGS     "org.ukui.panel.settings"
#define PANEL_LINES        "panellines"
#define PANEL_CONFIG_PATH  "/usr/share/ukui/ukui-panel/panel-commission.ini"
using namespace UKUi;
/************************************************

************************************************/
UKUITaskBar::UKUITaskBar(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QScrollArea(parent),
    mSignalMapper(new QSignalMapper(this)),
    mButtonStyle(Qt::ToolButtonIconOnly),
    mButtonWidth(400),
    mButtonHeight(100),
    mCloseOnMiddleClick(true),
    mRaiseOnCurrentDesktop(true),
    mShowOnlyOneDesktopTasks(true),
    mShowDesktopNum(0),
    mShowOnlyCurrentScreenTasks(false),
    mShowOnlyMinimizedTasks(false),
    mAutoRotate(true),
    mGroupingEnabled(true),
    mShowGroupOnHover(true),
    mIconByClass(false),
    mCycleOnWheelScroll(true),
    mPlugin(plugin),
    mIgnoreWindow(),
    mPlaceHolder(new QWidget(this)),
    mStyle(new LeftAlignedTextStyle())
{
    mAllFrame=new QWidget(this);
    mAllFrame->setAttribute(Qt::WA_TranslucentBackground);

    this->setWidget(mAllFrame);
    this->horizontalScrollBar()->setVisible(false);
    this->verticalScrollBar()->setVisible(false);
    this->setFrameShape(QFrame::NoFrame);//去掉边框
    this->setWidgetResizable(true);
    this->viewport()->setStyleSheet("background-color:transparent;");
    horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");

    taskstatus=NORMAL;

    mLayout = new UKUi::GridLayout(mAllFrame);
    mAllFrame->setLayout(mLayout);
    mLayout->setMargin(0);
    mLayout->setStretch(UKUi::GridLayout::StretchHorizontal | UKUi::GridLayout::StretchVertical);

    mPlaceHolder->setMinimumSize(1,1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    QString filename = QString::fromLocal8Bit(PANEL_CONFIG_PATH);
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");
    m_settings.beginGroup("IgnoreWindow");
    mIgnoreWindow = m_settings.value("ignoreWindow", "").toStringList();
    m_settings.endGroup();

    //往任务栏中加入快速启动按钮
    refreshQuickLaunch();
    realign();

    settingsChanged();
    setAcceptDrops(true);

    const QByteArray id(PANEL_SETTINGS);
    if (QGSettings::isSchemaInstalled(id)) {
        settings=new QGSettings(id);
    }

    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &UKUITaskBar::activateTask);

    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &UKUITaskBar::onWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &UKUITaskBar::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &UKUITaskBar::onWindowRemoved);
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(onDesktopChanged()));

    //龙芯机器的最小化任务窗口的预览窗口的特殊处理
    system("cat /proc/cpuinfo >> /tmp/_tmp_cpu_info_cat_");
    QFile file("/tmp/_tmp_cpu_info_cat_");
    if (!file.open(QIODevice::ReadOnly)) qDebug() << "Read CpuInfo Failed.";
    while (CpuInfoFlg && !file.atEnd()) {
        QByteArray line = file.readLine();
        QString str(line);
        if (str.contains("Loongson")) CpuInfoFlg = false;
    }
    file.close();
    saveSettings();

    /**/
    QDBusConnection::sessionBus().unregisterService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerObject("/taskbar/click", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/quicklaunch"), "org.ukui.panel.taskbar", "AddToTaskbar", this, SLOT(_AddToTaskbar(QString)));
    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/quicklaunch"), "org.ukui.panel.taskbar", "RemoveFromTaskbar", this, SLOT(removeFromTaskbar(QString)));

    if (mLayout->count() == 0) {
        mLayout->addWidget(mPlaceHolder);
    } else {
        mPlaceHolder->setFixedSize(0,0);
    }
}

/************************************************

 ************************************************/
UKUITaskBar::~UKUITaskBar()
{
    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        (*it)->deleteLater();
        mVBtn.erase(it);
    }
    mVBtn.clear();
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
    for (auto i = mKnownWindows.begin(); mKnownWindows.end() != i; ++i)
    {
        (*i)->onDesktopChanged();
        if ((*i)->existSameQckBtn) {
            UKUITaskGroup* btn = (*i)->getOwnQckBtn();
            if (mVBtn.contains(btn))
                btn->setVisible((*i)->isHidden());
        }
    }
}

void UKUITaskBar::refreshQuickLaunch(){
    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        (*it)->deleteLater();
        mVBtn.erase(it);
    }

    QString desktop;
    QString file;

    //gsetting的方式读取写入 apps
    QList<QMap<QString, QVariant> > apps = mPlugin->settings()->readArray("apps");
    QList<QMap<QString, QVariant> > &taskbar_apps = apps;
    if (apps.isEmpty()) {
        apps = verifyQuicklaunchConfig(taskbar_apps);
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

QList<QMap<QString, QVariant> > UKUITaskBar::verifyQuicklaunchConfig(QList<QMap<QString, QVariant> > &apps)
{
    QString filename = QDir::homePath() + "/.config/ukui/panel.conf";
    //若taskbar中没有apps，则把quicklaunch中的内容复制到taskbar
    if (apps.isEmpty()) {
        qDebug()<<"Taskbar is empty, read apps from quicklaunch";
        QSettings user_qsettings(filename,QSettings::IniFormat);
        user_qsettings.beginGroup("quicklaunch");
        //加入标志位,保证只读取一次
        if (!user_qsettings.contains("readLock")) {
            QList<QMap<QString, QVariant> > array;
            int size = user_qsettings.beginReadArray("apps");
            for (int i = 0; i < size; ++i)
            {
                user_qsettings.setArrayIndex(i);
                QMap<QString, QVariant> hash;
                const auto keys = user_qsettings.childKeys();
                for (const QString &key : keys)
                    hash[key] = user_qsettings.value(key);
                array << hash;
            }
            apps = array;
            user_qsettings.endArray();
            user_qsettings.setValue("readLock", QString("Locked"));
        }
        user_qsettings.endGroup();
        user_qsettings.sync();

        return apps;
    }
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
    if (!src || -1 == (src_index = mLayout->indexOf(src)))
    {
        return;
    }

    const int size = mLayout->count();
    Q_ASSERT(0 < size);
    //dst is nullptr in case the drop occured on empty space in taskbar
    int dst_index;
    if (nullptr == dst)
    {
        //moving based on taskbar (not signaled by button)
        QRect occupied = mLayout->occupiedGeometry();
        QRect last_empty_row{occupied};
        const QRect last_item_geometry = mLayout->itemAt(size - 1)->geometry();
        if (mPlugin->panel()->isHorizontal())
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
        dst_index = mLayout->indexOf(dst);
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
            || mLayout->animatedMoveInProgress()
       ) {
        return;
    }

    mLayout->moveItem(src_index, dst_index, true);
}

void UKUITaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    UKUITaskGroup * const group = qobject_cast<UKUITaskGroup*>(sender());
    Q_ASSERT(group);

    for (auto i = mKnownWindows.begin(); mKnownWindows.end() != i; )
    {
        if (group == *i) {
            swid.removeOne(i.key());
            i = mKnownWindows.erase(i);
            break;
        }
        else
            ++i;
    }
    for (auto it = mVBtn.begin(); it!=mVBtn.end(); ++it)
    {
        UKUITaskGroup *pQuickBtn = *it;
        if(pQuickBtn->file_name == group->file_name
           &&(mLayout->indexOf(pQuickBtn) >= 0 ))
        {
            pQuickBtn->setHidden(false);
            mLayout->moveItem(mLayout->indexOf(pQuickBtn), mLayout->indexOf(group));
            pQuickBtn->existSameQckBtn = false;
            break;
        }
    }
    mLayout->removeWidget(group);
    group->deleteLater();
}

void UKUITaskBar::addWindow(WId window)
{
    // If grouping disabled group behaves like regular button
    const QString group_id = mGroupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
    if (mIgnoreWindow.contains(group_id)) {
        return;
    }

    UKUITaskGroup *group = nullptr;
    bool isNeedAddNewWidget = true;
    auto i_group = mKnownWindows.find(window);
    if (mKnownWindows.end() != i_group)
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
    if (!group && mGroupingEnabled && !andriod_window_list.contains(group_id))
    {
        for (auto i = mKnownWindows.cbegin(), i_e = mKnownWindows.cend(); i != i_e; ++i)
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
        for (auto it = mVBtn.begin(); it!=mVBtn.end(); ++it)
        {
            UKUITaskGroup *pQuickBtn = *it;
            if(pQuickBtn->file_name == group->file_name
               &&(mLayout->indexOf(pQuickBtn) >= 0 ))
            {
                mLayout->addWidget(group);
                mLayout->moveItem(mLayout->indexOf(group), mLayout->indexOf(pQuickBtn));
                pQuickBtn->setHidden(true);
                isNeedAddNewWidget = false;
                group->existSameQckBtn = true;
                pQuickBtn->existSameQckBtn = true;
                group->setQckLchBtn(pQuickBtn);
                break;
            }
        }
        if(isNeedAddNewWidget)
        {
            mLayout->addWidget(group);
        }
        group->setToolButtonsStyle(mButtonStyle);
    }
    mKnownWindows[window] = group;
    swid.push_back(window);

    group->addWindow(window);
    group->groupName();
    group->updateIcon();
}

auto UKUITaskBar::removeWindow(windowMap_t::iterator pos) -> windowMap_t::iterator
{
    WId const window = pos.key();
    UKUITaskGroup * const group = *pos;
    swid.removeOne(window);
    auto ret = mKnownWindows.erase(pos);
    group->onWindowRemoved(window);
    //if (countOfButtons() <= 32) tmpwidget->setHidden(true);
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
  //  mLayout->addWidget(tmpwidget);

    //emulate windowRemoved if known window not reported by KWindowSystem
    for (auto i = mKnownWindows.begin(), i_e = mKnownWindows.end(); i != i_e; )
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

    auto i = mKnownWindows.find(window);
    if (mKnownWindows.end() != i)
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
    auto const pos = mKnownWindows.find(window);
    if (mKnownWindows.end() == pos && acceptWindow(window))
        addWindow(window);
}

void UKUITaskBar::onWindowRemoved(WId window)
{
    auto const pos = mKnownWindows.find(window);
    if (mKnownWindows.end() != pos)
    {
        removeWindow(pos);
    }
}

void UKUITaskBar::refreshButtonRotation()
{
    bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

    IUKUIPanel::Position panelPosition = mPlugin->panel()->position();
   // emit buttonRotationRefreshed(autoRotate, panelPosition);
}

/************************************************

 ************************************************/
void UKUITaskBar::refreshPlaceholderVisibility()
{
    bool haveVisibleWindow = false;
    for (auto i = mKnownWindows.cbegin(), i_e = mKnownWindows.cend(); i_e != i; ++i)
    {
        if ((*i)->isVisible())
        {
            haveVisibleWindow = true;
            break;
        }
    }
    mPlaceHolder->setVisible(!haveVisibleWindow);
    if (haveVisibleWindow || mLayout->count() != 0) {
        mPlaceHolder->setFixedSize(0,0);
     } else {
        mPlaceHolder->setMinimumSize(1,1);
        mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
}

/************************************************

 ************************************************/
void UKUITaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    emit buttonStyleRefreshed(mButtonStyle);
}

void UKUITaskBar::settingsChanged()
{
    bool groupingEnabledOld = mGroupingEnabled;
    bool showOnlyOneDesktopTasksOld = mShowOnlyOneDesktopTasks;
    const int showDesktopNumOld = mShowDesktopNum;
    bool showOnlyCurrentScreenTasksOld = mShowOnlyCurrentScreenTasks;
    bool showOnlyMinimizedTasksOld = mShowOnlyMinimizedTasks;
    const bool iconByClassOld = mIconByClass;

    mButtonWidth = mPlugin->settings()->value("buttonWidth", 400).toInt();
    mButtonHeight = mPlugin->settings()->value("buttonHeight", 100).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
        setButtonStyle(Qt::ToolButtonIconOnly);
    else if (s == "TEXT")
        setButtonStyle(Qt::ToolButtonTextOnly);
    else
        setButtonStyle(Qt::ToolButtonIconOnly);

    mShowOnlyOneDesktopTasks = mPlugin->settings()->value("showOnlyOneDesktopTasks", mShowOnlyOneDesktopTasks).toBool();
    mShowDesktopNum = mPlugin->settings()->value("showDesktopNum", mShowDesktopNum).toInt();
    mShowOnlyCurrentScreenTasks = mPlugin->settings()->value("showOnlyCurrentScreenTasks", mShowOnlyCurrentScreenTasks).toBool();
    mShowOnlyMinimizedTasks = mPlugin->settings()->value("showOnlyMinimizedTasks", mShowOnlyMinimizedTasks).toBool();
    mAutoRotate = mPlugin->settings()->value("autoRotate", true).toBool();
    mCloseOnMiddleClick = mPlugin->settings()->value("closeOnMiddleClick", true).toBool();
    mRaiseOnCurrentDesktop = mPlugin->settings()->value("raiseOnCurrentDesktop", false).toBool();
    mGroupingEnabled = mPlugin->settings()->value("groupingEnabled",true).toBool();
    mShowGroupOnHover = mPlugin->settings()->value("showGroupOnHover",true).toBool();
    mIconByClass = mPlugin->settings()->value("iconByClass", false).toBool();
    mCycleOnWheelScroll = mPlugin->settings()->value("cycleOnWheelScroll", true).toBool();

    // Delete all groups if grouping feature toggled and start over
    if (groupingEnabledOld != mGroupingEnabled)
    {
        for (int i = mKnownWindows.size() - 1; 0 <= i; --i)
        {
            UKUITaskGroup * group = mKnownWindows.value(swid.value(i));
            if (nullptr != group)
            {
                mLayout->takeAt(i);
                group->deleteLater();
            }
        }
        mKnownWindows.clear();
        swid.clear();
    }

    if (showOnlyOneDesktopTasksOld != mShowOnlyOneDesktopTasks
            || (mShowOnlyOneDesktopTasks && showDesktopNumOld != mShowDesktopNum)
            || showOnlyCurrentScreenTasksOld != mShowOnlyCurrentScreenTasks
            || showOnlyMinimizedTasksOld != mShowOnlyMinimizedTasks
            )
        emit showOnlySettingChanged();
    if (iconByClassOld != mIconByClass)
        emit iconByClassChanged();

    refreshTaskList();
}

void UKUITaskBar::setShowGroupOnHover(bool bFlag)
{
    mShowGroupOnHover = bFlag;
}

int i = 0;
void UKUITaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    IUKUIPanel *panel = mPlugin->panel();
    QSize maxSize = QSize(mPlugin->panel()->panelSize(), mPlugin->panel()->panelSize());
    QSize minSize = QSize(mPlugin->panel()->iconSize()/2, mPlugin->panel()->iconSize()/2);
    int iconsize = panel->iconSize();

    bool rotated = false;
    if (panel->isHorizontal())
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mAllFrame->setMinimumSize(QSize((mLayout->count()+3)*panel->panelSize(),panel->panelSize()));
        if (mAllFrame->width() < this->width()) {
            mAllFrame->setFixedWidth(this->width());
        }

        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
    }
    else
    {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        mAllFrame->setMinimumSize(QSize(panel->panelSize(),(mLayout->count()+3)*panel->panelSize()));
        if (mAllFrame->height() < this->height()) {
            mAllFrame->setFixedHeight(this->height());
        }

        mLayout->setRowCount(0);

        if (mButtonStyle == Qt::ToolButtonIconOnly)
        {
            // Vertical + Icons
            mLayout->setColumnCount(panel->lineCount());
        }
        else
        {
            rotated = mAutoRotate && (panel->position() == IUKUIPanel::PositionLeft || panel->position() == IUKUIPanel::PositionRight);

            // Vertical + Text
            if (rotated)
            {
                maxSize.rwidth()  = mButtonHeight;
                maxSize.rheight() = mButtonWidth;

                mLayout->setColumnCount(panel->lineCount());
            }
            else
            {
                mLayout->setColumnCount(1);
            }
        }
    }

    for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
        group->setIconSize(QSize(iconsize,iconsize));
    }
    for (int i = 0; i < mVBtn.size(); i++) {
        UKUITaskGroup * quicklaunch = mVBtn.value(i);
        quicklaunch->setIconSize(QSize(iconsize, iconsize));
    }
    mLayout->setCellMinimumSize(minSize);
    mLayout->setCellMaximumSize(maxSize);
    mLayout->setDirection(rotated ? UKUi::GridLayout::TopToBottom : UKUi::GridLayout::LeftToRight);
    mLayout->setEnabled(true);
    //our placement on screen could have been changed
    emit showOnlySettingChanged();
    emit refreshIconGeometry();
}

void UKUITaskBar::wheelEvent(QWheelEvent* event)
{
    if (this->verticalScrollBarPolicy()==Qt::ScrollBarAlwaysOff) {
        if (event->delta()>=0) {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()-40);
            qDebug()<<"-40-horizontalScrollBar()->value()"<<horizontalScrollBar()->value();
        } else {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value()+40);
            if (horizontalScrollBar()->value()>mAllFrame->width()) {
                horizontalScrollBar()->setValue(mAllFrame->width());
            }
            qDebug()<<"+40+horizontalScrollBar()->value()"<<horizontalScrollBar()->value();
            //                qDebug()<<"scrollArea->horizontalScrollBar()->width()"<<horizontalScrollBar()->width();
        }
        //            qDebug()<<"scrollArea->horizontalScrollBar()->width()"<<horizontalScrollBar()->width();
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
        mStyle->setBaseStyle(NULL);

    QFrame::changeEvent(event);
}


void UKUITaskBar::activateTask(int pos)
{
    for (int i = 0; i < mKnownWindows.size(); ++i)
    {
        UKUITaskGroup * g = mKnownWindows.value(swid.value(i));
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
    taskstatus=HOVER;
    update();
}

void UKUITaskBar::leaveEvent(QEvent *)
{
    taskstatus=NORMAL;
    update();
}

void UKUITaskBar::paintEvent(QPaintEvent *)
{
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);

        switch(taskstatus)
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
    mLayout->setEnabled(false);
    UKUITaskGroup *btn = new UKUITaskGroup(action, mPlugin, this);
    btn->setArrowType(Qt::NoArrow);
    /*＠bug
     * 快速启动栏右键菜单原本的样式有对于不可选项有置灰效果，
     * 后跟随主题框架之后置灰效果消失，可能与此属性相关
     */
    //        btn->setMenu(Qt::InstantPopup);
    for (auto it = mKnownWindows.begin(); it!=mKnownWindows.end(); ++it)
    {
        UKUITaskGroup *group = *it;
        if(btn->file_name == group->file_name
           &&(mLayout->indexOf(group) >= 0))
        {
            mLayout->addWidget(btn);
            mLayout->moveItem(mLayout->indexOf(btn), mLayout->indexOf(group));
            isNeedAddNewWidget = false;
            group->existSameQckBtn = true;
            btn->existSameQckBtn = true;
            mVBtn.push_back(btn);
            group->setQckLchBtn(btn);
            btn->setHidden(group->isVisible());
            break;
        }
    }
    if (isNeedAddNewWidget) {
        mLayout->addWidget(btn);
        mVBtn.push_back(btn);
        mLayout->moveItem(mLayout->indexOf(btn), countOfButtons() - 1);
    }
    connect(btn, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
        switchButtons(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource));//, pos);
    });
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(t_saveSettings()), this, SLOT(saveSettingsSlot()));
    mLayout->setEnabled(true);
}

void UKUITaskBar::switchButtons(UKUITaskGroup *dst_button, UKUITaskGroup *src_button)
{
    if (dst_button == src_button)
        return;
    if (!dst_button || !src_button)
        return;
    int dst = mLayout->indexOf(dst_button);
    int src = mLayout->indexOf(src_button);
    if (dst == src
            || mLayout->animatedMoveInProgress()
       ) {
        return;
    }
    mLayout->moveItem(src, dst, true);
}

bool UKUITaskBar::checkButton(QuickLaunchAction* action)
{
    bool checkresult;
    UKUITaskGroup* btn = new UKUITaskGroup(action, mPlugin, this);
    int i = 0;
    int counts = mVBtn.size();
    if(mVBtn.size()>0){
        while (i != counts) {
            UKUITaskGroup *b = mVBtn.value(i);
            qDebug()<<"mLayout->itemAt("<<i<<") ";
            if(b->file_name == btn->file_name) {
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
    UKUITaskGroup* btn = new UKUITaskGroup(action, mPlugin, this);
    if (!btn)
        return;
    while (i < mVBtn.size()) {
        UKUITaskGroup *tmp = mVBtn.value(i);
        if (QString::compare(btn->file_name, tmp->file_name) == 0) {
            doInitGroupButton(tmp->file_name);
            tmp->deleteLater();
            mLayout->removeWidget(tmp);
            mVBtn.remove(i);
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
    while (i < mVBtn.size()) {
        UKUITaskGroup *tmp = mVBtn.value(i);
        if (QString::compare(file, tmp->file_name) == 0) {
            doInitGroupButton(tmp->file_name);
            tmp->deleteLater();
            mLayout->removeWidget(tmp);
            mVBtn.remove(i);
            break;
        }
        ++i;
    }
}

void UKUITaskBar::WindowAddtoTaskBar(QString arg) {
    for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
            if (arg.compare(group->groupName()) == 0) {
                _AddToTaskbar(group->file_name);
                break;
        }
    }
}

void UKUITaskBar::WindowRemovefromTaskBar(QString arg) {
    for (auto it = mVBtn.begin(); it!=mVBtn.end(); ++it)
    {
        UKUITaskGroup *pQuickBtn = *it;
        if(pQuickBtn->file_name == arg
           && (mLayout->indexOf(pQuickBtn) >= 0 ))
        {
            doInitGroupButton(pQuickBtn->file_name);
            mVBtn.removeOne(pQuickBtn);
            pQuickBtn->deleteLater();
            mLayout->removeWidget(pQuickBtn);
            saveSettings();
            break;
        }
    }
}

void UKUITaskBar::_AddToTaskbar(QString arg) {
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    if (xdg.load(fileName)){
        if(!checkButton(new QuickLaunchAction(&xdg, this))){
            addButton(new QuickLaunchAction(&xdg, this));
            mPlaceHolder->hide();
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
    for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
    {
        UKUITaskGroup *group = it.value();
        if (group->existSameQckBtn) {
            if (sname == group->file_name) {
                    group->existSameQckBtn = false;
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
    for(auto it = mVBtn.begin();it != mVBtn.end();it++)
    {
        if(*it == btn)
        {
            for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
            {
                UKUITaskGroup *group = it.value();
                if (group->existSameQckBtn) {
                    if (btn->file_name == group->file_name) {
                            group->existSameQckBtn = false;
                            group->setQckLchBtn(NULL);
                    }
                }
            }
            mVBtn.erase(it);
            break;
        }
    }
    mLayout->removeWidget(btn);
    btn->deleteLater();
    saveSettings();
}

void UKUITaskBar::saveSettingsSlot() {
    saveSettings();
}

void UKUITaskBar::saveSettings()
{
    PluginSettings *settings = mPlugin->settings();
    settings->remove("apps");
    QList<QMap<QString, QVariant> > hashList;
    int size = mLayout->count();
    for (int j = 0; j < size; ++j)
    {
        UKUITaskGroup *b = qobject_cast<UKUITaskGroup*>(mLayout->itemAt(j)->widget());
        if (!(mVBtn.contains(b) || mKnownWindows.contains(mKnownWindows.key(b)))) continue;
        if (!b->statFlag && b->existSameQckBtn) continue;
        if (!b) continue;
        if (b->statFlag && b->existSameQckBtn){
            b = b->getQckLchBtn();
        }
        if (!b || b->statFlag)
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
    return mLayout->indexOf(button);
}

int UKUITaskBar::countOfButtons() const
{
    return mLayout->count();
}
