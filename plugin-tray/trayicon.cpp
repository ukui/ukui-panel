/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
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


// Warning: order of those include is important.
#include <QDebug>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QBitmap>
#include <QStyle>
#include <QScreen>
#include <QDrag>

#include "../panel/ukuipanel.h"
#include "trayicon.h"
#include "xfitman.h"

#include <QtX11Extras/QX11Info>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include "qmath.h"
#include <QToolButton>
#define XEMBED_EMBEDDED_NOTIFY 0

static bool xError;
#define MIMETYPE "ukui/UkuiTray"


int windowErrorHandler(Display *d, XErrorEvent *e)
{
    xError = true;
    if (e->error_code != BadWindow) {
        char str[1024];
        XGetErrorText(d, e->error_code,  str, 1024);
        qWarning() << "Error handler" << e->error_code
                   << str;
    }
    return 0;
}


/************************************************

 ************************************************/
TrayIcon::TrayIcon(Window iconId, QSize const & iconSize, QWidget* parent):
    QToolButton(parent),
    mIconId(iconId),
    mWindowId(0),
    mIconSize(iconSize),
    mDamage(0),
    mDisplay(QX11Info::display())
{
    /*
     * NOTE:
     * 如果不保存QX11Info::display()的返回值，ukui-panel会有潜在崩溃问题
     * it's a good idea to save the return value of QX11Info::display().
     * In Qt 5, this API is slower and has some limitations which can trigger crashes.
     * The XDisplay value is actally stored in QScreen object of the primary screen rather than
     * in a global variable. So when the parimary QScreen is being deleted and becomes invalid,
     * QX11Info::display() will fail and cause crash. Storing this value improves the efficiency and
     * also prevent potential crashes caused by this bug.
    */

    traystatus=NORMAL;
    setObjectName("TrayIcon");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setAcceptDrops(true);

    QTimer::singleShot(200, [this] { init(); update(); });
    repaint();

    /**/
    QDBusConnection::sessionBus().unregisterService("com.ukui.panel");
    QDBusConnection::sessionBus().registerService("com.ukui.panel");
    QDBusConnection::sessionBus().registerObject("/traybutton/click", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);
}



/************************************************

 ************************************************/
void TrayIcon::init()
{
    Display* dsp = mDisplay;

    XWindowAttributes attr;
    if (! XGetWindowAttributes(dsp, mIconId, &attr))
    {
        deleteLater();
        return;
    }

//        qDebug() << "New tray icon ***********************************";
//        qDebug() << "  * window id:  " << hex << mIconId;
//        qDebug() << "  * window name:" << xfitMan().getApplicationName(mIconId);
//        qDebug() << "  * size (WxH): " << attr.width << "x" << attr.height;
//        qDebug() << "  * color depth:" << attr.depth;

    unsigned long mask = 0;
    XSetWindowAttributes set_attr;

    Visual* visual = attr.visual;
    set_attr.colormap = attr.colormap;
    set_attr.background_pixel = 0;
    set_attr.border_pixel = 0;
    mask = CWColormap|CWBackPixel|CWBorderPixel;

    const QRect icon_geom = iconGeometry();
    mWindowId = XCreateWindow(dsp, this->winId(), icon_geom.x(), icon_geom.y(), icon_geom.width() * metric(PdmDevicePixelRatio), icon_geom.height() * metric(PdmDevicePixelRatio),
                              0, attr.depth, InputOutput, visual, mask, &set_attr);


    xError = false;
    XErrorHandler old;
    old = XSetErrorHandler(windowErrorHandler);
    XReparentWindow(dsp, mIconId, mWindowId, 0, 0);
    XSync(dsp, false);
    XSetErrorHandler(old);

    if (xError)
    {
        qWarning() << "* Not icon_swallow                     *";
        XDestroyWindow(dsp, mWindowId);
        mWindowId = 0;
        deleteLater();
        return;
    }
    {
        Atom acttype;
        int actfmt;
        unsigned long nbitem, bytes;
        unsigned char *data = 0;
        int ret;

        ret = XGetWindowProperty(dsp, mIconId, xfitMan().atom("_XEMBED_INFO"),
                                 0, 2, false, xfitMan().atom("_XEMBED_INFO"),
                                 &acttype, &actfmt, &nbitem, &bytes, &data);
        if (ret == Success)
        {
            if (data)
                XFree(data);
        }
        else
        {
            qWarning() << "TrayIcon: xembed error";
            XDestroyWindow(dsp, mWindowId);
            deleteLater();
            return;
        }
    }

    {
        XEvent e;
        e.xclient.type = ClientMessage;
        e.xclient.serial = 0;
        e.xclient.send_event = True;
        e.xclient.message_type = xfitMan().atom("_XEMBED");
        e.xclient.window = mIconId;
        e.xclient.format = 32;
        e.xclient.data.l[0] = CurrentTime;
        e.xclient.data.l[1] = XEMBED_EMBEDDED_NOTIFY;
        e.xclient.data.l[2] = 0;
        e.xclient.data.l[3] = mWindowId;
        e.xclient.data.l[4] = 0;
        XSendEvent(dsp, mIconId, false, 0xFFFFFF, &e);
    }

    XSelectInput(dsp, mIconId, StructureNotifyMask);
    mDamage = XDamageCreate(dsp, mIconId, XDamageReportRawRectangles);
    XCompositeRedirectWindow(dsp, mWindowId, CompositeRedirectManual);

    XMapWindow(dsp, mIconId);
    XMapRaised(dsp, mWindowId);

    const QSize req_size{mIconSize * metric(PdmDevicePixelRatio)};
    XResizeWindow(dsp, mIconId, req_size.width(), req_size.height());
}


/************************************************

 ************************************************/
TrayIcon::~TrayIcon()
{
    Display* dsp = mDisplay;
    XSelectInput(dsp, mIconId, NoEventMask);

    if (mDamage)
        XDamageDestroy(dsp, mDamage);

    // reparent to root
    xError = false;
    XErrorHandler old = XSetErrorHandler(windowErrorHandler);

    XUnmapWindow(dsp, mIconId);
    XReparentWindow(dsp, mIconId, QX11Info::appRootWindow(), 0, 0);

    if (mWindowId)
        XDestroyWindow(dsp, mWindowId);
    XSync(dsp, False);
    XSetErrorHandler(old);
}


/************************************************

 ************************************************/
QSize TrayIcon::sizeHint() const
{
    QMargins margins = contentsMargins();
    return QSize(margins.left() + mIconSize.width() + margins.right(),
                 margins.top() + mIconSize.height() + margins.bottom()
                );
}


/************************************************

 ************************************************/
void TrayIcon::setIconSize(QSize iconSize)
{
    mIconSize = iconSize;

    const QSize req_size{mIconSize * metric(PdmDevicePixelRatio)};
    if (mWindowId)
        xfitMan().resizeWindow(mWindowId, req_size.width()*2, req_size.height()*2);

    if (mIconId)
        xfitMan().resizeWindow(mIconId, req_size.width()*2, req_size.height()*2);
}

/*处理　TrayIcon　绘图，点击等事件*/
bool TrayIcon::event(QEvent *event)
{
    if (mWindowId)
    {
        switch (event->type())
        {
        case QEvent::Paint:
            /*必须在draw函数中绘制图标，若在paintEvent中绘制则XGetImage 会出错*/
            draw(static_cast<QPaintEvent*>(event));
            break;

        case QEvent::Move:
        case QEvent::Resize:
        {
            QRect rect = iconGeometry();
            xfitMan().moveWindow(mWindowId, rect.left(), rect.top());
        }
            break;

        case QEvent::MouseButtonPress:
//            trayButtonPress(static_cast<QMouseEvent*>(event));
//            break;
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            qDebug()<<"QEvent::MouseButtonDblClick";
            event->accept();
            break;

        default:
            break;
        }
    }

    return QToolButton::event(event);
}


/************************************************

 ************************************************/
QRect TrayIcon::iconGeometry()
{
    QRect res = QRect(QPoint(0, 0), mIconSize);

    res.moveCenter(QRect(0, 0, width(), height()).center());
    return res;
}

/**
 * @brief needReDraw 判断图标是否需要重新绘制（高亮处理）
 * @return 不需要重新绘制的图标返回值为false
 * 备注：ukui3.1 主题提供两套图标后，任务栏不需要进行重新绘制
 */
bool TrayIcon::needReDraw()
{
    QStringList ignoreAppList;
    ignoreAppList<<"kylin-video";
    if(ignoreAppList.contains(xfitMan().getApplicationName(mIconId))){
        return false;
    }
    return true;
}

/*draw 函数执行的是绘图事件*/
void TrayIcon::draw(QPaintEvent* /*event*/)
{
    Display* dsp = mDisplay;

    XWindowAttributes attr;
    if (!XGetWindowAttributes(dsp, mIconId, &attr))
    {
        qWarning() << "Paint error";
        return;
    }

    QImage image;
    XImage* ximage = XGetImage(dsp, mIconId, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    if(ximage)
    {
        image = QImage((const uchar*) ximage->data, ximage->width, ximage->height, ximage->bytes_per_line,  QImage::Format_ARGB32_Premultiplied);
    }
    else
    {
        qWarning() << "    * Error image is NULL";

        XClearArea(mDisplay, (Window)winId(), 0, 0, attr.width, attr.height, False);
        // for some unknown reason, XGetImage failed. try another less efficient method.
        // QScreen::grabWindow uses XCopyArea() internally.
        image = qApp->primaryScreen()->grabWindow(mIconId).toImage();
    }

    //    qDebug() << "Paint icon **************************************";
    //    qDebug() << "  * XComposite: " << isXCompositeAvailable();
    //    qDebug() << "  * Icon geometry:" << iconGeometry();
    //    qDebug() << "  Icon";
    //    qDebug() << "    * window id:  " << hex << mIconId;
    //    qDebug() << "    * window name:" << xfitMan().getName(mIconId);
    //    qDebug() << "    * size (WxH): " << attr.width << "x" << attr.height;
    //    qDebug() << "    * pos (XxY):  " << attr.x << attr.y;
    //    qDebug() << "    * color depth:" << attr.depth;
    //    qDebug() << "  XImage";
    //    qDebug() << "    * size (WxH):  " << ximage->width << "x" << ximage->height;
    //    switch (ximage->format)
    //    {
    //        case XYBitmap: qDebug() << "    * format:   XYBitmap"; break;
    //        case XYPixmap: qDebug() << "    * format:   XYPixmap"; break;
    //        case ZPixmap:  qDebug() << "    * format:   ZPixmap"; break;
    //    }
    //    qDebug() << "    * color depth:  " << ximage->depth;
    //    qDebug() << "    * bits per pixel:" << ximage->bits_per_pixel;

    QPainter painter(this);
    QRect iconRect = iconGeometry();
    if (image.size() != iconRect.size())
    {
        image = image.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect r = image.rect();
        r.moveCenter(iconRect.center());
        iconRect = r;
    }

    if(needReDraw())
        image=HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(image)).toImage();
    painter.drawImage(iconRect, image);

    if(ximage)
        XDestroyImage(ximage);
}

/*关于点击托盘应用的非图标区域实现打开托盘应用*/
void TrayIcon::trayButtonPress(QMouseEvent* /*event*/)
{
    //qDebug() << "    * window name:" << xfitMan().getApplicationName(mIconId);
    /* 需要此点击信号的应用需要做如下绑定
     * QDBusConnection::sessionBus().connect(QString(), QString("/traybutton/click"),  "com.ukui.panel.plugins.tray", "ClickTrayApp", this, SLOT(clientGet(void)));
     * 在槽函数clientGet(void)　中处理接受到的点击信号
     */
    QDBusMessage message =QDBusMessage::createSignal("/traybutton/click", "com.ukui.panel.plugins.tray", "ClickTrayApp");
    message<<xfitMan().getApplicationName(mIconId);
    QDBusConnection::sessionBus().send(message);
}

void TrayIcon::windowDestroyed(Window w)
{
    //damage is destroyed if it's parent window was destroyed
    if (mIconId == w)
        mDamage = 0;
}


/************************************************

 ************************************************/
bool TrayIcon::isXCompositeAvailable()
{
    int eventBase, errorBase;
    return XCompositeQueryExtension(QX11Info::display(), &eventBase, &errorBase );
}

/*enterEvent 和leaveEvent是用来处理traystatus的*/
void TrayIcon::enterEvent(QEvent *)
{
    traystatus=HOVER;
    update();
}

void TrayIcon::leaveEvent(QEvent *)
{
    traystatus=NORMAL;
    update();
}

void TrayIcon::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);

    switch(traystatus)
    {
    case NORMAL:
    {
        //                  p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
        p.setPen(Qt::NoPen);
        break;
    }
    case HOVER:
    {
        //                  p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x1f)));
        p.setPen(Qt::NoPen);
        break;
    }
    case PRESS:
    {
        //                  p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x0f)));
        p.setPen(Qt::NoPen);
        break;
    }
    }
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TrayIcon::notifyAppFreeze()
{
    emit notifyTray(mIconId);
}

#if 1
void TrayIcon::mouseMoveEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        return;
    if (!(e->buttons() & Qt::LeftButton))
        return;
    if ((e->pos() - mDragStart).manhattanLength() < QApplication::startDragDistance())
        return;

    if (e->modifiers() == Qt::ControlModifier)
    {
        return;
    }
    QDrag *drag = new QDrag(this);
    QIcon ico = icon();
    int size = 16;
    QPixmap img = ico.pixmap(ico.actualSize({size, size}));

    drag->setMimeData(mimeData());
    drag->setPixmap(img);

            drag->setHotSpot(img.rect().bottomRight());


    drag->exec();
    drag->deleteLater();

    QAbstractButton::mouseMoveEvent(e);
}

void TrayIcon::dragMoveEvent(QDragMoveEvent * e)
{
    if (e->mimeData()->hasFormat(MIMETYPE))
        e->acceptProposedAction();
    else
        e->ignore();
}

void TrayIcon::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
    const TrayButtonMimeData *mimeData = qobject_cast<const TrayButtonMimeData*>(e->mimeData());
    if (mimeData && mimeData->button())
          emit switchButtons(mimeData->button(), this);
    QToolButton::dragEnterEvent(e);
}

QMimeData * TrayIcon::mimeData()
{
    TrayButtonMimeData *mimeData = new TrayButtonMimeData();
//    QByteArray ba;
//    mimeData->setData(mimeDataFormat(), ba);
    mimeData->setButton(this);
    return mimeData;
}

void TrayIcon::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
    {
        mDragStart = e->pos();
        return;
    }

    QToolButton::mousePressEvent(e);
}
#endif
