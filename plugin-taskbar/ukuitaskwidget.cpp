/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "ukuitaskwidget.h"
#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"

//#include <UKUi/Settings>
#include "../panel/common/ukuisettings.h"

#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPainterPath>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QDragEnterEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDesktopWidget>
#include <QScreen>

#include "ukuitaskgroup.h"
#include "ukuitaskbar.h"
#include "ukuitaskclosebutton.h"

#include <KWindowSystem/KWindowSystem>
// Necessary for closeApplication()
#include <KWindowSystem/NETWM>
#include <QtX11Extras/QX11Info>

#define WAYLAND_GROUP_HIDE     0
#define WAYLAND_GROUP_ACTIVATE 1
#define WAYLAND_GROUP_CLOSE    2

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
    //    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    status=NORMAL;
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);   //设置无边框窗口

    //for layout
    mCloseBtn =  new UKUITaskCloseButton(mWindow, this);
//    mCloseBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    mCloseBtn->setIconSize(QSize(19,19));
    mCloseBtn->setFixedSize(QSize(19,19));
    mTitleLabel = new QLabel(this);
    mTitleLabel->setMargin(0);
    //    mTitleLabel->setContentsMargins(0,0,0,10);
    //    mTitleLabel->adjustSize();
    //    mTitleLabel->setStyleSheet("QLabel{background-color: red;}");

    mThumbnailLabel = new QLabel(this);
    mAppIcon = new QLabel(this);
    mVWindowsLayout = new QVBoxLayout(this);
    mTopBarLayout = new QHBoxLayout(this);
    mTopBarLayout->setContentsMargins(0,0,0,0);
    //    mTopBarLayout->setAlignment(Qt::AlignVCenter);
    //    mTopBarLayout->setDirection(QBoxLayout::LeftToRight);

    mTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mAppIcon->setAlignment(Qt::AlignLeft);
    mAppIcon->setScaledContents(false);


    // 自动缩放图片
    //	titleLabel->setScaledContents(true);
    mThumbnailLabel->setScaledContents(true);

    // 设置控件缩放方式
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    mTitleLabel->setSizePolicy(sizePolicy);
    mAppIcon->setSizePolicy(sizePolicy);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    //    mTitleLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    //    mAppIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    //    mAppIcon->resize(QSize(32,32));

    // 设置控件最大尺寸
    //mTitleLabel->setFixedHeight(32);
    mTitleLabel->setMinimumWidth(1);
    mThumbnailLabel->setMinimumSize(QSize(1, 1));

    mTitleLabel->setContentsMargins(0, 0, 5, 0);
    //    mTopBarLayout->setSpacing(5);
    mTopBarLayout->addWidget(mAppIcon, 0, Qt::AlignLeft | Qt::AlignVCenter);
    mTopBarLayout->addWidget(mTitleLabel, 10, Qt::AlignLeft);
    mTopBarLayout->addWidget(mCloseBtn, 0, Qt::AlignRight);
    //    mTopBarLayout->addStretch();
//    mTopBarLayout->addWidget(mCloseBtn, 0, Qt::AlignRight | Qt::AlignVCenter);
    //    mVWindowsLayout->setAlignment(Qt::AlignCenter);
    mVWindowsLayout->addLayout(mTopBarLayout);
    mVWindowsLayout->addWidget(mThumbnailLabel, Qt::AlignCenter, Qt::AlignCenter);
    mVWindowsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    this->setLayout(mVWindowsLayout);
    updateText();
    updateIcon();
    mDNDTimer->setSingleShot(true);
    mDNDTimer->setInterval(700);
    connect(mDNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(mParentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskWidget::updateIcon);
    connect(mCloseBtn, SIGNAL(sigClicked()), this, SLOT(closeApplication()));
}

UKUITaskWidget::UKUITaskWidget(QString iconName, const WId window, UKUITaskBar * taskbar, QWidget *parent) :
    QWidget(parent),
    mParentTaskBar(taskbar),
    mWindow(window),
    mDNDTimer(new QTimer(this))
{
    isWaylandWidget = true;
    //setMinimumWidth(400);
    //setMinimumHeight(400);
    status=NORMAL;
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);   //设置无边框窗口

    //for layout
    mCloseBtn =  new UKUITaskCloseButton(mWindow, this);
//    mCloseBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    mCloseBtn->setIconSize(QSize(19,19));
    mCloseBtn->setFixedSize(QSize(19,19));
    mTitleLabel = new QLabel(this);
    mTitleLabel->setMargin(0);
    //    mTitleLabel->setContentsMargins(0,0,0,10);
    //    mTitleLabel->adjustSize();
    //    mTitleLabel->setStyleSheet("QLabel{background-color: red;}");

    mThumbnailLabel = new QLabel(this);
    mAppIcon = new QLabel(this);
    mVWindowsLayout = new QVBoxLayout(this);
    mTopBarLayout = new QHBoxLayout(this);
    mTopBarLayout->setContentsMargins(0,0,0,0);
    //    mTopBarLayout->setAlignment(Qt::AlignVCenter);
    //    mTopBarLayout->setDirection(QBoxLayout::LeftToRight);

    mTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mAppIcon->setAlignment(Qt::AlignLeft);
    mAppIcon->setScaledContents(false);


    // 自动缩放图片
    //	titleLabel->setScaledContents(true);
    mThumbnailLabel->setScaledContents(true);

    // 设置控件缩放方式
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    mTitleLabel->setSizePolicy(sizePolicy);
    mAppIcon->setSizePolicy(sizePolicy);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    //    mTitleLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    //    mAppIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    //    mAppIcon->resize(QSize(32,32));

    // 设置控件最大尺寸
    //mTitleLabel->setFixedHeight(32);
    mTitleLabel->setMinimumWidth(1);
    mThumbnailLabel->setMinimumSize(QSize(1, 1));
    mThumbnailLabel->setMaximumSize(QSize(this->width()*2,this->height()*8));

    mTitleLabel->setContentsMargins(0, 0, 5, 0);
    //    mTopBarLayout->setSpacing(5);
    mTopBarLayout->addWidget(mAppIcon, 0, Qt::AlignLeft | Qt::AlignVCenter);
    mTopBarLayout->addWidget(mTitleLabel, 10, Qt::AlignLeft);
    mTopBarLayout->addWidget(mCloseBtn, 0, Qt::AlignRight);
    //    mTopBarLayout->addStretch();
//    mTopBarLayout->addWidget(mCloseBtn, 0, Qt::AlignRight | Qt::AlignVCenter);
    //    mVWindowsLayout->setAlignment(Qt::AlignCenter);
    mVWindowsLayout->addLayout(mTopBarLayout);
    mVWindowsLayout->addWidget(mThumbnailLabel, Qt::AlignCenter, Qt::AlignCenter);
    mVWindowsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    this->setLayout(mVWindowsLayout);
    updateText();
    updateIcon();
    mDNDTimer->setSingleShot(true);
    mDNDTimer->setInterval(700);
    connect(mDNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(mParentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskWidget::updateIcon);
    connect(mCloseBtn, SIGNAL(sigClicked()), this, SLOT(closeApplication()));
}
/************************************************

************************************************/
UKUITaskWidget::~UKUITaskWidget()
{
    this->deleteLater();
}

/************************************************

 ************************************************/
void UKUITaskWidget::updateText()
{
    KWindowInfo info(mWindow, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();

    mTitleLabel->setToolTip(title);
    QTimer::singleShot(0,this,[=](){
        QString formatAppName = mTitleLabel->fontMetrics().elidedText(title,Qt::ElideRight, mTitleLabel->width());
        mTitleLabel->setText(formatAppName);
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::white);
        mTitleLabel->setPalette(pa);
    });

//    mTitleLabel->setText(title);

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
        ico = QIcon::fromTheme(QString::fromUtf8(KWindowInfo{mWindow, 0, NET::WM2WindowClass}.windowClassClass()).toLower());
    }
    if (ico.isNull())
    {
        ico = KWindowSystem::icon(mWindow);
    }
    mAppIcon->setPixmap(ico.pixmap(QSize(19,19)));
    setPixmap(KWindowSystem::icon(mWindow));
//    mPixmap = ico.pixmap(QSize(64,64);
    //mAppIcon->setWindowIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
    //setIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
}


void UKUITaskWidget::setPixmap(QPixmap _pixmap)
{
    mPixmap = _pixmap;
}

QPixmap UKUITaskWidget::getPixmap()
{
    return mPixmap;
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
    {
        mDragStartPosition = event->pos();
    }
    else if (Qt::MidButton == b && parentTaskBar()->closeOnMiddleClick())
        closeApplication();

    QWidget::mousePressEvent(event);
}

/************************************************

 ************************************************/
void UKUITaskWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        //        if (isChecked())
        //            minimizeApplication();
        //        else
        raiseApplication();
    }
    status = NORMAL;
    update();
    QWidget::mouseReleaseEvent(event);

}

/************************************************

 ************************************************/

void UKUITaskWidget::enterEvent(QEvent *)
{
    status = HOVER;
    repaint();
}

void UKUITaskWidget::leaveEvent(QEvent *)
{
    status = NORMAL;
    repaint();
}
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

void UKUITaskWidget::closeGroup() {
    emit closeSigtoGroup();
}

void UKUITaskWidget::contextMenuEvent(QContextMenuEvent *event)
{
    KWindowInfo info(mWindow, 0, NET::WM2AllowedActions);
    unsigned long state = KWindowInfo(mWindow, NET::WMState).state();

    if (!mPlugin || isWaylandWidget)
        return;
    QMenu * menu = new QMenu(tr("Widget"));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    /* 对应预览图右键功能 关闭 还原 最大化  最小化 置顶 取消置顶*/
    QAction *close = menu->addAction(QIcon::fromTheme("window-close-symbolic"), tr("close"));
    QAction *restore = menu->addAction(QIcon::fromTheme("window-restore-symbolic"), tr("restore"));

    QAction *maxim = menu->addAction(QIcon::fromTheme("window-maximize-symbolic"), tr("maximaze"));
    maxim->setEnabled(info.actionSupported(NET::ActionMax) && (!(state & NET::Max) || (state & NET::Hidden)));

    QAction *minim = menu->addAction(QIcon::fromTheme("window-minimize-symbolic"), tr("minimize"));
    QAction *above = menu->addAction(QIcon::fromTheme("ukui-fixed"), tr("above"));
    QAction *clear = menu->addAction(QIcon::fromTheme("ukui-unfixed"), tr("clear"));

    connect(close, SIGNAL(triggered()), this, SLOT(closeApplication()));
    connect(restore, SIGNAL(triggered()), this, SLOT(deMaximizeApplication()));
    connect(maxim, SIGNAL(triggered()), this, SLOT(maximizeApplication()));
    connect(minim, SIGNAL(triggered()), this, SLOT(minimizeApplication()));
    connect(above, SIGNAL(triggered()), this, SLOT(setWindowKeepAbove()));
    connect(clear, SIGNAL(triggered()), this, SLOT(setWindowStatusClear()));
    connect(menu, &QMenu::aboutToHide, [this] {
        emit closeSigtoPop();

    });
    above->setEnabled(!(state & NET::KeepAbove));
    clear->setEnabled(state & NET::KeepAbove);
    menu->exec(cursor().pos());
    plugin()->willShowWindow(menu);
    if (!isWaylandWidget)
        menu->show();
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
    KWindowSystem::clearState(mWindow, NET::Hidden);
    if (isWaylandWidget) {
        QDBusMessage message = QDBusMessage::createSignal("/", "com.ukui.kwin", "request");
        QList<QVariant> args;
        quint32 m_wid=windowId();
        args.append(m_wid);
        args.append(WAYLAND_GROUP_ACTIVATE);
        repaint();
        message.setArguments(args);
        QDBusConnection::sessionBus().send(message);
        emit windowMaximize();

        setUrgencyHint(false);
        return;
    }

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
    emit windowMaximize();

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


void UKUITaskWidget::setWindowKeepAbove()
{
    if (!isApplicationActive())
        raiseApplication();
    KWindowSystem::setState(mWindow, NET::KeepAbove);
}

void UKUITaskWidget::setWindowStatusClear()
{
    KWindowSystem::clearState(mWindow, NET::KeepAbove);
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
//void UKUITaskWidget::priv_closeApplication() {

//}
void UKUITaskWidget::closeApplication()
{
    // FIXME: Why there is no such thing in KWindowSystem??
    if (isWaylandWidget) {
        QDBusMessage message = QDBusMessage::createSignal("/", "com.ukui.kwin", "request");
        QList<QVariant> args;
        quint32 m_wid=windowId();
        args.append(m_wid);
        args.append(WAYLAND_GROUP_CLOSE);
        message.setArguments(args);
        QDBusConnection::sessionBus().send(message);
    }
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

bool UKUITaskWidget::isFocusState() const
{
    qDebug()<<"KWindowInfo(mWindow,NET::WMState).state():"<<KWindowInfo(mWindow,NET::WMState).state();
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    return NET::Focused == (KWindowInfo(mWindow,NET::WMState).state()&NET::Focused);
#else
    return isApplicationActive();
#endif
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

    /*旧的设置预览三态的方式，注释掉的原因是其未能设置阴影*/
#if 0
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);

    switch(status)
    {
    case NORMAL:
    {
        p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
        p.setPen(Qt::black);
        break;
    }
    case HOVER:
    {
        //              p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
        p.setBrush(QBrush(QColor(0x13,0x14,0x14,0x19)));
        p.setPen(Qt::black);
        break;
    }
    case PRESS:
    {
        p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
        p.setPen(Qt::white);
        break;
    }
    }
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

#endif

#if 1

    /*
     * 预览图的设置阴影的方式与其他控件有所不同
     * 由于涉及到UKUITaskWidget　中心是一张截图
     * 此处设置阴影的方式不是一种通用的方式
     * tr:
     * The way of setting shadow in preview image is different from other controls
     * As it involves UKUITaskWidget center is a screenshot
     * The way to set the shadow here is not a general way
*/
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

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    /*在Qt中定义了一个常量，用于设置透明的颜色，即Qt::transparent，表示RGBA值为(0,0,0,0)的透明色。*/
    //    pixmapPainter2.setPen(Qt::transparent);
    //    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制底色
    p.save();
    switch(status)
    {
    case NORMAL:
    {
        p.fillPath(rectPath, QColor(0x13,0x14,0x14,0xb2));
        break;
    }
    case HOVER:
    {
        p.fillPath(rectPath, QColor(0x13,0x14,0x14,0x66));
        break;
    }
    case PRESS:
    {
        p.fillPath(rectPath, QColor(0xFF,0xFF,0xFF,0x19));

        break;
    }
    }
    p.restore();
#endif
}


bool UKUITaskWidget::hasDragAndDropHover() const
{
    return mDNDTimer->isActive();
}
void UKUITaskWidget::updateTitle()
{
    updateText();
}

void UKUITaskWidget::setThumbNail(QPixmap _pixmap)
{
    mThumbnailLabel->setPixmap(_pixmap);
}

void UKUITaskWidget::removeThumbNail()
{
    if(mThumbnailLabel)
    {
        mVWindowsLayout->removeWidget(mThumbnailLabel);
        mThumbnailLabel->setParent(NULL);
        mThumbnailLabel->deleteLater();
        mThumbnailLabel = NULL;
    }
}

void UKUITaskWidget::addThumbNail()
{
    if(!mThumbnailLabel)
    {
        mThumbnailLabel = new QLabel(this);
        mThumbnailLabel->setScaledContents(true);
        mThumbnailLabel->setMinimumSize(QSize(1, 1));
        //        mVWindowsLayout->addLayout(mTopBarLayout, 100);
        mVWindowsLayout->addWidget(mThumbnailLabel, 0, Qt::AlignCenter);
    }
    else
    {
        return;
    }
}


void UKUITaskWidget::setTitleFixedWidth(int size)
{
    mTitleLabel->setFixedWidth(size);
    mTitleLabel->adjustSize();
}

int UKUITaskWidget::getWidth()
{
    return mTitleLabel->width();
}

void UKUITaskWidget::setThumbFixedSize(int w) {
    this->mThumbnailLabel->setFixedWidth(w);
}

void UKUITaskWidget::setThumbMaximumSize(int w) {
    this->mThumbnailLabel->setMaximumWidth(w);
}

void UKUITaskWidget::setThumbScale(bool val) {
    this->mThumbnailLabel->setScaledContents(val);
}

void UKUITaskWidget::wl_updateIcon(QString iconName){
    mAppIcon->setPixmap(QIcon::fromTheme(iconName).pixmap(QSize(19,19)));
}

void UKUITaskWidget::wl_updateTitle(QString caption) {
    mTitleLabel->setText(caption);
    printf("\n%s\n", caption.toStdString().data());
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    mTitleLabel->setPalette(pa);
}
