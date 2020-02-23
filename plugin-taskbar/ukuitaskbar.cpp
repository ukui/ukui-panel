/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
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
#include <XdgIcon>

#include "ukuitaskbar.h"
#include "ukuitaskgroup.h"
using namespace UKUi;

/************************************************

************************************************/
UKUITaskBar::UKUITaskBar(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
    mSignalMapper(new QSignalMapper(this)),
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mButtonWidth(400),
    mButtonHeight(100),
    mCloseOnMiddleClick(true),
    mRaiseOnCurrentDesktop(true),
    mShowOnlyOneDesktopTasks(false),
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
    setStyle(mStyle);
    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    mLayout->setStretch(UKUi::GridLayout::StretchHorizontal | UKUi::GridLayout::StretchVertical);

    realign();

    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mLayout->addWidget(mPlaceHolder);

    QTimer::singleShot(0, this, SLOT(settingsChanged()));
    setAcceptDrops(true);

    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &UKUITaskBar::activateTask);
    QTimer::singleShot(0, this, &UKUITaskBar::registerShortcuts);

    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &UKUITaskBar::onWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &UKUITaskBar::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &UKUITaskBar::onWindowRemoved);
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

    UKUITaskGroup *group = nullptr;
    auto i_group = mKnownWindows.find(window);
    if (mKnownWindows.end() != i_group)
    {
        if ((*i_group)->groupName() == group_id)
            group = *i_group;
        else
            (*i_group)->onWindowRemoved(window);
    }

    //check if window belongs to some existing group
    if (!group && mGroupingEnabled)
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
    qDebug()<<"onWindowAdded";
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
    emit buttonRotationRefreshed(autoRotate, panelPosition);
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
    const Qt::ToolButtonStyle old_style = mButtonStyle;
    mButtonStyle = buttonStyle;
    if (old_style != mButtonStyle)
        emit buttonStyleRefreshed(mButtonStyle);
}

void UKUITaskBar::setShowGroupOnHover(bool bFlag)
{

}

/************************************************

 ************************************************/
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

/************************************************

 ************************************************/
void UKUITaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    IUKUIPanel *panel = mPlugin->panel();
    QSize maxSize = QSize(50, mButtonHeight);
    QSize minSize = QSize(0, 0);

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
    while (!button)
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
