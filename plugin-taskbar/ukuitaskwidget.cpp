/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "ukuitaskwidget.h"
#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"

//#include <UKUi/Settings>
#include "../panel/common/ukuisettings.h"

#include <QDebug>
#include <XdgIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QDragEnterEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDesktopWidget>

#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"
#include "ukuitaskclosebutton.h"

#include <KWindowSystem/KWindowSystem>
// Necessary for closeApplication()
#include <KWindowSystem/NETWM>
#include <QtX11Extras/QX11Info>

bool UKUITaskWidget::sDraggging = false;

/************************************************

************************************************/
//void LeftAlignedTextStyle::drawItemText(QPainter * painter, const QRect & rect, int flags
//            , const QPalette & pal, bool enabled, const QString & text
//            , QPalette::ColorRole textRole) const
//{
//    QString txt = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, rect.width());
//    return QProxyStyle::drawItemText(painter, rect, (flags & ~Qt::AlignHCenter) | Qt::AlignLeft, pal, enabled, txt, textRole);
//}


/************************************************

************************************************/
UKUITaskWidget::UKUITaskWidget(const WId window, UKUITaskBar * taskbar, QWidget *parent) :
    QWidget(parent),
    mWindow(window),
    mUrgencyHint(false),
    mOrigin(Qt::TopLeftCorner),
    mDrawPixmap(false),
    mParentTaskBar(taskbar),
    mPlugin(mParentTaskBar->plugin()),
    mDNDTimer(new QTimer(this))
{
    Q_ASSERT(taskbar);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMinimumWidth(1);
    setMinimumHeight(1);
    setAcceptDrops(true);
    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

    //for layout
    mCloseBtn =  new UKUITaskCloseButton(mWindow, this);
    mCloseBtn->setIcon(closePix);
    mTitleLabel = new QLabel;
    mThumbnailLabel = new QLabel;
    mAppIcon = new QLabel;
    mVWindowsLayout = new QVBoxLayout;
    mTopBarLayout = new QHBoxLayout;


    mTitleLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    mAppIcon->setAlignment(Qt::AlignLeft);
    mAppIcon->setScaledContents(false);


    // 自动缩放图片
    //	titleLabel->setScaledContents(true);
    mThumbnailLabel->setScaledContents(false);

    // 设置控件缩放方式
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    mTitleLabel->setSizePolicy(sizePolicy);
    mAppIcon->setSizePolicy(sizePolicy);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

//    mTitleLabel->setAttribute(Qt::WA_TranslucentBackground, true);
//    mAppIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    mAppIcon->resize(QSize(32,32));

    // 设置控件最大尺寸
    mTitleLabel->setFixedHeight(32);
    mTitleLabel->setMinimumWidth(1);
    mThumbnailLabel->setMinimumSize(QSize(1, 1));

//    iconLabel->setContentsMargins(5, 0, 0, 0);
    mTitleLabel->setContentsMargins(0, 0, 5, 0);
    mTopBarLayout->addWidget(mAppIcon);
    mTopBarLayout->addWidget(mTitleLabel);
    mTopBarLayout->addWidget(mCloseBtn);
    mVWindowsLayout->addLayout(mTopBarLayout);
    mVWindowsLayout->addWidget(mThumbnailLabel);
    this->setLayout(mVWindowsLayout);
    //
    updateText();
    updateIcon();
    mDNDTimer->setSingleShot(true);
    mDNDTimer->setInterval(700);
    connect(mDNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(mParentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskWidget::updateIcon);
    connect(mCloseBtn, SIGNAL(sigClicked()), this, SLOT(closeApplication()));
//    mParentTaskBar->setStyleSheet(
//                //正常状态样式
//                "QFrame{"
//                "border-width:2px;"                     //边框宽度像素
//                "}"
//                );
//    mParentTaskBar->setStyleSheet(
//                //正常状态样式
//                "QWidget{"
//                "background-color:rgba(190,216,239,5%);"
//                                "border-style:outset;"                  //边框样式（inset/outset）
//                                "qproperty-iconSize: 28px 28px;"
//                                "border-width:2px;"                     //边框宽度像素
//                                "border-radius:2px;"                   //边框圆角半径像素
//                                "font:bold 14px;"                       //字体，字体大小
//                                "color:rgba(0,0,0,100);"                //字体颜色
//                                "padding:0px;"
//                "}"
//                //鼠标悬停样式
//                "QWidget:hover{"
//                "background-color:rgba(190,216,239,20%);"
//                "}"
//                //鼠标按下样式
//                "QWidget:pressed{"
//                "background-color:rgba(190,216,239,12%);"
//                "}"


//                );
}

/************************************************

************************************************/
UKUITaskWidget::~UKUITaskWidget()
{
}

/************************************************

 ************************************************/
void UKUITaskWidget::updateText()
{
    KWindowInfo info(mWindow, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
    mTitleLabel->setText(title);
//    setText(title.replace("&", "&&"));
//    setToolTip(title);
}

/************************************************

 ************************************************/
void UKUITaskWidget::updateIcon()
{
    QIcon ico;
    if (mParentTaskBar->isIconByClass())
    {
        ico = XdgIcon::fromTheme(QString::fromUtf8(KWindowInfo{mWindow, 0, NET::WM2WindowClass}.windowClassClass()).toLower());
    }
    if (ico.isNull())
    {
        ico = KWindowSystem::icon(mWindow);
    }
    mAppIcon->setPixmap(ico.pixmap(QSize(32,32)));
    //mAppIcon->setWindowIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
    //setIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
}

/************************************************

 ************************************************/
void UKUITaskWidget::refreshIconGeometry(QRect const & geom)
{
    NETWinInfo info(QX11Info::connection(),
                    windowId(),
                    (WId) QX11Info::appRootWindow(),
                    NET::WMIconGeometry,
                    0);
    NETRect const curr = info.iconGeometry();
    if (curr.pos.x != geom.x() || curr.pos.y != geom.y()
            || curr.size.width != geom.width() || curr.size.height != geom.height())
    {
        NETRect nrect;
        nrect.pos.x = geom.x();
        nrect.pos.y = geom.y();
        nrect.size.height = geom.height();
        nrect.size.width = geom.width();
        info.setIconGeometry(nrect);
    }
}

/************************************************

 ************************************************/
void UKUITaskWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // It must be here otherwise dragLeaveEvent and dragMoveEvent won't be called
    // on the other hand drop and dragmove events of parent widget won't be called
    event->acceptProposedAction();
    if (event->mimeData()->hasFormat(mimeDataFormat()))
    {
        emit dragging(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    } else
    {
        mDNDTimer->start();
    }

    QWidget::dragEnterEvent(event);
}

void UKUITaskWidget::dragMoveEvent(QDragMoveEvent * event)
{
    if (event->mimeData()->hasFormat(mimeDataFormat()))
    {
        emit dragging(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    }
}

void UKUITaskWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    mDNDTimer->stop();
    QWidget::dragLeaveEvent(event);
}

void UKUITaskWidget::dropEvent(QDropEvent *event)
{
    mDNDTimer->stop();
    if (event->mimeData()->hasFormat(mimeDataFormat()))
    {
        emit dropped(event->source(), event->pos());
        setAttribute(Qt::WA_UnderMouse, false);
    }
    QWidget::dropEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskWidget::mousePressEvent(QMouseEvent* event)
{
    const Qt::MouseButton b = event->button();

    if (Qt::LeftButton == b)
        mDragStartPosition = event->pos();
    else if (Qt::MidButton == b && parentTaskBar()->closeOnMiddleClick())
        closeApplication();

    QWidget::mousePressEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskWidget::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug()<<"mouseReleaseEvent";
    if (event->button() == Qt::LeftButton)
    {
//        if (isChecked())
//            minimizeApplication();
//        else
            raiseApplication();
    }
    QWidget::mouseReleaseEvent(event);

}

/************************************************

 ************************************************/
QMimeData * UKUITaskWidget::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::WriteOnly);
    stream << (qlonglong)(mWindow);
    mimedata->setData(mimeDataFormat(), ba);
    return mimedata;
}

/************************************************

 ************************************************/
void UKUITaskWidget::mouseMoveEvent(QMouseEvent* event)
{
}

/************************************************

 ************************************************/
bool UKUITaskWidget::isApplicationHidden() const
{
    KWindowInfo info(mWindow, NET::WMState);
    return (info.state() & NET::Hidden);
}

/************************************************

 ************************************************/
bool UKUITaskWidget::isApplicationActive() const
{
    return KWindowSystem::activeWindow() == mWindow;
}

/************************************************

 ************************************************/
void UKUITaskWidget::activateWithDraggable()
{
    // raise app in any time when there is a drag
    // in progress to allow drop it into an app
    raiseApplication();
    KWindowSystem::forceActiveWindow(mWindow);
}

/************************************************

 ************************************************/
void UKUITaskWidget::raiseApplication()
{
    KWindowInfo info(mWindow, NET::WMDesktop | NET::WMState | NET::XAWMState);
    if (parentTaskBar()->raiseOnCurrentDesktop() && info.isMinimized())
    {
        KWindowSystem::setOnDesktop(mWindow, KWindowSystem::currentDesktop());
    }
    else
    {
        int winDesktop = info.desktop();
        if (KWindowSystem::currentDesktop() != winDesktop)
            KWindowSystem::setCurrentDesktop(winDesktop);
    }
    KWindowSystem::activateWindow(mWindow);

    setUrgencyHint(false);
}

/************************************************

 ************************************************/
void UKUITaskWidget::minimizeApplication()
{
    KWindowSystem::minimizeWindow(mWindow);
}

/************************************************

 ************************************************/
void UKUITaskWidget::maximizeApplication()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    int state = act->data().toInt();
    switch (state)
    {
        case NET::MaxHoriz:
            KWindowSystem::setState(mWindow, NET::MaxHoriz);
            break;

        case NET::MaxVert:
            KWindowSystem::setState(mWindow, NET::MaxVert);
            break;

        default:
            KWindowSystem::setState(mWindow, NET::Max);
            break;
    }

    if (!isApplicationActive())
        raiseApplication();
}

/************************************************

 ************************************************/
void UKUITaskWidget::deMaximizeApplication()
{
    KWindowSystem::clearState(mWindow, NET::Max);

    if (!isApplicationActive())
        raiseApplication();
}

/************************************************

 ************************************************/
void UKUITaskWidget::shadeApplication()
{
    KWindowSystem::setState(mWindow, NET::Shaded);
}

/************************************************

 ************************************************/
void UKUITaskWidget::unShadeApplication()
{
    KWindowSystem::clearState(mWindow, NET::Shaded);
}

/************************************************

 ************************************************/
void UKUITaskWidget::closeApplication()
{
    // FIXME: Why there is no such thing in KWindowSystem??
    qDebug()<<"closeApplication";
    NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(mWindow);
}

/************************************************

 ************************************************/
void UKUITaskWidget::setApplicationLayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    int layer = act->data().toInt();
    switch(layer)
    {
        case NET::KeepAbove:
            KWindowSystem::clearState(mWindow, NET::KeepBelow);
            KWindowSystem::setState(mWindow, NET::KeepAbove);
            break;

        case NET::KeepBelow:
            KWindowSystem::clearState(mWindow, NET::KeepAbove);
            KWindowSystem::setState(mWindow, NET::KeepBelow);
            break;

        default:
            KWindowSystem::clearState(mWindow, NET::KeepBelow);
            KWindowSystem::clearState(mWindow, NET::KeepAbove);
            break;
    }
}

/************************************************

 ************************************************/
void UKUITaskWidget::moveApplicationToDesktop()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    bool ok;
    int desk = act->data().toInt(&ok);

    if (!ok)
        return;

    KWindowSystem::setOnDesktop(mWindow, desk);
}

/************************************************

 ************************************************/
void UKUITaskWidget::moveApplication()
{
    KWindowInfo info(mWindow, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KWindowSystem::setCurrentDesktop(info.desktop());
    if (isMinimized())
        KWindowSystem::unminimizeWindow(mWindow);
    KWindowSystem::forceActiveWindow(mWindow);
    const QRect& g = KWindowInfo(mWindow, NET::WMGeometry).geometry();
    int X = g.center().x();
    int Y = g.center().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(mWindow, X, Y, NET::Move);
}

/************************************************

 ************************************************/
void UKUITaskWidget::resizeApplication()
{
    KWindowInfo info(mWindow, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KWindowSystem::setCurrentDesktop(info.desktop());
    if (isMinimized())
        KWindowSystem::unminimizeWindow(mWindow);
    KWindowSystem::forceActiveWindow(mWindow);
    const QRect& g = KWindowInfo(mWindow, NET::WMGeometry).geometry();
    int X = g.bottomRight().x();
    int Y = g.bottomRight().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(mWindow, X, Y, NET::BottomRight);
}

/************************************************

 ************************************************/
void UKUITaskWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        event->ignore();
        return;
    }

    KWindowInfo info(mWindow, 0, NET::WM2AllowedActions);
    unsigned long state = KWindowInfo(mWindow, NET::WMState).state();

    QMenu * menu = new QMenu(tr("Application"));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction* a;

    /* KDE menu *******

      + To &Desktop >
      +     &All Desktops
      +     ---
      +     &1 Desktop 1
      +     &2 Desktop 2
      + &To Current Desktop
        &Move
        Re&size
      + Mi&nimize
      + Ma&ximize
      + &Shade
        Ad&vanced >
            Keep &Above Others
            Keep &Below Others
            Fill screen
        &Layer >
            Always on &top
            &Normal
            Always on &bottom
      ---
      + &Close
    */

    /********** Desktop menu **********/
    int deskNum = KWindowSystem::numberOfDesktops();
    if (deskNum > 1)
    {
        int winDesk = KWindowInfo(mWindow, NET::WMDesktop).desktop();
        QMenu* deskMenu = menu->addMenu(tr("To &Desktop"));

        a = deskMenu->addAction(tr("&All Desktops"));
        a->setData(NET::OnAllDesktops);
        a->setEnabled(winDesk != NET::OnAllDesktops);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        deskMenu->addSeparator();

        for (int i = 0; i < deskNum; ++i)
        {
            a = deskMenu->addAction(tr("Desktop &%1").arg(i + 1));
            a->setData(i + 1);
            a->setEnabled(i + 1 != winDesk);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        }

        int curDesk = KWindowSystem::currentDesktop();
        a = menu->addAction(tr("&To Current Desktop"));
        a->setData(curDesk);
        a->setEnabled(curDesk != winDesk);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
    }

    /********** Move/Resize **********/
    menu->addSeparator();
    a = menu->addAction(tr("&Move"));
    a->setEnabled(info.actionSupported(NET::ActionMove) && !(state & NET::Max) && !(state & NET::FullScreen));
    connect(a, &QAction::triggered, this, &UKUITaskWidget::moveApplication);
    a = menu->addAction(tr("Resi&ze"));
    a->setEnabled(info.actionSupported(NET::ActionResize) && !(state & NET::Max) && !(state & NET::FullScreen));
    connect(a, &QAction::triggered, this, &UKUITaskWidget::resizeApplication);

    /********** State menu **********/
    menu->addSeparator();

    a = menu->addAction(tr("Ma&ximize"));
    a->setEnabled(info.actionSupported(NET::ActionMax) && (!(state & NET::Max) || (state & NET::Hidden)));
    a->setData(NET::Max);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

    if (event->modifiers() & Qt::ShiftModifier)
    {
        a = menu->addAction(tr("Maximize vertically"));
        a->setEnabled(info.actionSupported(NET::ActionMaxVert) && !((state & NET::MaxVert) || (state & NET::Hidden)));
        a->setData(NET::MaxVert);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

        a = menu->addAction(tr("Maximize horizontally"));
        a->setEnabled(info.actionSupported(NET::ActionMaxHoriz) && !((state & NET::MaxHoriz) || (state & NET::Hidden)));
        a->setData(NET::MaxHoriz);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));
    }

    a = menu->addAction(tr("&Restore"));
    a->setEnabled((state & NET::Hidden) || (state & NET::Max) || (state & NET::MaxHoriz) || (state & NET::MaxVert));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(deMaximizeApplication()));

    a = menu->addAction(tr("Mi&nimize"));
    a->setEnabled(info.actionSupported(NET::ActionMinimize) && !(state & NET::Hidden));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));

    if (state & NET::Shaded)
    {
        a = menu->addAction(tr("Roll down"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeApplication()));
    }
    else
    {
        a = menu->addAction(tr("Roll up"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeApplication()));
    }

    /********** Layer menu **********/
    menu->addSeparator();

    QMenu* layerMenu = menu->addMenu(tr("&Layer"));

    a = layerMenu->addAction(tr("Always on &top"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepAbove)
    a->setEnabled(!(state & NET::KeepAbove));
    a->setData(NET::KeepAbove);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("&Normal"));
    a->setEnabled((state & NET::KeepAbove) || (state & NET::KeepBelow));
    // FIXME: There is no NET::KeepNormal, so passing 0
    a->setData(0);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("Always on &bottom"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepBelow)
    a->setEnabled(!(state & NET::KeepBelow));
    a->setData(NET::KeepBelow);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    /********** Kill menu **********/
    menu->addSeparator();
    a = menu->addAction(XdgIcon::fromTheme("process-stop"), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu->setGeometry(mParentTaskBar->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menu->sizeHint()));
    mPlugin->willShowWindow(menu);
    menu->show();
}

/************************************************

 ************************************************/
void UKUITaskWidget::setUrgencyHint(bool set)
{
    if (mUrgencyHint == set)
        return;

    if (!set)
        KWindowSystem::demandAttention(mWindow, false);

    mUrgencyHint = set;
    setProperty("urgent", set);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/************************************************

 ************************************************/
bool UKUITaskWidget::isOnDesktop(int desktop) const
{
    return KWindowInfo(mWindow, NET::WMDesktop).isOnDesktop(desktop);
}

bool UKUITaskWidget::isOnCurrentScreen() const
{
    return QApplication::desktop()->screenGeometry(parentTaskBar()).intersects(KWindowInfo(mWindow, NET::WMFrameExtents).frameGeometry());
}

bool UKUITaskWidget::isMinimized() const
{
    return KWindowInfo(mWindow,NET::WMState | NET::XAWMState).isMinimized();
}

Qt::Corner UKUITaskWidget::origin() const
{
    return mOrigin;
}

void UKUITaskWidget::setOrigin(Qt::Corner newOrigin)
{
    if (mOrigin != newOrigin)
    {
        mOrigin = newOrigin;
        update();
    }
}

void UKUITaskWidget::setAutoRotation(bool value, IUKUIPanel::Position position)
{
    if (value)
    {
        switch (position)
        {
        case IUKUIPanel::PositionTop:
        case IUKUIPanel::PositionBottom:
            setOrigin(Qt::TopLeftCorner);
            break;

        case IUKUIPanel::PositionLeft:
            setOrigin(Qt::BottomLeftCorner);
            break;

        case IUKUIPanel::PositionRight:
            setOrigin(Qt::TopRightCorner);
            break;
        }
    }
    else
        setOrigin(Qt::TopLeftCorner);
}

void UKUITaskWidget::paintEvent(QPaintEvent *event)
{
}

bool UKUITaskWidget::hasDragAndDropHover() const
{
    return mDNDTimer->isActive();
}
 void UKUITaskWidget::setTitle()
 {
     mTitleLabel->setText("12345");
 }

 void UKUITaskWidget::setThumbNail(QPixmap _pixmap)
 {
     mThumbnailLabel->setPixmap(_pixmap);
 }
