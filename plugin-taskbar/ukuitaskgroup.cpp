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

#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"

#include <QDebug>
#include <QMimeData>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QStringBuilder>
#include <QMenu>
#include <KF5/KWindowSystem/KWindowSystem>
#include <functional>

#include <QtX11Extras/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <QLabel>

#include <qmainwindow.h>
#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include "../panel/iukuipanelplugin.h"
#include <QSize>
#include <QScreen>
#include <XdgIcon>
#include <XdgDesktopFile>
#include "../panel/customstyle.h"

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

QPixmap qimageFromXImage(XImage* ximage)
{
    QImage::Format format = QImage::Format_ARGB32_Premultiplied;
    if (ximage->depth == 24)
        format = QImage::Format_RGB32;
    else if (ximage->depth == 16)
        format = QImage::Format_RGB16;

    QImage image = QImage(reinterpret_cast<uchar*>(ximage->data),
                          ximage->width, ximage->height,
                          ximage->bytes_per_line, format).copy();

    // 大端还是小端?
    if ((QSysInfo::ByteOrder == QSysInfo::LittleEndian && ximage->byte_order == MSBFirst)
            || (QSysInfo::ByteOrder == QSysInfo::BigEndian && ximage->byte_order == LSBFirst)) {

        for (int i = 0; i < image.height(); i++) {
            if (ximage->depth == 16) {
                ushort* p = reinterpret_cast<ushort*>(image.scanLine(i));
                ushort* end = p + image.width();
                while (p < end) {
                    *p = ((*p << 8) & 0xff00) | ((*p >> 8) & 0x00ff);
                    p++;
                }
            } else {
                uint* p = reinterpret_cast<uint*>(image.scanLine(i));
                uint* end = p + image.width();
                while (p < end) {
                    *p = ((*p << 24) & 0xff000000) | ((*p << 8) & 0x00ff0000)
                         | ((*p >> 8) & 0x0000ff00) | ((*p >> 24) & 0x000000ff);
                    p++;
                }
            }
        }
    }

    // 修复alpha通道
    if (format == QImage::Format_RGB32) {
        QRgb* p = reinterpret_cast<QRgb*>(image.bits());
        for (int y = 0; y < ximage->height; ++y) {
            for (int x = 0; x < ximage->width; ++x)
                p[x] |= 0xff000000;
            p += ximage->bytes_per_line / 4;
        }
    }
    return QPixmap::fromImage(image);
}



/************************************************

 ************************************************/
UKUITaskGroup::UKUITaskGroup(const QString &groupName, WId window, UKUITaskBar *parent)
    : UKUITaskButton(groupName,window, parent, parent),
    mGroupName(groupName),
    mPopup(new UKUIGroupPopup(this)),
    mPreventPopup(false),
    mSingleButton(true),
    mTimer(new QTimer(this)),
    mpWidget(NULL)
{
    Q_ASSERT(parent);
    mpScrollArea = NULL;
    taskgroupStatus = NORMAL;

    setObjectName(groupName);
    setText(groupName);

    initDesktopFileName(window);

    initActionsInRightButtonMenu();

    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(onDesktopChanged(int)));
    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), this, SLOT(onActiveWindowChanged(WId)));
    connect(parent, &UKUITaskBar::refreshIconGeometry, this, &UKUITaskGroup::refreshIconsGeometry);
    connect(parent, &UKUITaskBar::buttonStyleRefreshed, this, &UKUITaskGroup::setToolButtonsStyle);
    connect(parent, &UKUITaskBar::showOnlySettingChanged, this, &UKUITaskGroup::refreshVisibility);
    connect(parent, &UKUITaskBar::popupShown, this, &UKUITaskGroup::groupPopupShown);
    mTimer->setTimerType(Qt::PreciseTimer);
    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));
}

UKUITaskGroup::~UKUITaskGroup()
{
//    if(mPopup)
//    {
//        mPopup->deleteLater();
//        mPopup = NULL;
//    }
//    if(mpWidget)
//    {
//        mpWidget->deleteLater();
//        mpWidget = NULL;
//    }
//    if(VLayout)
//    {
//        VLayout->deleteLater();
//        VLayout = NULL;
//    }
}


bool DesktopFileNameCompare(QString str1, QString str2) {
    if (str1 == str2)
        return true;
    if (str2.contains(str1))
        return true;
    if (str1.contains(str2))
        return true;
    return false;
}

QString getDesktopFileName(QString cmd) {
    char name[200];
    FILE *fp1 = NULL;
    if ((fp1 = popen(cmd.toStdString().data(), "r")) == NULL)
        return QString();
    memset(name, 0, sizeof(name));
    fgets(name, sizeof(name),fp1);
    pclose(fp1);
    return QString(name);
}

void UKUITaskGroup::badBackFunctionToFindDesktop() {
    if (file_name.isEmpty()) {
        QDir dir("/usr/share/applications/");
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); i++) {
            QFileInfo fileInfo = list.at(i);
            if (parentTaskBar()->ignoreSymbolCMP(fileInfo.filePath(), groupName())) {
                file_name = fileInfo.filePath();
                if (file_name == QString(PEONY_COMUTER) ||
                    file_name == QString(PEONY_TRASH) ||
                    file_name == QString(PEONY_HOME))
                    file_name = QString(PEONY_MAIN);
                break;
            }
        }
    }
}

void UKUITaskGroup::initDesktopFileName(WId window) {
    KWindowInfo info(window, 0, NET::WM2DesktopFileName);
    QString cmd;
    cmd.sprintf(GET_PROCESS_EXEC_NAME_MAIN, info.pid());
    QString processExeName = getDesktopFileName(cmd);

    QDir dir(DEKSTOP_FILE_PATH);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        bool flag = false;
        QFileInfo fileInfo = list.at(i);
        QString _cmd;
        if (fileInfo.filePath() == QString(USR_SHARE_APP_CURRENT) ||
            fileInfo.filePath() == QString(USR_SHARE_APP_UPER) )
            continue;
        _cmd.sprintf(GET_DESKTOP_EXEC_NAME_MAIN, fileInfo.filePath().toStdString().data());
        QString desktopFileExeName = getDesktopFileName(_cmd);
        flag = DesktopFileNameCompare(desktopFileExeName, processExeName);
        if (flag && !desktopFileExeName.isEmpty()) {
            file_name = fileInfo.filePath();
            if (file_name == QString(PEONY_COMUTER) ||
                file_name == QString(PEONY_TRASH) ||
                file_name == QString(PEONY_HOME) )
                file_name = QString(PEONY_MAIN);
            break;
        }
    }
    if (file_name.isEmpty()) {
        for (int i = 0; i < list.size(); i++) {
            bool flag = false;
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.filePath() == QString(USR_SHARE_APP_CURRENT) ||
                fileInfo.filePath() == QString(USR_SHARE_APP_UPER) )
                continue;
            QString _cmd;
            _cmd.sprintf(GET_DESKTOP_EXEC_NAME_BACK, fileInfo.filePath().toStdString().data());
            QString desktopFileExeName = getDesktopFileName(_cmd);
            flag = DesktopFileNameCompare(desktopFileExeName, processExeName);
            if (flag && !desktopFileExeName.isEmpty()) {
                file_name = fileInfo.filePath();
                if (file_name == QString(PEONY_COMUTER) ||
                    file_name == QString(PEONY_TRASH) ||
                    file_name == QString(PEONY_HOME) )
                    file_name = QString(PEONY_MAIN);
                break;
            }
        }
    }
    badBackFunctionToFindDesktop();
}

void UKUITaskGroup::initActionsInRightButtonMenu(){
    if (file_name.isEmpty()) return;
    const auto url=QUrl(file_name);
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
        mAct = new QuickLaunchAction(&xdg, this);
    }
    else if (fi.exists() && fi.isExecutable() && !fi.isDir())
    {
        mAct = new QuickLaunchAction(fileName, fileName, "", this);
    }
    else if (fi.exists())
    {
        mAct = new QuickLaunchAction(fileName, this);
    }
    setGroupIcon(mAct->getIconfromAction());
}

/************************************************

 ************************************************/
void UKUITaskGroup::contextMenuEvent(QContextMenuEvent *event)
{
    setPopupVisible(false, true);
    mPreventPopup = true;
    if (mSingleButton)
    {
        UKUITaskButton::contextMenuEvent(event);
        return;
    }

    QMenu * menu = new QMenu(tr("Group"));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction *close = menu->addAction(QIcon::fromTheme("window-close-symbolic"), tr("close"));
    connect(close, SIGNAL(triggered()), this, SLOT(closeGroup()));
    connect(menu, &QMenu::aboutToHide, [this] {
        mPreventPopup = false;
    });
    menu->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
    plugin()->willShowWindow(menu);
    menu->show();
}

/************************************************

 ************************************************/
void UKUITaskGroup::closeGroup()
{
    //To Do
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(auto it=mButtonHash.begin();it!=mButtonHash.end();it++)
    {
    UKUITaskWidget *button =it.value();
  if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->closeApplication();
    }
#else
    for (UKUITaskWidget *button : qAsConst(mButtonHash) )
        if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->minimizeApplication();
    for (UKUITaskWidget *button : qAsConst(mButtonHash) )
        if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->closeApplication();
#endif
}

/************************************************

 ************************************************/
QWidget * UKUITaskGroup::addWindow(WId id)
{

    if (mButtonHash.contains(id))
        return mButtonHash.value(id);
    UKUITaskWidget *btn = new UKUITaskWidget(id, parentTaskBar(), mPopup);
    mButtonHash.insert(id, btn);
    connect(btn, SIGNAL(clicked()), this, SLOT(onChildButtonClicked()));
    connect(btn, SIGNAL(windowMaximize()), this, SLOT(onChildButtonClicked()));
    refreshVisibility();

    changeTaskButtonStyle();
    return btn;
}

/************************************************

 ************************************************/
QWidget * UKUITaskGroup::checkedButton() const
{
//    for (QWidget* button : qAsConst(mButtonHash))
//        if (button->isChecked())
//            return button;

    return NULL;
}

/*changeTaskButtonStyle in class UKUITaskGroup not class UKUITaskButton
 * because class UKUITaskButton can not get mButtonHash.size
 */
void UKUITaskGroup::changeTaskButtonStyle()
{
    if(mVisibleHash.size()>1)
        this->setStyle(new CustomStyle("taskbutton",true));
    else
        this->setStyle(new CustomStyle("taskbutton",false));
}
/************************************************

 ************************************************/
QWidget * UKUITaskGroup::getNextPrevChildButton(bool next, bool circular)
{
#if 0
    QWidget *button = checkedButton();
    int idx = mPopup->indexOf(button);
    int inc = next ? 1 : -1;
    idx += inc;

    // if there is no cheked button, get the first one if next equals true
    // or the last one if not
    if (!button)
    {
        idx = -1;
        if (next)
        {
            for (int i = 0; i < mPopup->count() && idx == -1; i++)
                if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
                    idx = i;
        }
        else
        {
            for (int i = mPopup->count() - 1; i >= 0 && idx == -1; i--)
                if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
                    idx = i;
        }
    }

    if (circular)
        idx = (idx + mButtonHash.count()) % mButtonHash.count();
    else if (mPopup->count() <= idx || idx < 0)
        return NULL;

    // return the next or the previous child
    QLayoutItem *item = mPopup->itemAt(idx);
    if (item)
    {
        button = qobject_cast<UKUITaskButton*>(item->widget());
        if (button->isVisibleTo(mPopup))
            return button;
    }

#endif
     return NULL;
}

/************************************************

 ************************************************/
void UKUITaskGroup::onActiveWindowChanged(WId window)
{
    UKUITaskWidget *button = mButtonHash.value(window, nullptr);
//    for (QWidget *btn : qAsConst(mButtonHash))
//        btn->setChecked(false);

//    if (button)
//    {
//        button->setChecked(true);
//        if (button->hasUrgencyHint())
//            button->setUrgencyHint(false);
//    }
    setChecked(nullptr != button);
}

/************************************************

 ************************************************/
void UKUITaskGroup::onDesktopChanged(int number)
{
    refreshVisibility();
    changeTaskButtonStyle();
}

/************************************************

 ************************************************/
void UKUITaskGroup::onWindowRemoved(WId window)
{
    if (mButtonHash.contains(window))
    {
        UKUITaskWidget *button = mButtonHash.value(window);
        mButtonHash.remove(window);
        if (mVisibleHash.contains(window))
            mVisibleHash.remove(window);
        mPopup->removeWidget(button);
        button->deleteLater();
        if (!parentTaskBar()->getCpuInfoFlg())
            system(QString("rm -f /tmp/%1.png").arg(window).toLatin1());
        if (isLeaderWindow(window))
            setLeaderWindow(mButtonHash.begin().key());
        if (mButtonHash.count())
        {
            if(mPopup->isVisible())
            {
//                mPopup->hide(true);
                showPreview();
            }
            else
            {
                regroup();
            }
        }
        else
        {
            if (isVisible())
                emit visibilityChanged(false);
            hide();
            emit groupBecomeEmpty(groupName());

        }
        changeTaskButtonStyle();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::onChildButtonClicked()
{
    setPopupVisible(false, true);
    parentTaskBar()->setShowGroupOnHover(true);
    //QToolButton::leaveEvent(event);
    taskgroupStatus = NORMAL;
    update();
}

/************************************************

 ************************************************/
Qt::ToolButtonStyle UKUITaskGroup::popupButtonStyle() const
{
    // do not set icons-only style in the buttons in the group,
    // as they'll be indistinguishable
    const Qt::ToolButtonStyle style = toolButtonStyle();
    return style == Qt::ToolButtonIconOnly ? Qt::ToolButtonTextBesideIcon : style;
}

/************************************************

 ************************************************/
void UKUITaskGroup::setToolButtonsStyle(Qt::ToolButtonStyle style)
{
    setToolButtonStyle(style);

//    const Qt::ToolButtonStyle styleInPopup = popupButtonStyle();
//    for (auto & button : mButtonHash)
//    {
//        button->setToolButtonStyle(styleInPopup);
//    }
}

/************************************************

 ************************************************/
int UKUITaskGroup::buttonsCount() const
{
    return mButtonHash.count();
}

void UKUITaskGroup::initVisibleHash()
{
   /* for (UKUITaskButtonHash::const_iterator it = mButtonHash.begin();it != mButtonHash.end();it++)
    {
        if (mVisibleHash.contains(it.key())) continue;
        if (it.value()->isVisibleTo(mPopup))
            mVisibleHash.insert(it.key(), it.value());
    }*/
}

/************************************************

 ************************************************/
int UKUITaskGroup::visibleButtonsCount() const
{
    int i = 0;
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for (auto it=mButtonHash.begin();it!=mButtonHash.end();it++)
     {
        UKUITaskWidget *btn=it.value();
        if (btn->isVisibleTo(mPopup))
            i++;
    }
#else
    for (UKUITaskWidget *btn : qAsConst(mButtonHash))
        if (btn->isVisibleTo(mPopup))
            i++;
#endif
    return i;
}

/************************************************

 ************************************************/
void UKUITaskGroup::draggingTimerTimeout()
{
    if (mSingleButton)
        setPopupVisible(false);
}

/************************************************

 ************************************************/
void UKUITaskGroup::onClicked(bool)
{
    if (1 == mVisibleHash.size())
    {
        singleWindowClick();
        return;
    }
    if(mPopup->isVisible())
    {
        if(HOVER == taskgroupStatus)
        {
            taskgroupStatus = NORMAL;
            return;
        }
        else
        {
            mPopup->hide();
            return;
        }
    }
    else
    {
        showPreview();
    }
    mTaskGroupEvent = OTHEREVENT;
    if(mTimer->isActive())
    {
       mTimer->stop();
    }
}


void UKUITaskGroup::singleWindowClick()
{
    UKUITaskWidget *btn = mVisibleHash.value(windowId());
    if(btn)
    {
        if(!btn->isFocusState())
        {
            if(mPopup->isVisible())
            {
                mPopup->hide();
            }
            KWindowSystem::activateWindow(windowId());
        }
        else
        {
            btn->minimizeApplication();
            if(mPopup->isVisible())
            {
                mPopup->hide();
            }
        }
    }
    mTaskGroupEvent = OTHEREVENT;
    if(mTimer->isActive())
    {
       mTimer->stop();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::regroup()
{
    int cont = visibleButtonsCount();
    recalculateFrameIfVisible();

//    if (cont == 1)
//    {
//        mSingleButton = false;
//        // Get first visible button
//        UKUITaskButton * button = NULL;
//        for (UKUITaskButton *btn : qAsConst(mButtonHash))
//        {
//            if (btn->isVisibleTo(mPopup))
//            {
//                button = btn;
//                break;
//            }
//        }

//        if (button)
//        {
//            setText(button->text());
//            setToolTip(button->toolTip());
//            setWindowId(button->windowId());

//        }
//    }
    /*else*/ if (cont == 0)
        hide();
    else
    {
        mSingleButton = false;
        QString t = QString("%1 - %2 windows").arg(mGroupName).arg(cont);
        setText(t);
        setToolTip(parentTaskBar()->isShowGroupOnHover() ? QString() : t);
    }

}

/************************************************

 ************************************************/
void UKUITaskGroup::recalculateFrameIfVisible()
{
    if (mPopup->isVisible())
    {
        recalculateFrameSize();
        if (plugin()->panel()->position() == IUKUIPanel::PositionBottom)
            recalculateFramePosition();
    }
}

/************************************************

 ************************************************/
void UKUITaskGroup::setAutoRotation(bool value, IUKUIPanel::Position position)
{
//    for (QWidget *button : qAsConst(mButtonHash))
//        button->setAutoRotation(false, position);

    //UKUITaskWidget::setAutoRotation(value, position);
}

/************************************************

 ************************************************/
void UKUITaskGroup::refreshVisibility()
{
    bool will = false;
    UKUITaskBar const * taskbar = parentTaskBar();
    const int showDesktop = taskbar->showDesktopNum();

    for(UKUITaskButtonHash::const_iterator i=mButtonHash.begin();i!=mButtonHash.end();i++)
    {
        UKUITaskWidget * btn=i.value();
        bool visible = taskbar->isShowOnlyOneDesktopTasks() ? btn->isOnDesktop(0 == showDesktop ? KWindowSystem::currentDesktop() : showDesktop) : true;
        visible &= taskbar->isShowOnlyCurrentScreenTasks() ? btn->isOnCurrentScreen() : true;
        visible &= taskbar->isShowOnlyMinimizedTasks() ? btn->isMinimized() : true;
        btn->setVisible(visible);
        if (btn->isVisibleTo(mPopup) && !mVisibleHash.contains(i.key()))
            mVisibleHash.insert(i.key(), i.value());
        else if (!btn->isVisibleTo(mPopup) && mVisibleHash.contains(i.key()))
            mVisibleHash.remove(i.key());
        will |= visible;
    }
    setLeaderWindow(mVisibleHash.begin().key());
    bool is = isVisible();
    setVisible(will);
    if(!mPopup->isVisible())
    {
        regroup();
    }

    if (is != will)
        emit visibilityChanged(will);
}

/************************************************

 ************************************************/
QMimeData * UKUITaskGroup::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << groupName();
    mimedata->setData(mimeDataFormat(), byteArray);
    return mimedata;
}

/************************************************

 ************************************************/
void UKUITaskGroup::setPopupVisible(bool visible, bool fast)
{
    if (visible && !mPreventPopup && !mSingleButton)
    {
//        QTimer::singleShot(400, this,SLOT(showPreview()));
        showPreview();
        /* for origin preview
        plugin()->willShowWindow(mPopup);
        mPopup->show();
        qDebug()<<"setPopupVisible ********";
        emit popupShown(this);*/
    }
    else
        mPopup->hide(fast);
}
/************************************************

 ************************************************/
void UKUITaskGroup::refreshIconsGeometry()
{
    QRect rect = geometry();
    rect.moveTo(mapToGlobal(QPoint(0, 0)));
    if (mSingleButton)
    {
        refreshIconGeometry(rect);
        return;
    }

//    for(UKUITaskButton *but : qAsConst(mButtonHash))
//    {
//        but->refreshIconGeometry(rect);
//        but->setIconSize(QSize(plugin()->panel()->iconSize(), plugin()->panel()->iconSize()));
//    }
}

/************************************************

 ************************************************/
QSize UKUITaskGroup::recalculateFrameSize()
{
    int height = recalculateFrameHeight();
    mPopup->setMaximumHeight(1000);
    mPopup->setMinimumHeight(0);

    int hh = recalculateFrameWidth();
    mPopup->setMaximumWidth(hh);
    mPopup->setMinimumWidth(0);

    QSize newSize(hh, height);
    mPopup->resize(newSize);

    return newSize;
}

/************************************************

 ************************************************/
int UKUITaskGroup::recalculateFrameHeight() const
{
//    int cont = visibleButtonsCount();
//    int h = !plugin()->panel()->isHorizontal() && parentTaskBar()->isAutoRotate() ? width() : height();
//    return cont * h + (cont + 1) * mPopup->spacing();
    return 120;
}

/************************************************

 ************************************************/
int UKUITaskGroup::recalculateFrameWidth() const
{
    const QFontMetrics fm = fontMetrics();
    int max = 100 * fm.width (' '); // elide after the max width
    int txtWidth = 0;
//    for (UKUITaskButton *btn : qAsConst(mButtonHash))
//        txtWidth = qMax(fm.width(btn->text()), txtWidth);
    return iconSize().width() + qMin(txtWidth, max) + 30/* give enough room to margins and borders*/;

}

/************************************************

 ************************************************/
QPoint UKUITaskGroup::recalculateFramePosition()
{
    // Set position
    int x_offset = 0, y_offset = 0;
    switch (plugin()->panel()->position())
    {
    case IUKUIPanel::PositionTop:
        y_offset += height();
        break;
    case IUKUIPanel::PositionBottom:
        y_offset = -recalculateFrameHeight();
        break;
    case IUKUIPanel::PositionLeft:
        x_offset += width();
        break;
    case IUKUIPanel::PositionRight:
        x_offset = -recalculateFrameWidth();
        break;
    }

    QPoint pos = mapToGlobal(QPoint(x_offset, y_offset));
    mPopup->move(pos);

    return pos;
}

/************************************************

 ************************************************/
void UKUITaskGroup::leaveEvent(QEvent *event)
{
    //QTimer::singleShot(300, this,SLOT(mouseLeaveOut()));
    mTaskGroupEvent = LEAVEEVENT;
    mEvent = event;
    if(mTimer->isActive())
    {
        mTimer->stop();//stay time is no more than 400 ms need kill timer
    }
    else
    {
        mTimer->start(300);
    }
}
/************************************************

 ************************************************/
void UKUITaskGroup::enterEvent(QEvent *event)
{
    //QToolButton::enterEvent(event);
    mTaskGroupEvent = ENTEREVENT;
    mEvent = event;
    mTimer->start(400);
//    if (sDraggging)
//        return;
//    if (parentTaskBar()->isShowGroupOnHover())
//    {
//        setPopupVisible(true);
//        parentTaskBar()->setShowGroupOnHover(false);//enter this group other groups will be blocked
//    }
//    taskgroupStatus = HOVER;
//    repaint();
}

void UKUITaskGroup::handleSavedEvent()
{
    if (sDraggging)
        return;
    if (parentTaskBar()->isShowGroupOnHover())
    {
        setPopupVisible(true);
    }
    taskgroupStatus = HOVER;
    repaint();
    QToolButton::enterEvent(mEvent);
}

//void UKUITaskGroup::paintEvent(QPaintEvent *)
//{
//}
/************************************************

 ************************************************/
void UKUITaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    // only show the popup if we aren't dragging a taskgroup
    if (!event->mimeData()->hasFormat(mimeDataFormat()))
    {
        setPopupVisible(true);
    }
    UKUITaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskGroup::dragLeaveEvent(QDragLeaveEvent *event)
{
    // if draggind something into the taskgroup or the taskgroups' popup,
    // do not close the popup
    if (!sDraggging)
        setPopupVisible(false);
    UKUITaskButton::dragLeaveEvent(event);
}

void UKUITaskGroup::mouseMoveEvent(QMouseEvent* event)
{
    // if dragging the taskgroup, do not show the popup
    setPopupVisible(false, true);
    UKUITaskButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
bool UKUITaskGroup::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{ // returns true if the class is preserved
    bool needsRefreshVisibility{false};
    QVector<QWidget *> buttons;
    if (mButtonHash.contains(window))
        buttons.append(mButtonHash.value(window));

    // If group is based on that window properties must be changed also on button group
    if (window == windowId())
        buttons.append(this);

    if (!buttons.isEmpty())
    {
        // if class is changed the window won't belong to our group any more
        if (parentTaskBar()->isGroupingEnabled() && prop2.testFlag(NET::WM2WindowClass))
        {
            KWindowInfo info(window, 0, NET::WM2WindowClass);
            if (info.windowClassClass() != mGroupName)
            {
                onWindowRemoved(window);
                return false;
            }
        }
        // window changed virtual desktop
        if (prop.testFlag(NET::WMDesktop) || prop.testFlag(NET::WMGeometry))
        {
            if (parentTaskBar()->isShowOnlyOneDesktopTasks()
                    || parentTaskBar()->isShowOnlyCurrentScreenTasks())
            {
                needsRefreshVisibility = true;
            }
        }

//        if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
//            std::for_each(buttons.begin(), buttons.end(), std::mem_fn(&UKUITaskButton::updateText));

        // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
        // Icon of the button can be based on windowClass
        if (prop.testFlag(NET::WMIcon) || prop2.testFlag(NET::WM2WindowClass)){
            updateIcon();
            for(UKUITaskButtonHash::const_iterator i=mVisibleHash.begin();i!= mVisibleHash.end();i++)
                i.value()->updateIcon();
        }

        if (prop.testFlag(NET::WMState))
        {
            KWindowInfo info{window, NET::WMState};
            if (info.hasState(NET::SkipTaskbar))
                onWindowRemoved(window);
//            std::for_each(buttons.begin(), buttons.end(), std::bind(&UKUITaskButton::setUrgencyHint, std::placeholders::_1, info.hasState(NET::DemandsAttention)));

            if (parentTaskBar()->isShowOnlyMinimizedTasks())
            {
                needsRefreshVisibility = true;
            }
        }
    }

    if (needsRefreshVisibility)
        refreshVisibility();

    return true;
}

/************************************************

 ************************************************/
void UKUITaskGroup::groupPopupShown(UKUITaskGroup * const sender)
{
    //close all popups (should they be visible because of close delay)
    if (this != sender && isVisible())
            setPopupVisible(false, true/*fast*/);
}

void UKUITaskGroup::removeWidget()
{
    if(mpScrollArea)
    {
        removeSrollWidget();
    }
    if(mpWidget)
    {
        mPopup->layout()->removeWidget(mpWidget);
        QHBoxLayout *hLayout = dynamic_cast<QHBoxLayout*>(mpWidget->layout());
        QVBoxLayout *vLayout = dynamic_cast<QVBoxLayout*>(mpWidget->layout());
        if(hLayout != NULL)
        {
            hLayout->deleteLater();
            hLayout = NULL;
        }
        if(vLayout != NULL)
        {
            vLayout->deleteLater();
            vLayout = NULL;
        }
        //mpWidget->setParent(NULL);
        mpWidget->deleteLater();
        mpWidget = NULL;
    }
}


void UKUITaskGroup::removeSrollWidget()
{
    if(mpScrollArea)
    {
        mPopup->layout()->removeWidget(mpScrollArea);
        mPopup->layout()->removeWidget(mpScrollArea->takeWidget());
    }
    if(mpWidget)
    {
        mPopup->layout()->removeWidget(mpWidget);
        QHBoxLayout *hLayout = dynamic_cast<QHBoxLayout*>(mpWidget->layout());
        QVBoxLayout *vLayout = dynamic_cast<QVBoxLayout*>(mpWidget->layout());
        if(hLayout != NULL)
        {
            hLayout->deleteLater();
            hLayout = NULL;
        }
        if(vLayout != NULL)
        {
            vLayout->deleteLater();
            vLayout = NULL;
        }
        //mpWidget->setParent(NULL);
        mpWidget->deleteLater();
        mpWidget = NULL;
    }
    if(mpScrollArea)
    {
        mpScrollArea->deleteLater();
        mpScrollArea = NULL;
    }

}

void UKUITaskGroup::setLayOutForPostion()
{
    if(mVisibleHash.size() > 10)//more than 10 need
    {
        mpWidget->setLayout(new QVBoxLayout);
        mpWidget->layout()->setAlignment(Qt::AlignTop);
        mpWidget->layout()->setSpacing(3);
        mpWidget->layout()->setMargin(3);
        return;
    }

    if(plugin()->panel()->isHorizontal())
    {
        mpWidget->setLayout(new QHBoxLayout);
        mpWidget->layout()->setSpacing(3);
        mpWidget->layout()->setMargin(3);
    }
    else
    {
        mpWidget->setLayout(new QVBoxLayout);
        mpWidget->layout()->setSpacing(3);
        mpWidget->layout()->setMargin(3);
    }
}

bool UKUITaskGroup::isSetMaxWindow()
{
    int iScreenWidth = QApplication::screens().at(0)->size().width();
    int iScreenHeight = QApplication::screens().at(0)->size().height();
    if((iScreenWidth >= SCREEN_MID_WIDTH_SIZE)||((iScreenWidth > SCREEN_MAX_WIDTH_SIZE) && (iScreenHeight > SCREEN_MAX_HEIGHT_SIZE)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UKUITaskGroup::showPreview()
{
    int n = 7;
    if (plugin()->panel()->isHorizontal()) n = 10;
    if(mVisibleHash.size() <= n)
    {
        showAllWindowByThumbnail();
    }
    else
    {
        showAllWindowByList();
    }
}

void UKUITaskGroup::adjustPopWindowSize(int winWidth, int winHeight)
{
    int size = mVisibleHash.size();
    if(plugin()->panel()->isHorizontal())
    {
        mPopup->setFixedSize(winWidth*size + (size + 1)*3, winHeight + 6);
    }
    else
    {
        mPopup->setFixedSize(winWidth + 6,winHeight*size + (size + 1)*3);
    }
    mPopup->adjustSize();
}

void UKUITaskGroup::v_adjustPopWindowSize(int winWidth, int winHeight, int v_all)
{
    int fixed_size = v_all;

    if(plugin()->panel()->isHorizontal())
    {
        int iScreenWidth = QApplication::screens().at(0)->size().width();
        if (fixed_size > iScreenWidth)
            fixed_size = iScreenWidth;
        mPopup->setFixedSize(fixed_size,  winHeight + 6);
    }
    else
    {
        int iScreenHeight = QApplication::screens().at(0)->size().height();
        if (fixed_size > iScreenHeight)
            fixed_size = iScreenHeight;
        mPopup->setFixedSize(winWidth + 6, fixed_size);
    }
    mPopup->adjustSize();
}

void UKUITaskGroup::timeout()
{
    if(mTaskGroupEvent == ENTEREVENT)
    {
        if(mTimer->isActive())
        {
            mTimer->stop();
        }
        handleSavedEvent();
    }
    else if(mTaskGroupEvent == LEAVEEVENT)
    {
        if(mTimer->isActive())
        {
            mTimer->stop();
        }
        setPopupVisible(false);
        QToolButton::leaveEvent(mEvent);
        taskgroupStatus = NORMAL;
        update();
    }
    else
    {
        setPopupVisible(false);
    }
}

int UKUITaskGroup::calcAverageHeight()
{
    if(plugin()->panel()->isHorizontal())
    {
        return 0;
    }
    else
    {
        int size = mVisibleHash.size();
        int iScreenHeight = QApplication::screens().at(0)->size().height();
        int iMarginHeight = (size+1)*3;
        int iAverageHeight = (iScreenHeight - iMarginHeight)/size;//calculate average width of window
        return iAverageHeight;
    }
}

int UKUITaskGroup::calcAverageWidth()
{
    if(plugin()->panel()->isHorizontal())
    {
        int size = mVisibleHash.size();
        int iScreenWidth = QApplication::screens().at(0)->size().width();
        int iMarginWidth = (size+1)*3;
        int iAverageWidth;
        iAverageWidth = (size == 0 ? size : (iScreenWidth - iMarginWidth)/size);//calculate average width of window
        return iAverageWidth;
    }
    else
    {
        return 0;
    }
}


void UKUITaskGroup::showAllWindowByList()
{
    int winWidth = 246;
    int winheight = 46;
    int iPreviewPosition = 0;
    int popWindowheight = (winheight) * (mVisibleHash.size());
    int screenAvailabelHeight = QApplication::screens().at(0)->size().height() - plugin()->panel()->panelSize();
    if(!plugin()->panel()->isHorizontal())
    {
        screenAvailabelHeight = QApplication::screens().at(0)->size().height();//panel is vect
    }
    if(mPopup->layout()->count() > 0)
    {
        removeSrollWidget();
    }
    mpScrollArea = new QScrollArea(this);
    mpScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mpScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //mpScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpScrollArea->setWidgetResizable(true);
    mpScrollArea->setFrameStyle(QFrame::NoFrame);

    mPopup->layout()->addWidget(mpScrollArea);
    mPopup->setFixedSize(winWidth,  popWindowheight < screenAvailabelHeight? popWindowheight : screenAvailabelHeight);
    mpWidget = new QWidget(this);
    mpScrollArea->setWidget(mpWidget);
    setLayOutForPostion();

    /*begin catch preview picture*/
    for (UKUITaskButtonHash::const_iterator it = mVisibleHash.begin();it != mVisibleHash.end();it++)
    {
        UKUITaskWidget *btn = it.value();
        btn->clearMask();
        btn->setTitleFixedWidth(mpWidget->width() - 25);
        btn->setParent(mpScrollArea);
        btn->removeThumbNail();
        btn->addThumbNail();
        btn->adjustSize();
        btn->setFixedHeight(winheight);

        connect(btn, &UKUITaskWidget::closeSigtoPop, [this] { mPopup->pubcloseWindowDelay(); });
        connect(btn, &UKUITaskWidget::closeSigtoGroup, [this] { closeGroup(); });
        mpWidget->layout()->addWidget(btn);
    }
    /*end*/
    plugin()->willShowWindow(mPopup);
    if(plugin()->panel()->isHorizontal())
    {
        iPreviewPosition =  plugin()->panel()->panelSize()/2 - winWidth/2;
        mPopup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(iPreviewPosition,0)), mPopup->size()));
    }
    else
    {
        iPreviewPosition = plugin()->panel()->panelSize()/2 - winWidth/2;
        mPopup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,iPreviewPosition)), mPopup->size()));
    }
    mpScrollArea->show();
    mPopup->show();
//   emit popupShown(this);
}


void UKUITaskGroup::showAllWindowByThumbnail()
{
    XImage *img = NULL;
    Display *display = NULL;
    QPixmap thumbnail;
    XWindowAttributes attr;
    int previewPosition = 0;
    int winWidth = 0;
    int winHeight = 0;
    int truewidth = 0;
   // initVisibleHash();
    refreshVisibility();
    int iAverageWidth = calcAverageWidth();
    int iAverageHeight = calcAverageHeight();
    /*begin get the winsize*/
    bool isMaxWinSize = isSetMaxWindow();
    if(isMaxWinSize)
    {
        if(0 == iAverageWidth)
        {
            winHeight = PREVIEW_WIDGET_MAX_HEIGHT < iAverageHeight?PREVIEW_WIDGET_MAX_HEIGHT:iAverageHeight;
            winWidth = winHeight*PREVIEW_WIDGET_MAX_WIDTH/PREVIEW_WIDGET_MAX_HEIGHT;
        }
        else
        {
            winWidth = PREVIEW_WIDGET_MAX_WIDTH < iAverageWidth?PREVIEW_WIDGET_MAX_WIDTH:iAverageWidth;
            winHeight = winWidth*PREVIEW_WIDGET_MAX_HEIGHT/PREVIEW_WIDGET_MAX_WIDTH;
        }
    }
    else
    {
        if(0 == iAverageWidth)
        {
            winHeight = PREVIEW_WIDGET_MIN_HEIGHT < iAverageHeight?PREVIEW_WIDGET_MIN_HEIGHT:iAverageHeight;
            winWidth = winHeight*PREVIEW_WIDGET_MIN_WIDTH/PREVIEW_WIDGET_MIN_HEIGHT;
        }
        else
        {
            winWidth = PREVIEW_WIDGET_MIN_WIDTH < iAverageWidth?PREVIEW_WIDGET_MIN_WIDTH:iAverageWidth;
            winHeight = winWidth*PREVIEW_WIDGET_MIN_HEIGHT/PREVIEW_WIDGET_MIN_WIDTH;
        }
    }
    /*end get the winsize*/

    if(mPopup->layout()->count() > 0)
    {
        removeWidget();
    }
    mpWidget = new QWidget;
    mpWidget->setAttribute(Qt::WA_TranslucentBackground);
    setLayOutForPostion();
    /*begin catch preview picture*/

    int max_Height = 0;
    int max_Width = 0;
    int imgWidth_sum = 0;
    int changed = 0;
    int title_width = 0;
    int v_all = 0;
    int iScreenWidth = QApplication::screens().at(0)->size().width();
    float minimumHeight = THUMBNAIL_HEIGHT;
    int allwidth = winWidth * mVisibleHash.size();
    for (UKUITaskButtonHash::const_iterator it = mVisibleHash.begin();it != mVisibleHash.end();it++)
    {
        it.value()->removeThumbNail();
        display = XOpenDisplay(nullptr);
        XGetWindowAttributes(display, it.key(), &attr);
        max_Height = attr.height > max_Height ? attr.height : max_Height;
        max_Width = attr.width > max_Width ? attr.width : max_Width;
        truewidth += attr.width;
        if(display)
            XCloseDisplay(display);
    }

    for (UKUITaskButtonHash::const_iterator it = mVisibleHash.begin();it != mVisibleHash.end();it++)
    {
        UKUITaskWidget *btn = it.value();
        btn->setParent(mPopup);
        connect(btn, &UKUITaskWidget::closeSigtoPop, [this] { mPopup->pubcloseWindowDelay(); });
        connect(btn, &UKUITaskWidget::closeSigtoGroup, [this] { closeGroup(); });
        btn->addThumbNail();
        display = XOpenDisplay(nullptr);
        XGetWindowAttributes(display, it.key(), &attr);
        img = XGetImage(display, it.key(), 0, 0, attr.width, attr.height, 0xffffffff,ZPixmap);
        float imgWidth = 0;
        float imgHeight = 0;
        if (plugin()->panel()->isHorizontal()) {
            if (mVisibleHash.size() == 1)
                imgWidth = THUMBNAIL_WIDTH;
            else
                imgWidth = allwidth * (float)attr.width/(float)truewidth ;
            imgHeight = THUMBNAIL_HEIGHT;
        } else {
            imgWidth = THUMBNAIL_WIDTH;
            imgHeight = (float)attr.height / (float)attr.width * THUMBNAIL_WIDTH;
        }
        if (plugin()->panel()->isHorizontal())
        {
            if (mVisibleHash.contains(btn->windowId())) {
                v_all += (int)imgWidth;
                imgWidth_sum += (int)imgWidth;
            }
            if (mVisibleHash.size() == 1 ) {
                changed = (int)imgWidth;
            }
            btn->setThumbMaximumSize(MAX_SIZE_OF_Thumb);
            btn->setThumbScale(true);
        } else {
            if (attr.width != max_Width)
            {
                float tmp = (float)attr.width / (float)max_Width;
                imgWidth =  imgWidth * tmp;
            }
            if ((int)imgHeight > (int)minimumHeight)
            {
                imgHeight = minimumHeight;
            }
            if (mVisibleHash.contains(btn->windowId())) {
                v_all += (int)imgHeight;
            }
            if (mVisibleHash.size() == 1 ) changed = (int)imgHeight;
            if ((int)imgWidth < 150)
            {
                btn->setThumbFixedSize((int)imgWidth);
                btn->setThumbScale(false);
            } else {
                btn->setThumbMaximumSize(MAX_SIZE_OF_Thumb);
                btn->setThumbScale(true);
            }
        }
        if(img)
        {
            thumbnail = qimageFromXImage(img).scaled((int)imgWidth, (int)imgHeight, Qt::KeepAspectRatio,Qt::SmoothTransformation);
            if (!parentTaskBar()->getCpuInfoFlg()) thumbnail.save(QString("/tmp/%1.png").arg(it.key()));
        }
        else
        {
            qDebug()<<"can not catch picture";
            QPixmap pxmp;
            if (pxmp.load(QString("/tmp/%1.png").arg(it.key())))
                thumbnail = pxmp.scaled((int)imgWidth, (int)imgHeight, Qt::KeepAspectRatio,Qt::SmoothTransformation);
            else {
                thumbnail = QPixmap((int)imgWidth, (int)imgHeight);
                thumbnail.fill(QColor(0, 0, 0, 127));
            }
        }
        btn->setThumbNail(thumbnail);
        btn->updateTitle();
        btn->setFixedSize((int)imgWidth, (int)imgHeight);
        mpWidget->layout()->setContentsMargins(0,0,0,0);
        mpWidget->layout()->addWidget(btn);
        if(img)
        {
           XDestroyImage(img);
        }
        if(display)
        {
            XCloseDisplay(display);
        }
    }
    /*end*/
        for (UKUITaskButtonHash::const_iterator it = mButtonHash.begin();it != mButtonHash.end();it++)
        {
            UKUITaskWidget *btn = it.value();
            if (plugin()->panel()->isHorizontal())  {
                if (imgWidth_sum > iScreenWidth)
                    title_width = (int)(btn->width()  * iScreenWidth / imgWidth_sum - 80);
                else
                    title_width = btn->width() - 75;
            } else {
                 title_width = winWidth- 70;
            }
            btn->setTitleFixedWidth(title_width);
        }
    plugin()->willShowWindow(mPopup);
    mPopup->layout()->addWidget(mpWidget);
    if (mVisibleHash.size() == 1 && changed != 0)
        if (plugin()->panel()->isHorizontal()) {
            adjustPopWindowSize(changed, winHeight);
        } else {
            adjustPopWindowSize(winWidth, changed);
        }
    else if (mVisibleHash.size() != 1)
        v_adjustPopWindowSize(winWidth, winHeight, v_all);
    else
        adjustPopWindowSize(winWidth, winHeight);

    if(plugin()->panel()->isHorizontal())//set preview window position
    {
        if(mPopup->size().width()/2 < QCursor::pos().x())
        {
            previewPosition = 0 - mPopup->size().width()/2 + plugin()->panel()->panelSize()/2;
        }
        else
        {
            previewPosition = 0 -(QCursor::pos().x() + plugin()->panel()->panelSize()/2);
        }
        mPopup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(previewPosition,0)), mPopup->size()));
    }
    else
    {
        if(mPopup->size().height()/2 < QCursor::pos().y())
        {
            previewPosition = 0 - mPopup->size().height()/2 + plugin()->panel()->panelSize()/2;
        }
        else
        {
            previewPosition = 0 -(QCursor::pos().y() + plugin()->panel()->panelSize()/2);
        }
        mPopup->setGeometry(plugin()->panel()->calculatePopupWindowPos(mapToGlobal(QPoint(0,previewPosition)), mPopup->size()));
    }
    if(mPopup->isVisible())
    {
//        mPopup-
    }
    else
    {
        mPopup->show();
    }
//   emit popupShown(this);
}
