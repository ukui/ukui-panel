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
#define PANEL_SETTINGS "org.ukui.panel.settings"
#define PANEL_LINES    "panellines"
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
    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    mLayout->setStretch(UKUi::GridLayout::StretchHorizontal | UKUi::GridLayout::StretchVertical);

    //往任务栏中加入快速启动按钮
    refreshQuickLaunch();

    //往任务栏中加入任务栏按钮
    realign();
    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }

    settingsChanged();
    setAcceptDrops(true);

    const QByteArray id(PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id)){
        settings=new QGSettings(id);
    }

    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &UKUITaskBar::activateTask);
    QTimer::singleShot(0, this, &UKUITaskBar::registerShortcuts);

    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &UKUITaskBar::onWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, &UKUITaskBar::onWindowAdded);
    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, &UKUITaskBar::onWindowRemoved);
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(onDesktopChanged()));
//    QGSettings *changeTheme;
//        const QByteArray id_Theme("org.ukui.style");
//        if(QGSettings::isSchemaInstalled(id_Theme)){
//            changeTheme = new QGSettings(id_Theme);
//        }
//        connect(changeTheme, &QGSettings::changed, this, [=] (const QString &key){
//            if(key=="iconThemeName"){
//                sleep(1);
//                for(auto it= mKnownWindows.begin(); it != mKnownWindows.end();it++)
//                {
//                    UKUITaskGroup *group = it.value();
//                    group->updateIcon();
//                }
//            }
//        });

    /*监听系统应用的目录以及安卓兼容应用的目录*/
    FilectrlAdaptor *f;
    f=new FilectrlAdaptor(this);
    QDBusConnection con=QDBusConnection::sessionBus();
    if(!con.registerService("com.ukui.panel.desktop") ||
       !con.registerObject("/", this))
    {
       qDebug()<<"fail";
    }
    fsWatcher=new QFileSystemWatcher(this);
    fsWatcher->addPath(desktopFilePath);
    fsWatcher->addPath(androidDesktopFilePath);
    connect(fsWatcher,&QFileSystemWatcher::directoryChanged,[this](){
               directoryUpdated(desktopFilePath);
               directoryUpdated(androidDesktopFilePath);
            });

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

QString UKUITaskBar::GetSecurityConfigPath(){
    return SecurityConfigPath;
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
    if (hasPlaceHolder) {
        mLayout->removeWidget(mPlaceHolder);
        hasPlaceHolder = false;
    }

    QStringList mblacklist;
    QStringList mwhitelist;
    if(mModel=="blacklist")
    mblacklist=blacklist;
    if(mModel=="whitelist")
    mwhitelist=whitelist;

    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        (*it)->deleteLater();
        mVBtn.erase(it);
    }

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;

    //gsetting的方式读取写入 apps
    const auto apps = mPlugin->settings()->readArray("apps");
    for (const QMap<QString, QVariant> &app : apps)
    {
        desktop = app.value("desktop", "").toString();

        if(mblacklist.contains(desktop)){
            desktop.clear();
        }
        if(mModel=="whitelist"){
            if(!mwhitelist.contains(desktop)){
                desktop.clear();
            }
        }

        file = app.value("file", "").toString();
        if (!desktop.isEmpty())
        {
            XdgDesktopFile xdg;
            if (xdg.load(desktop))
                addButton(new QuickLaunchAction(&xdg, this));
        }
        else if (! file.isEmpty())
        {
            addButton(new QuickLaunchAction(file, this));
        }
        else
        {
            execname = app.value("name", "").toString();
            exec = app.value("exec", "").toString();
            icon = app.value("icon", "").toString();
        }
    }
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
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
        return;
    }

    if (event->source() && event->source()->parent() == this)
    {
        event->acceptProposedAction();
    }
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

QString UKUITaskBar::isComputerOrTrash(QString urlName) {
    if (!urlName.compare("computer:///"))
        return QString("/usr/share/applications/peony-computer.desktop");
    if (!urlName.compare("trash:///"))
        return QString("/usr/share/applications/peony-trash.desktop");
    return urlName;
}

void UKUITaskBar::dropEvent(QDropEvent *e)
{
    //saveSettings();
    const auto urls = e->mimeData()->urls().toSet();
    for (const QUrl &url : urls)
    {
        XdgDesktopFile xdg;
        QString urlName(url.isLocalFile() ? url.toLocalFile() : url.url());
        QFileInfo ur(urlName);
        QString fileName("/usr/share/applications/");

        fileName.append(urlName.section('/', -1, -1));
        fileName = isComputerOrTrash(urlName);
        urlName = isComputerOrTrash(urlName);

        if (CheckIfExist(urlName)) return;
        if (CheckIfExist(fileName)) return;
        if (isDesktopFile(urlName)) {
            if (ur.isSymLink()){
                if (xdg.load(urlName) && xdg.isSuitable()) {
                   if (CheckIfExist(xdg.fileName())) return;
                   addButton(new QuickLaunchAction(&xdg, this));
                }
            } else {
                if (xdg.load(fileName) && xdg.isSuitable()) {
                   if (CheckIfExist(urlName)) return;
                   addButton(new QuickLaunchAction(&xdg, this));
                }
            }
        } else if (ur.exists() && ur.isExecutable() && !ur.isDir() || ur.isSymLink()) {
            if (ur.size() <= 153600)
                xdg.load(urlName);
            addButton(new QuickLaunchAction(urlName, this));
        } else if (ur.exists()) {
            if (ur.size() <= 153600)
                xdg.load(urlName);
            addButton(new QuickLaunchAction(urlName, this));
            //taskbar->pubAddButton(new QuickLaunchAction(urlName, urlName, "", this));
        } else {
            qWarning() << "XdgDesktopFile" << urlName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                                     tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(urlName)
                                     );
        }
    }
    saveSettings();
}

/************************************************

 ************************************************/
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

/************************************************

 ************************************************/
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
           &&(layout()->indexOf(pQuickBtn) >= 0 ))
        {
            pQuickBtn->setHidden(false);
            mLayout->moveItem(mLayout->indexOf(pQuickBtn), mLayout->indexOf(group));
            pQuickBtn->existSameQckBtn = false;
            break;
        }
    }
    mLayout->removeWidget(group);
    group->deleteLater();
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }

}

/************************************************

 ************************************************/
void UKUITaskBar::addWindow(WId window)
{
    if (countOfButtons() && hasPlaceHolder) {
        mLayout->removeWidget(mPlaceHolder);
        hasPlaceHolder = false;
    }
    // If grouping disabled group behaves like regular button
    const QString group_id = mGroupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    if(!group_id.compare("peony-qt-desktop"))
    {
        return;
    }
#endif
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
        //connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &UKUITaskGroup::popupShown, this, &UKUITaskBar::popupShown);
        connect(group, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
            switchButtons(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource));//, pos);
        });
        for (auto it = mVBtn.begin(); it!=mVBtn.end(); ++it)
        {
            UKUITaskGroup *pQuickBtn = *it;
            if(pQuickBtn->file_name == group->file_name
               &&(layout()->indexOf(pQuickBtn) >= 0 ))
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
    /*
    if (countOfButtons() > 32) {
        tmpwidget->setHidden(false);
        mLayout->moveItem(mLayout->indexOf(tmpwidget), countOfButtons() - 1);
    }
    */
    mKnownWindows[window] = group;
    swid.push_back(window);

    group->addWindow(window);
    group->groupName();
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

bool UKUITaskBar::isDesktopFile(QString urlName) {
   return urlName.section('/', -1, -1).section('.', -1, -1) == QString("desktop");
}

bool UKUITaskBar::pubCheckIfExist(QString name) {
    for (int i = 0; i < mVBtn.size(); i++) {
        QString cmpName;
        cmpName = (!mVBtn.value(i)->file_name.isEmpty() ? mVBtn.value(i)->file_name :
                   (!mVBtn.value(i)->file.isEmpty() ? mVBtn.value(i)->file :
                    (!mVBtn.value(i)->name.isEmpty() ? mVBtn.value(i)->name : mVBtn.value(i)->exec)));
        if (cmpName.isEmpty()) return false;
        if (cmpName.compare(name) == 0) return true;
    }
    return false;
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
    //set taskbar width by panel
    QSize maxSize = QSize(mPlugin->panel()->panelSize(), mPlugin->panel()->panelSize());
    QSize minSize = QSize(mPlugin->panel()->iconSize()/2, mPlugin->panel()->iconSize()/2);
    int iconsize = panel->iconSize();

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
        //group->setFixedSize(mPlugin->panel()->panelSize(), mPlugin->panel()->panelSize());
        //group->updateIcon();
        group->setIconSize(QSize(iconsize,iconsize));
    }
    for (int i = 0; i < mVBtn.size(); i++) {
        UKUITaskGroup * quicklaunch = mVBtn.value(i);
        quicklaunch->setIconSize(QSize(iconsize, iconsize));
       // quicklaunch->setFixedSize(mPlugin->panel()->panelSize(), mPlugin->panel()->panelSize());
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

void UKUITaskBar::mouseMoveEvent(QMouseEvent *e) {
    QFrame::mouseMoveEvent(e);
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

///////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/// About Quick Launch Functions
///
///

void UKUITaskBar::addButton(QuickLaunchAction* action)
{
    if (countOfButtons() && hasPlaceHolder) {
        mLayout->removeWidget(mPlaceHolder);
        hasPlaceHolder = false;
    }
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
           &&(layout()->indexOf(group) >= 0))
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
        /*
        if (countOfButtons() > 32) {
            tmpwidget->setHidden(false);
            mLayout->moveItem(mLayout->indexOf(tmpwidget), countOfButtons() - 1);
        }
        */
    }
    connect(btn, &UKUITaskButton::dragging, this, [this] (QObject * dragSource, QPoint const & pos) {
        switchButtons(qobject_cast<UKUITaskGroup *>(sender()), qobject_cast<UKUITaskGroup *>(dragSource));//, pos);
    });
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(t_saveSettings()), this, SLOT(saveSettingsSlot()));
    mLayout->setEnabled(true);
   // GetMaxPage();
    //realign();
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

    /*
    int l = qMin(n1, n2);
    int m = qMax(n1, n2);

    mLayout->moveItem(l, m, true);
    mLayout->moveItem(m-1, l, true);
    saveSettings();
    */
}

bool UKUITaskBar::checkButton(QuickLaunchAction* action)
{
    bool checkresult;
    UKUITaskGroup* btn = new UKUITaskGroup(action, mPlugin, this);
    int i = 0;
    int counts = mVBtn.size();

    /* 仅仅在快速启动栏上的应用数量大于０的时候才进行判断
     * 若在快速启动栏　应用数量为０的时候b->file_name为空
     * 会造成任务栏的崩溃
      */
    qDebug()<<"检测到目前已经固定到任务栏的应用数量 "<<counts;
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
    // GetMaxPage();
    //    btn->deleteLater();
    //realign();
    //if (countOfButtons() <= 32) tmpwidget->setHidden(true);
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }
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
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }
   // if (countOfButtons() <= 32) tmpwidget->setHidden(true);
}


// 只要任何监控的目录更新（添加、删除、重命名），就会调用。
void UKUITaskBar::directoryUpdated(const QString &path)
{
    // 比较最新的内容和保存的内容找出区别(变化)
    QStringList currEntryList = m_currentContentsMap[path];
    const QDir dir(path);
    QStringList newEntryList = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);
    QSet<QString> currentDirSet = QSet<QString>::fromList(currEntryList);

    // 添加了文件
    QSet<QString> newFiles = newDirSet - currentDirSet;
    QStringList newFile = newFiles.toList();

    // 文件已被移除
    QSet<QString> deletedFiles = currentDirSet - newDirSet;
    QStringList deleteFile = deletedFiles.toList();

    // 更新当前设置
    m_currentContentsMap[path] = newEntryList;

    if (!newFile.isEmpty() && !deleteFile.isEmpty())
    {
        // 文件/目录重命名
        if ((newFile.count() == 1) && (deleteFile.count() == 1))
        {
//            qDebug() << QString("File Renamed from %1 to %2").arg(deleteFile.first()).arg(newFile.first());
        }
    }
    else
    {
        // 添加新文件/目录至Dir
        if (!newFile.isEmpty())
        {
            foreach (QString file, newFile)
            {
                // 处理操作每个新文件....
            }
        }
        // 从Dir中删除文件/目录
        if (!deleteFile.isEmpty())
        {
            foreach(QString file, deleteFile)
            {
                // 处理操作每个被删除的文件....
                FileDeleteFromTaskbar(path+file);
            }
        }
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
           && (layout()->indexOf(pQuickBtn) >= 0 ))
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
    if (xdg.load(fileName))
    {
        /*This fuction returns true if the desktop file is applicable to the
          current environment.
          but I don't need this attributes now
        */
        //        if (xdg.isSuitable())
        addButton(new QuickLaunchAction(&xdg, this));
    }
    else if (fi.exists() && fi.isExecutable() && !fi.isDir())
    {
        addButton(new QuickLaunchAction(fileName, fileName, "", this));
    }
    else if (fi.exists())
    {
        addButton(new QuickLaunchAction(fileName, this));
    }
    else
    {
        qWarning() << "XdgDesktopFile" << fileName << "is not valid";
        QMessageBox::information(this, tr("Drop Error"),
                                 tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                                 );
    }
    saveSettings();
}

bool UKUITaskBar::AddToTaskbar(QString arg)
{
    _AddToTaskbar(arg);
    return true;
}

/*
 * @need resolved bug
 * 为开始菜单提供检测应用是否在任务栏上面的接口
 */
bool UKUITaskBar::CheckIfExist(QString arg)
{
    if(countOfButtons()>0)
    {
        const auto url=QUrl(arg);
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        XdgDesktopFile xdg;
        xdg.load(fileName);
        bool state;
        state=checkButton(new QuickLaunchAction(&xdg, this));
        return state;
    }
    return 0;
}

/*为开始菜单提供从任务栏上移除的接口*/
bool UKUITaskBar::RemoveFromTaskbar(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    XdgDesktopFile xdg;
    xdg.load(fileName);
    removeButton(new QuickLaunchAction(&xdg, this));
    return true;
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

/*为开始菜单提供从任务栏上移除文件的接口*/
void UKUITaskBar::FileDeleteFromTaskbar(QString file)
{
    int i=0;
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
   // if (countOfButtons() <= 32) tmpwidget->setHidden(true);
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }
    saveSettings();
}

/*获取任务栏位置的接口*/
int UKUITaskBar::GetPanelPosition(QString arg)
{
    return mPlugin->panel()->position();
}

/*获取任务栏高度的接口*/
int UKUITaskBar::GetPanelSize(QString arg)
{
    return mPlugin->panel()->panelSize();
}


/*快速启动栏的按钮删除 右键删除*/
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
    //GetMaxPage();
    //realign();
    if (!countOfButtons()) {
        mLayout->addWidget(mPlaceHolder);
        hasPlaceHolder = true;
    }
    saveSettings();

    /*//注释showPlaceHolder的原因是在开始菜单检测快速启动栏上面固定的应用数量的时候
      //countOfButtons无法获取快速启动栏上的应用为０的情况
    if (mLayout->isEmpty()){
        qDebug()<<"mLayout->isEmpty()"<<countOfButtons();
        showPlaceHolder();
    }
    */
}


void UKUITaskBar::saveSettingsSlot() {
    saveSettings();
}

/*保持设置*/
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


/*在快速启动栏区域没有应用的时候显示一块空白的区域用以实现拖拽等操作
*/
void UKUITaskBar::showPlaceHolder()
{

}

int UKUITaskBar::indexOfButton(UKUITaskGroup* button) const
{
    return mLayout->indexOf(button);
}

/*快速启动栏上应用的数量*/
int UKUITaskBar::countOfButtons() const
{
    return mLayout->count();
}

/*
 * Implementation of adaptor class FilectrlAdaptor
 * 为开始菜单提供D_Bus接口
 */

FilectrlAdaptor::FilectrlAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

FilectrlAdaptor::~FilectrlAdaptor()
{
    // destructor
}

/*添加到快速启动栏*/
bool FilectrlAdaptor::AddToTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddToTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*检测是否已经存在于快速启动栏*/
bool FilectrlAdaptor::CheckIfExist(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "CheckIfExist", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*从快速启动栏删除应用*/
bool FilectrlAdaptor::RemoveFromTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*从快速启动栏删除应用*/
bool FilectrlAdaptor::FileDeleteFromTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "FileDeleteFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*获取任务栏位置*/
int FilectrlAdaptor::GetPanelPosition(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelPosition", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}

/*获取任务栏高度*/
int FilectrlAdaptor::GetPanelSize(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelSize", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}

void FilectrlAdaptor::ReloadSecurityConfig()
{
    QMetaObject::invokeMethod(parent(), "ReloadSecurityConfig");
}

QString FilectrlAdaptor::GetSecurityConfigPath()
{
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetSecurityConfigPath", Q_RETURN_ARG(QString, out0));
    return out0;
}
