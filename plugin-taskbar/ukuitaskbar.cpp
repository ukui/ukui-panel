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

#include "../panel/common/ukuigridlayout.h"
#include <unistd.h>
#include "ukuitaskbar.h"
#include "ukuitaskgroup.h"
using namespace UKUi;

/************************************************

************************************************/
UKUITaskBar::UKUITaskBar(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
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
    mPlaceHolder(new QWidget(this)),
    mStyle(new LeftAlignedTextStyle())
{
    taskstatus=NORMAL;
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);   //设置无边框窗口

    //setStyle(mStyle);
    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    mLayout->setStretch(UKUi::GridLayout::StretchHorizontal | UKUi::GridLayout::StretchVertical);

    realign();

    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mLayout->addWidget(mPlaceHolder);
    QTimer::singleShot(0,[this] {refreshPlaceholderVisibility();});
//    QTimer::singleShot(0, this, SLOT(settingsChanged()));
    settingsChanged();
//    setButtonStyle(Qt::ToolButtonIconOnly);
    setAcceptDrops(true);
    QGSettings *changeTheme;
    const QByteArray id_Theme("org.ukui.style");
    if(QGSettings::isSchemaInstalled(id_Theme)){
        changeTheme = new QGSettings(id_Theme);
    }
    connect(changeTheme, &QGSettings::changed, this, [=] (const QString &key){
        if(key=="iconThemeName"){
            sleep(1);
            for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
            {
                UKUITaskGroup *group = it.value();
                group->updateIcon();
            }
        }
    });
    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &UKUITaskBar::activateTask);
    QTimer::singleShot(0, this, &UKUITaskBar::registerShortcuts);

    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &UKUITaskBar::onWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &UKUITaskBar::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &UKUITaskBar::onWindowRemoved);

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

    /**/
    QDBusConnection::sessionBus().unregisterService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerService("com.ukui.panel.plugins.service");
    QDBusConnection::sessionBus().registerObject("/taskbar/click", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);
}

/************************************************

 ************************************************/
UKUITaskBar::~UKUITaskBar()
{
    delete mStyle;
}

/************************************************

 ************************************************/
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

bool UKUITaskBar::ignoreSymbolCMP(QString filename,QString groupname) {
    if (filename.isEmpty()) return false;
    groupname.replace(" ", "");
    groupname.replace("-", ".");
    groupname.replace(".demo", "");
    groupname.replace(".py", "");
    groupname.replace("org.", "");
    groupname.replace(".qt", "");
    filename.replace(" ", "");
    filename.replace("-", ".");
    filename.replace("org.", "");
    filename.replace(".desktop", "");
    if (groupname.toLower().contains(filename.toLower(), Qt::CaseInsensitive))
        return true;
    if (filename.toLower().contains(groupname.toLower(), Qt::CaseInsensitive))
        return true;
    if (groupname.toLower().contains("kylinweather") && filename.toLower().contains("china.weather"))
        return true;
    if (groupname.toLower().contains("srhuijian") && filename.toLower().contains("huijian"))
        return true;
    if (groupname.contains("用户手册") && filename.toLower().contains("kylin.user.guid"))
        return true;
    if (groupname.toLower().contains("wpsoffice") && filename.toLower().contains("wps.office.prometheus"))
        return true;
    if (groupname.toLower().contains("ukuisystemmonitor") && filename.toLower().contains("ukui.system.monitor"))
        return true;
    return false;
}

/************************************************

 ************************************************/
void UKUITaskBar::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(UKUITaskGroup::mimeDataFormat()))
    {
        event->acceptProposedAction();
        buttonMove(nullptr, qobject_cast<UKUITaskGroup *>(event->source()), event->pos());
    } else
        event->ignore();
    QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskBar::dragMoveEvent(QDragMoveEvent * event)
{
    //we don't get any dragMoveEvents if dragEnter wasn't accepted
    buttonMove(nullptr, qobject_cast<UKUITaskGroup *>(event->source()), event->pos());
    QWidget::dragMoveEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskBar::buttonMove(UKUITaskGroup * dst, UKUITaskGroup * src, QPoint const & pos)
{
    int src_index;
    if (!src || -1 == (src_index = mLayout->indexOf(src)))
    {
        qDebug() << "Dropped invalid";
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
        if (occupied.contains(pos) && !last_empty_row.contains(pos))
            return;

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
       )
        return;

    mLayout->moveItem(src_index, dst_index, true);
}

/************************************************

 ************************************************/
void UKUITaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    UKUITaskGroup * const group = qobject_cast<UKUITaskGroup*>(sender());
    Q_ASSERT(group);

    for (auto i = mKnownWindows.begin(); mKnownWindows.end() != i; )
    {
        if (group == *i)
            i = mKnownWindows.erase(i);
        else
            ++i;
    }
    mLayout->removeWidget(group);
    group->deleteLater();
}

/************************************************

 ************************************************/
void UKUITaskBar::addWindow(WId window)
{
    // If grouping disabled group behaves like regular button
    const QString group_id = mGroupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
    //针对ukui-menu和ukui-sidebar做的特殊处理，及时窗口是普通窗口，也不在任务栏显示
    if(group_id.compare("ukui-menu")==0 || group_id.compare("ukui-sidebar")==0)
    {
        return;
    }
    UKUITaskGroup *group = nullptr;
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
    if (!group && mGroupingEnabled && group_id.compare("kydroid-display-window"))
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
        connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &UKUITaskGroup::popupShown, this, &UKUITaskBar::popupShown);
        connect(group, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
            buttonMove(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource), pos);
        });

        //group->setFixedSize(panel()->panelSize(),panel()->panelSize());
        mLayout->addWidget(group);
        group->setToolButtonsStyle(mButtonStyle);
    }

    mKnownWindows[window] = group;
    group->addWindow(window);
    group->groupName();
}

/************************************************

 ************************************************/
auto UKUITaskBar::removeWindow(windowMap_t::iterator pos) -> windowMap_t::iterator
{
    WId const window = pos.key();
    UKUITaskGroup * const group = *pos;
    auto ret = mKnownWindows.erase(pos);
    group->onWindowRemoved(window);
    return ret;
}

/************************************************

 ************************************************/
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

/************************************************

 ************************************************/
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
    auto const pos = mKnownWindows.find(window);
    if (mKnownWindows.end() == pos && acceptWindow(window))
        addWindow(window);
}

/************************************************

 ************************************************/
void UKUITaskBar::onWindowRemoved(WId window)
{
    auto const pos = mKnownWindows.find(window);
    if (mKnownWindows.end() != pos)
    {
        removeWindow(pos);
    }
}

/************************************************

 ************************************************/
void UKUITaskBar::refreshButtonRotation()
{
    bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

    IUKUIPanel::Position panelPosition = mPlugin->panel()->position();
//    emit buttonRotationRefreshed(autoRotate, panelPosition);
}

/************************************************

 ************************************************/
void UKUITaskBar::refreshPlaceholderVisibility()
{
    // if no visible group button show placeholder widget
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
    if (haveVisibleWindow)
        mPlaceHolder->setFixedSize(0, 0);
    else
    {
        mPlaceHolder->setMinimumSize(1, 1);
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
        for (int i = mLayout->count() - 1; 0 <= i; --i)
        {
            UKUITaskGroup * group = qobject_cast<UKUITaskGroup*>(mLayout->itemAt(i)->widget());
            if (nullptr != group)
            {
                mLayout->takeAt(i);
                group->deleteLater();
            }
        }
        mKnownWindows.clear();
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

void UKUITaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    IUKUIPanel *panel = mPlugin->panel();
    //set taskbar width by panel
    QSize maxSize = QSize(mPlugin->panel()->panelSize(), mPlugin->panel()->panelSize());
    QSize minSize = QSize(mPlugin->panel()->iconSize()/2, mPlugin->panel()->iconSize()/2);
    int iconsize = panel->iconSize();
    int panelsize = panel->panelSize();

    bool rotated = false;
    if (panel->isHorizontal())
    {
        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
    }
    else
    {
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
        //group->setFixedSize(panelsize, panelsize);
        group->setIconSize(QSize(iconsize,iconsize));
//        group->updateIcon();
    }
    mLayout->setCellMinimumSize(minSize);
    mLayout->setCellMaximumSize(maxSize);
    mLayout->setDirection(rotated ? UKUi::GridLayout::TopToBottom : UKUi::GridLayout::LeftToRight);
    mLayout->setEnabled(true);

    //our placement on screen could have been changed
    emit showOnlySettingChanged();
    emit refreshIconGeometry();
}

/************************************************

 ************************************************/
void UKUITaskBar::wheelEvent(QWheelEvent* event)
{
#if 0
    if (!mCycleOnWheelScroll)
        return QFrame::wheelEvent(event);

    static int threshold = 0;
    threshold += abs(event->delta());
    if (threshold < 300)
        return QFrame::wheelEvent(event);
    else
        threshold = 0;

    int delta = event->delta() < 0 ? 1 : -1;

    // create temporary list of visible groups in the same order like on the layout
    QList<UKUITaskGroup*> list;
    UKUITaskGroup *group = NULL;
    for (int i = 0; i < mLayout->count(); i++)
    {
        QWidget * o = mLayout->itemAt(i)->widget();
        UKUITaskGroup * g = qobject_cast<UKUITaskGroup *>(o);
        if (!g)
            continue;

        if (g->isVisible())
            list.append(g);
        if (g->isChecked())
            group = g;
    }

    if (list.isEmpty())
        return QFrame::wheelEvent(event);

    if (!group)
        group = list.at(0);

    UKUITaskButton *button = NULL;

    // switching between groups from temporary list in modulo addressing
    while (!button)conSize()
    {
        button = group->getNextPrevChildButton(delta == 1, !(list.count() - 1));
        if (button)
            button->raiseApplication();
        int idx = (list.indexOf(group) + delta + list.count()) % list.count();
        group = list.at(idx);
    }
    QFrame::wheelEvent(event);
#endif
}

/************************************************

 ************************************************/
void UKUITaskBar::resizeEvent(QResizeEvent* event)
{
    emit refreshIconGeometry();
    return QWidget::resizeEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskBar::changeEvent(QEvent* event)
{
    // if current style is changed, reset the base style of the proxy style
    // so we can apply the new style correctly to task buttons.
    if(event->type() == QEvent::StyleChange)
        mStyle->setBaseStyle(NULL);

    QFrame::changeEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskBar::registerShortcuts()
{
    // Register shortcuts to switch to the task
    // mPlaceHolder is always at position 0
    // tasks are at positions 1..10
//    GlobalKeyShortcut::Action * gshortcut;
//    QString path;
//    QString description;
//    for (int i = 1; i <= 10; ++i)
//    {
//        path = QString("/panel/%1/task_%2").arg(mPlugin->settings()->group()).arg(i);
//        description = tr("Activate task %1").arg(i);

//        gshortcut = GlobalKeyShortcut::Client::instance()->addAction(QStringLiteral(), path, description, this);

//        if (nullptr != gshortcut)
//        {
//            mKeys << gshortcut;
//            connect(gshortcut, &GlobalKeyShortcut::Action::registrationFinished, this, &UKUITaskBar::shortcutRegistered);
//            connect(gshortcut, &GlobalKeyShortcut::Action::activated, mSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//            mSignalMapper->setMapping(gshortcut, i);
//        }
//    }
}

void UKUITaskBar::shortcutRegistered()
{
//    GlobalKeyShortcut::Action * const shortcut = qobject_cast<GlobalKeyShortcut::Action*>(sender());

//    disconnect(shortcut, &GlobalKeyShortcut::Action::registrationFinished, this, &UKUITaskBar::shortcutRegistered);

//    const int i = mKeys.indexOf(shortcut);
//    Q_ASSERT(-1 != i);

//    if (shortcut->shortcut().isEmpty())
//    {
//        // Shortcuts come in order they were registered
//        // starting from index 0
//        const int key = (i + 1) % 10;
//        shortcut->changeShortcut(QStringLiteral("Meta+%1").arg(key));
//    }
}

void UKUITaskBar::activateTask(int pos)
{
    for (int i = 1; i < mLayout->count(); ++i)
    {
        QWidget * o = mLayout->itemAt(i)->widget();
        UKUITaskGroup * g = qobject_cast<UKUITaskGroup *>(o);
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
//                  p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
                  p.setPen(Qt::NoPen);
                  break;
              }
          case HOVER:
              {
//                  p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
                  p.setPen(Qt::NoPen);
                  break;
              }
          case PRESS:
              {
//                  p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
                  p.setPen(Qt::NoPen);
                  break;
              }
          }
        p.setRenderHint(QPainter::Antialiasing);
        p.drawRoundedRect(opt.rect,6,6);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UKUITaskBar::mousePressEvent(QMouseEvent *)
{
    /*创建QT的DBus信号*/
    QDBusMessage message =QDBusMessage::createSignal("/taskbar/click", "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp");
    /*
     * 发射信号,此处不给信号赋值　　message << QString("clicked");的原因是
     * 侧边栏和开始菜单仅需要点击信号
     * 若后期有特殊的设计需求，例如对鼠标左键，右键，滚轮　进行不同的处理就需要给信号赋值
     * tr:
     * Transmit the signal, the signal is not assigned here The reason is
     * The sidebar and start menu only need to click the signal
     * If there are special design requirements in the later stage,
     * such as different processing for the left mouse button, right mouse button, and scroll wheel,
     * the signal needs to be assigned and processed
     *
     * 需要此点击信号的应用需要做如下绑定
     * QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(client_get(void)));
     * 在槽函数client_get(void)　中处理接受到的点击信号
     * tr:
     * Applications that require this click signal need to do the following binding
     * Process the received click signal in the slot function client_get (void)
     * NOTE:https://blog.csdn.net/czhzasui/article/details/81071383
　　　*/
    QDBusConnection::sessionBus().send(message);
}

