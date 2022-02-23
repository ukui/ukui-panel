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

bool UKUITaskWidget::m_draggging = false;

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
    m_window(window),
    m_urgencyHint(false),
    m_origin(Qt::TopLeftCorner),
    m_drawPixmap(false),
    m_parentTaskBar(taskbar),
    m_plugin(m_parentTaskBar->plugin()),
    m_DNDTimer(new QTimer(this))
{
    Q_ASSERT(taskbar);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMinimumWidth(1);
    setMinimumHeight(1);
    setAcceptDrops(true);
    //    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
    m_status=NORMAL;
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);   //设置无边框窗口

    //for layout
    m_closeBtn =  new UKUITaskCloseButton(m_window, this);
//    m_closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_closeBtn->setIconSize(QSize(19,19));
    m_closeBtn->setFixedSize(QSize(19,19));
    m_titleLabel = new QLabel(this);
    m_titleLabel->setMargin(0);
    //    m_titleLabel->setContentsMargins(0,0,0,10);
    //    m_titleLabel->adjustSize();
    //    m_titleLabel->setStyleSheet("QLabel{background-color: red;}");

    m_thumbnailLabel = new QLabel(this);
    m_appIcon = new QLabel(this);
    m_vWindowsLayout = new QVBoxLayout(this);
    m_topBarLayout = new QHBoxLayout(this);
    m_topBarLayout->setContentsMargins(0,0,0,0);
    //    m_topBarLayout->setAlignment(Qt::AlignVCenter);
    //    m_topBarLayout->setDirection(QBoxLayout::LeftToRight);

    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_appIcon->setAlignment(Qt::AlignLeft);
    m_appIcon->setScaledContents(false);


    // 自动缩放图片
    //	titleLabel->setScaledContents(true);
    m_thumbnailLabel->setScaledContents(true);

    // 设置控件缩放方式
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_titleLabel->setSizePolicy(sizePolicy);
    m_appIcon->setSizePolicy(sizePolicy);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    //    m_titleLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    //    m_appIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    //    m_appIcon->resize(QSize(32,32));

    // 设置控件最大尺寸
    //m_titleLabel->setFixedHeight(32);
    m_titleLabel->setMinimumWidth(1);
    m_thumbnailLabel->setMinimumSize(QSize(1, 1));

    m_titleLabel->setContentsMargins(0, 0, 5, 0);
    //    m_topBarLayout->setSpacing(5);
    m_topBarLayout->addWidget(m_appIcon, 0, Qt::AlignLeft | Qt::AlignVCenter);
    m_topBarLayout->addWidget(m_titleLabel, 10, Qt::AlignLeft);
    m_topBarLayout->addWidget(m_closeBtn, 0, Qt::AlignRight);
    //    m_topBarLayout->addStretch();
//    m_topBarLayout->addWidget(m_closeBtn, 0, Qt::AlignRight | Qt::AlignVCenter);
    //    m_vWindowsLayout->setAlignment(Qt::AlignCenter);
    m_vWindowsLayout->addLayout(m_topBarLayout);
    m_vWindowsLayout->addWidget(m_thumbnailLabel, Qt::AlignCenter, Qt::AlignCenter);
    m_vWindowsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    this->setLayout(m_vWindowsLayout);
    updateText();
    updateIcon();
    m_DNDTimer->setSingleShot(true);
    m_DNDTimer->setInterval(700);
    connect(m_DNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(m_parentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskWidget::updateIcon);
    connect(m_closeBtn, SIGNAL(sigClicked()), this, SLOT(closeApplication()));
}

UKUITaskWidget::UKUITaskWidget(QString iconName, const WId window, UKUITaskBar * taskbar, QWidget *parent) :
    QWidget(parent),
    m_parentTaskBar(taskbar),
    m_window(window),
    m_DNDTimer(new QTimer(this))
{
    m_isWaylandWidget = true;
    //setMinimumWidth(400);
    //setMinimumHeight(400);
    m_status=NORMAL;
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setWindowFlags(Qt::FramelessWindowHint);   //设置无边框窗口

    //for layout
    m_closeBtn =  new UKUITaskCloseButton(m_window, this);
//    m_closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_closeBtn->setIconSize(QSize(19,19));
    m_closeBtn->setFixedSize(QSize(19,19));
    m_titleLabel = new QLabel(this);
    m_titleLabel->setMargin(0);
    //    m_titleLabel->setContentsMargins(0,0,0,10);
    //    m_titleLabel->adjustSize();
    //    m_titleLabel->setStyleSheet("QLabel{background-color: red;}");

    m_thumbnailLabel = new QLabel(this);
    m_appIcon = new QLabel(this);
    m_vWindowsLayout = new QVBoxLayout(this);
    m_topBarLayout = new QHBoxLayout(this);
    m_topBarLayout->setContentsMargins(0,0,0,0);
    //    m_topBarLayout->setAlignment(Qt::AlignVCenter);
    //    m_topBarLayout->setDirection(QBoxLayout::LeftToRight);

    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_appIcon->setAlignment(Qt::AlignLeft);
    m_appIcon->setScaledContents(false);


    // 自动缩放图片
    //	titleLabel->setScaledContents(true);
    m_thumbnailLabel->setScaledContents(true);

    // 设置控件缩放方式
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_titleLabel->setSizePolicy(sizePolicy);
    m_appIcon->setSizePolicy(sizePolicy);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    //    m_titleLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    //    m_appIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    //    m_appIcon->resize(QSize(32,32));

    // 设置控件最大尺寸
    //m_titleLabel->setFixedHeight(32);
    m_titleLabel->setMinimumWidth(1);
    m_thumbnailLabel->setMinimumSize(QSize(1, 1));
    m_thumbnailLabel->setMaximumSize(QSize(this->width()*2,this->height()*8));

    m_titleLabel->setContentsMargins(0, 0, 5, 0);
    //    m_topBarLayout->setSpacing(5);
    m_topBarLayout->addWidget(m_appIcon, 0, Qt::AlignLeft | Qt::AlignVCenter);
    m_topBarLayout->addWidget(m_titleLabel, 10, Qt::AlignLeft);
    m_topBarLayout->addWidget(m_closeBtn, 0, Qt::AlignRight);
    //    m_topBarLayout->addStretch();
//    m_topBarLayout->addWidget(m_closeBtn, 0, Qt::AlignRight | Qt::AlignVCenter);
    //    m_vWindowsLayout->setAlignment(Qt::AlignCenter);
    m_vWindowsLayout->addLayout(m_topBarLayout);
    m_vWindowsLayout->addWidget(m_thumbnailLabel, Qt::AlignCenter, Qt::AlignCenter);
    m_vWindowsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    this->setLayout(m_vWindowsLayout);
    updateText();
    updateIcon();
    m_DNDTimer->setSingleShot(true);
    m_DNDTimer->setInterval(700);
    connect(m_DNDTimer, SIGNAL(timeout()), this, SLOT(activateWithDraggable()));
    connect(UKUi::Settings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(updateIcon()));
    connect(m_parentTaskBar, &UKUITaskBar::iconByClassChanged, this, &UKUITaskWidget::updateIcon);
    connect(m_closeBtn, SIGNAL(sigClicked()), this, SLOT(closeApplication()));
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
    KWindowInfo info(m_window, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();

    m_titleLabel->setToolTip(title);
    QTimer::singleShot(0,this,[=](){
        QString formatAppName = m_titleLabel->fontMetrics().elidedText(title,Qt::ElideRight, m_titleLabel->width());
        m_titleLabel->setText(formatAppName);
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::white);
        m_titleLabel->setPalette(pa);
    });

//    m_titleLabel->setText(title);

    //    setText(title.replace("&", "&&"));
    //    setToolTip(title);
}

/************************************************

 ************************************************/
void UKUITaskWidget::updateIcon()
{
    QIcon ico;
    if (m_parentTaskBar->isIconByClass())
    {
        ico = QIcon::fromTheme(QString::fromUtf8(KWindowInfo{m_window, 0, NET::WM2WindowClass}.windowClassClass()).toLower());
    }
    if (ico.isNull())
    {
        ico = KWindowSystem::icon(m_window);
    }
    m_appIcon->setPixmap(ico.pixmap(QSize(19,19)));
    setPixmap(KWindowSystem::icon(m_window));
//    m_pixmap = ico.pixmap(QSize(64,64);
    //m_appIcon->setWindowIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
    //setIcon(ico.isNull() ? XdgIcon::defaultApplicationIcon() : ico);
}


void UKUITaskWidget::setPixmap(QPixmap _pixmap)
{
    m_pixmap = _pixmap;
}

QPixmap UKUITaskWidget::getPixmap()
{
    return m_pixmap;
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
        m_DNDTimer->start();
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
    m_DNDTimer->stop();
    QWidget::dragLeaveEvent(event);
}

void UKUITaskWidget::dropEvent(QDropEvent *event)
{
    m_DNDTimer->stop();
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
        m_dragStartPosition = event->pos();
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
    m_status = NORMAL;
    update();
    QWidget::mouseReleaseEvent(event);

}

/************************************************

 ************************************************/

void UKUITaskWidget::enterEvent(QEvent *)
{
    m_status = HOVER;
    repaint();
}

void UKUITaskWidget::leaveEvent(QEvent *)
{
    m_status = NORMAL;
    repaint();
}
QMimeData * UKUITaskWidget::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::WriteOnly);
    stream << (qlonglong)(m_window);
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
    KWindowInfo info(m_window, 0, NET::WM2AllowedActions);
    unsigned long state = KWindowInfo(m_window, NET::WMState).state();

    if (!m_plugin || m_isWaylandWidget)
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
    if (!m_isWaylandWidget)
        menu->show();
}
/************************************************

 ************************************************/
bool UKUITaskWidget::isApplicationHidden() const
{
    KWindowInfo info(m_window, NET::WMState);
    return (info.state() & NET::Hidden);
}


/************************************************

 ************************************************/
bool UKUITaskWidget::isApplicationActive() const
{
    return KWindowSystem::activeWindow() == m_window;
}

/************************************************

 ************************************************/
void UKUITaskWidget::activateWithDraggable()
{
    // raise app in any time when there is a drag
    // in progress to allow drop it into an app
    raiseApplication();
    KWindowSystem::forceActiveWindow(m_window);
}

/************************************************

 ************************************************/
void UKUITaskWidget::raiseApplication()
{
    KWindowSystem::clearState(m_window, NET::Hidden);
    if (m_isWaylandWidget) {
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

    KWindowInfo info(m_window, NET::WMDesktop | NET::WMState | NET::XAWMState);
    if (parentTaskBar()->raiseOnCurrentDesktop() && info.isMinimized())
    {
        KWindowSystem::setOnDesktop(m_window, KWindowSystem::currentDesktop());
    }
    else
    {
        int winDesktop = info.desktop();
        if (KWindowSystem::currentDesktop() != winDesktop)
            KWindowSystem::setCurrentDesktop(winDesktop);
    }
    KWindowSystem::activateWindow(m_window);
    emit windowMaximize();

    setUrgencyHint(false);
}

/************************************************

 ************************************************/
void UKUITaskWidget::minimizeApplication()
{
    KWindowSystem::minimizeWindow(m_window);
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
        KWindowSystem::setState(m_window, NET::MaxHoriz);
        break;

    case NET::MaxVert:
        KWindowSystem::setState(m_window, NET::MaxVert);
        break;

    default:
        KWindowSystem::setState(m_window, NET::Max);
        break;
    }

    if (!isApplicationActive())
        raiseApplication();
}

/************************************************

 ************************************************/
void UKUITaskWidget::deMaximizeApplication()
{
    KWindowSystem::clearState(m_window, NET::Max);

    if (!isApplicationActive())
        raiseApplication();
}


void UKUITaskWidget::setWindowKeepAbove()
{
    if (!isApplicationActive())
        raiseApplication();
    KWindowSystem::setState(m_window, NET::KeepAbove);
}

void UKUITaskWidget::setWindowStatusClear()
{
    KWindowSystem::clearState(m_window, NET::KeepAbove);
}
/************************************************

 ************************************************/
void UKUITaskWidget::shadeApplication()
{
    KWindowSystem::setState(m_window, NET::Shaded);
}

/************************************************

 ************************************************/
void UKUITaskWidget::unShadeApplication()
{
    KWindowSystem::clearState(m_window, NET::Shaded);
}

/************************************************

 ************************************************/
//void UKUITaskWidget::priv_closeApplication() {

//}
void UKUITaskWidget::closeApplication()
{
    // FIXME: Why there is no such thing in KWindowSystem??
    if (m_isWaylandWidget) {
        QDBusMessage message = QDBusMessage::createSignal("/", "com.ukui.kwin", "request");
        QList<QVariant> args;
        quint32 m_wid=windowId();
        args.append(m_wid);
        args.append(WAYLAND_GROUP_CLOSE);
        message.setArguments(args);
        QDBusConnection::sessionBus().send(message);
    }
    NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(m_window);
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
        KWindowSystem::clearState(m_window, NET::KeepBelow);
        KWindowSystem::setState(m_window, NET::KeepAbove);
        break;

    case NET::KeepBelow:
        KWindowSystem::clearState(m_window, NET::KeepAbove);
        KWindowSystem::setState(m_window, NET::KeepBelow);
        break;

    default:
        KWindowSystem::clearState(m_window, NET::KeepBelow);
        KWindowSystem::clearState(m_window, NET::KeepAbove);
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

    KWindowSystem::setOnDesktop(m_window, desk);
}

/************************************************

 ************************************************/
void UKUITaskWidget::moveApplication()
{
    KWindowInfo info(m_window, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KWindowSystem::setCurrentDesktop(info.desktop());
    if (isMinimized())
        KWindowSystem::unminimizeWindow(m_window);
    KWindowSystem::forceActiveWindow(m_window);
    const QRect& g = KWindowInfo(m_window, NET::WMGeometry).geometry();
    int X = g.center().x();
    int Y = g.center().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(m_window, X, Y, NET::Move);
}

/************************************************

 ************************************************/
void UKUITaskWidget::resizeApplication()
{
    KWindowInfo info(m_window, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KWindowSystem::setCurrentDesktop(info.desktop());
    if (isMinimized())
        KWindowSystem::unminimizeWindow(m_window);
    KWindowSystem::forceActiveWindow(m_window);
    const QRect& g = KWindowInfo(m_window, NET::WMGeometry).geometry();
    int X = g.bottomRight().x();
    int Y = g.bottomRight().y();
    QCursor::setPos(X, Y);
    NETRootInfo(QX11Info::connection(), NET::WMMoveResize).moveResizeRequest(m_window, X, Y, NET::BottomRight);
}

/************************************************

 ************************************************/
void UKUITaskWidget::setUrgencyHint(bool set)
{
    if (m_urgencyHint == set)
        return;

    if (!set)
        KWindowSystem::demandAttention(m_window, false);

    m_urgencyHint = set;
    setProperty("urgent", set);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/************************************************

 ************************************************/
bool UKUITaskWidget::isOnDesktop(int desktop) const
{
    return KWindowInfo(m_window, NET::WMDesktop).isOnDesktop(desktop);
}

bool UKUITaskWidget::isOnCurrentScreen() const
{
    return QApplication::desktop()->screenGeometry(parentTaskBar()).intersects(KWindowInfo(m_window, NET::WMFrameExtents).frameGeometry());
}

bool UKUITaskWidget::isMinimized() const
{
    return KWindowInfo(m_window,NET::WMState | NET::XAWMState).isMinimized();
}

bool UKUITaskWidget::isFocusState() const
{
    qDebug()<<"KWindowInfo(m_window,NET::WMState).state():"<<KWindowInfo(m_window,NET::WMState).state();
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    return NET::Focused == (KWindowInfo(m_window,NET::WMState).state()&NET::Focused);
#else
    return isApplicationActive();
#endif
}

Qt::Corner UKUITaskWidget::origin() const
{
    return m_origin;
}

void UKUITaskWidget::setOrigin(Qt::Corner newOrigin)
{
    if (m_origin != newOrigin)
    {
        m_origin = newOrigin;
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

    switch(m_status)
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
    switch(m_status)
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
    return m_DNDTimer->isActive();
}
void UKUITaskWidget::updateTitle()
{
    updateText();
}

void UKUITaskWidget::setThumbNail(QPixmap _pixmap)
{
    m_thumbnailLabel->setPixmap(_pixmap);
}

void UKUITaskWidget::removeThumbNail()
{
    if(m_thumbnailLabel)
    {
        m_vWindowsLayout->removeWidget(m_thumbnailLabel);
        m_thumbnailLabel->setParent(NULL);
        m_thumbnailLabel->deleteLater();
        m_thumbnailLabel = NULL;
    }
}

void UKUITaskWidget::addThumbNail()
{
    if(!m_thumbnailLabel)
    {
        m_thumbnailLabel = new QLabel(this);
        m_thumbnailLabel->setScaledContents(true);
        m_thumbnailLabel->setMinimumSize(QSize(1, 1));
        //        m_vWindowsLayout->addLayout(m_topBarLayout, 100);
        m_vWindowsLayout->addWidget(m_thumbnailLabel, 0, Qt::AlignCenter);
    }
    else
    {
        return;
    }
}


void UKUITaskWidget::setTitleFixedWidth(int size)
{
    m_titleLabel->setFixedWidth(size);
    m_titleLabel->adjustSize();
}

int UKUITaskWidget::getWidth()
{
    return m_titleLabel->width();
}

void UKUITaskWidget::setThumbFixedSize(int w) {
    this->m_thumbnailLabel->setFixedWidth(w);
}

void UKUITaskWidget::setThumbMaximumSize(int w) {
    this->m_thumbnailLabel->setMaximumWidth(w);
}

void UKUITaskWidget::setThumbScale(bool val) {
    this->m_thumbnailLabel->setScaledContents(val);
}

void UKUITaskWidget::wl_updateIcon(QString iconName){
    m_appIcon->setPixmap(QIcon::fromTheme(iconName).pixmap(QSize(19,19)));
}

void UKUITaskWidget::wl_updateTitle(QString caption) {
    m_titleLabel->setText(caption);
    printf("\n%s\n", caption.toStdString().data());
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    m_titleLabel->setPalette(pa);
}
