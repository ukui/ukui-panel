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
#include <QImage>

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


//适配高分屏
#include<X11/Xlib.h>
#include<X11/keysym.h>
#include<X11/Xutil.h>
#include<X11/extensions/XTest.h>

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY                        "scaling-factor"


static bool xError;
#define MIMETYPE "ukui/UkuiTray"

#define CSETTINGS_SCALING "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALING_FACTOR    "scalingFactor"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_UKUI       "ukui"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"

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

    setAcceptDrops(false);

    QTimer::singleShot(200, [this] { init(); update(); });
    repaint();

    /**/
    QDBusConnection::sessionBus().unregisterService("com.ukui.panel");
    QDBusConnection::sessionBus().registerService("com.ukui.panel");
    QDBusConnection::sessionBus().registerObject("/traybutton/click", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    const QByteArray scaling_factor(CSETTINGS_SCALING);
    if(QGSettings::isSchemaInstalled(scaling_factor)) {
        scaling_settings = new QGSettings(scaling_factor);
        qDebug()<<"scaling_settings->get(SCALING_FACTOR).toInt()"<<scaling_settings->get(SCALING_FACTOR).toInt();
    }

    const QByteArray id(ORG_UKUI_STYLE);
    QStringList stylelist;
    stylelist<<STYLE_NAME_KEY_DARK<<STYLE_NAME_KEY_BLACK<<STYLE_NAME_KEY_DEFAULT<<STYLE_NAME_KEY_UKUI;
    if(QGSettings::isSchemaInstalled(id)){
        gsettings = new QGSettings(id);
        if(stylelist.contains(gsettings->get(STYLE_NAME).toString()))
            dark_style=true;
        else
            dark_style=false;
    }
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==STYLE_NAME){
            if(stylelist.contains(gsettings->get(STYLE_NAME).toString()))
                dark_style=true;
            else
                dark_style=false;
            repaint();
        }
    });

    const QByteArray System_Palette_id(FONT_RENDERING_DPI);
    if(QGSettings::isSchemaInstalled(System_Palette_id)){
        //this->update();
        System_scale_gsettings = new QGSettings(System_Palette_id);
        scale = System_scale_gsettings->get(SCALE_KEY).toInt();
        qDebug()<<"scale is "<<scale;
    }
    connect(System_scale_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key == "styleName")
        {
            scale = System_scale_gsettings->get(SCALE_KEY).toInt();
            qDebug()<<"scale is "<<scale;
            this->update();
        }
    });

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
        xfitMan().resizeWindow(mWindowId, req_size.width(), req_size.height());

    if (mIconId)
        xfitMan().resizeWindow(mIconId, req_size.width(), req_size.height());
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
            trayButtonCoordinateMapping(x_panel,y_panel);
//            break;
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            event->accept();
            break;
        case QEvent::ContextMenu:
            //            moveMenu();
            break;
        case QEvent::Enter:
            //            this->setToolTip("右键可选择移入任务栏/收纳");
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
        //        image=HighLightEffect::drawSymbolicColoredPixmap(QPixmap::fromImage(image)).toImage();
        image = drawSymbolicColoredPixmap(QPixmap::fromImage(image)).toImage();
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.setRenderHints(QPainter::Antialiasing, true);

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

/*关于点击托盘应用的非图标区域实现打开托盘应用*/
void TrayIcon::trayButtonCoordinateMapping(int x_click,int y_click)//QMouseEvent* /*event*/,
{
     XEvent event;
     Display *dpy = XOpenDisplay(NULL);
     if(dpy == NULL){
       return ;
     }
     /* get info about current pointer position */
     XQueryPointer(dpy, RootWindow(dpy, DefaultScreen(dpy)),
         &event.xbutton.root, &event.xbutton.window,
         &event.xbutton.x_root, &event.xbutton.y_root,
         &event.xbutton.x, &event.xbutton.y,
         &event.xbutton.state);
      qDebug()<<"x send click ";

      QPoint x_desktop=QCursor::pos();
      qDebug()<<"Tset"<<x_desktop;

      if (scale!=2)//scale 100
      {
          int mid_icon_click_w=width()/2;// tray icon position
          int mid_icon_click_h=height()/2;
           qDebug()<<"100  icon position"<<mid_icon_click_w<<mid_icon_click_h;
          if (x_click<=width()/2&&y_click<height()/2)
          {
              x=x_desktop.x()+qAbs(x_click-mid_icon_click_w);
              y=x_desktop.y()+qAbs(y_click-mid_icon_click_h);
              qDebug()<<"positon left_up x y "<<x<<y;
          }
          else if(x_click>width()/2&&y_click<height()/2)
          {
              x=x_desktop.x()-qAbs(x_click-mid_icon_click_w);
              y=x_desktop.y()+qAbs(y_click-mid_icon_click_h);
              qDebug()<<"position right_up x y "<<x<<y;
          }
          else if(x_click<width()/2&&y_click>height()/2)
          {
              x=x_desktop.x()+qAbs(x_click-mid_icon_click_w);
              y=x_desktop.y()-qAbs(y_click-mid_icon_click_h);
              qDebug()<<"position left_down x y "<<x<<y;
          }
          else if (x_click>width()/2&&y_click>=height()/2)
          {
              x=x_desktop.x()-qAbs(x_click-mid_icon_click_w);
              y=x_desktop.y()-qAbs(y_click-mid_icon_click_h);
              qDebug()<<"position right_down x y "<<x<<y;
          }
          else
          {
              qDebug()<<"no change click position";
          }
      }
      else
      {
          int mid_icon_click_w=width()/2;// tray icon position
          int mid_icon_click_h=height()/3;
           qDebug()<<"200  icon position"<<mid_icon_click_w<<mid_icon_click_h;
          if (x_click<=width()/2&&y_click<height()/3)
          {
              x=x_desktop.x()*2+qAbs(x_click*2-mid_icon_click_w);
              y=x_desktop.y()*2+qAbs(y_click*2-mid_icon_click_h);
              qDebug()<<"positon left_up x y "<<x<<y;
          }
          else if(x_click>width()/2&&y_click<height()/3)
          {
              x=x_desktop.x()*2-qAbs(x_click*2-mid_icon_click_w);
              y=x_desktop.y()*2-qAbs(y_click*2-mid_icon_click_h);
              qDebug()<<"position right_up x y "<<x<<y;
          }
          else if(x_click<width()/2&&y_click>height()/3)
          {
              x=x_desktop.x()*2+qAbs(x_click*2-mid_icon_click_w);
              y=x_desktop.y()*2-qAbs(y_click*2-mid_icon_click_h);
              qDebug()<<"position left_down x y "<<x<<y;
          }
          else if (x_click>width()/2&&y_click>=height()/3)
          {
              x=x_desktop.x()*2-qAbs(x_click*2-mid_icon_click_w);
              y=x_desktop.y()*2-qAbs(y_click*2-mid_icon_click_h);
              qDebug()<<"position right_down x y "<<x<<y;
          }
          else
          {
              qDebug()<<"no change click position";
          }
      }

      XTestFakeMotionEvent(dpy, -1, x, y, 0);
      XTestFakeButtonEvent(dpy, 1, 1, 0);
      XTestFakeButtonEvent(dpy, 1, 0, 0);
      QElapsedTimer t;
      t.start();
      while(t.elapsed()<200);
      XSetInputFocus(mDisplay, mWindowId,RevertToPointerRoot,CurrentTime);
      /* place the mouse where it was */
      XTestFakeMotionEvent(dpy, -1, event.xbutton.x, event.xbutton.y, 0);
       qDebug()<<"event.xbutton.x, event.xbutton.y"<<event.xbutton.x<< event.xbutton.y;
      XCloseDisplay(dpy);

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
        //                          p.setBrush(QBrush(QColor(0x00,0xFF,0x00,0x19)));
        p.setPen(Qt::NoPen);
        break;
    }
    case HOVER:
    {
        p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x1f)));
        p.setPen(Qt::NoPen);
        break;
    }
    case PRESS:
    {
        p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x0f)));
        p.setPen(Qt::NoPen);
        break;
    }
    }
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    const QSize req_size{mIconSize * metric(PdmDevicePixelRatio)};
    xfitMan().resizeWindow(mWindowId, req_size.width(), req_size.height());
    p.drawRoundedRect(opt.rect.x(),opt.rect.y(),width(),height(),8,8);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QPixmap TrayIcon::drawSymbolicColoredPixmap(const QPixmap &source)
{
    QColor standard (31,32,34);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if(dark_style && qAbs(color.red()-standard.red())<20 && qAbs(color.green()-standard.green())<20 && qAbs(color.blue()-standard.blue())<20){
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    //                    color.setRgb(255,255,255);
                    img.setPixelColor(x, y, color);
                }
                else{
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

void TrayIcon::moveMenu()
{

    menu =new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    QAction *action;
    action = menu->addAction(tr("移入任务栏/收纳"));
    connect(action, &QAction::triggered, [this] { emit iconIsMoving(mIconId);});
    menu->setGeometry(caculateMenuWindowPos(QCursor::pos(), menu->sizeHint()));
    menu->show();
}

void TrayIcon::notifyAppFreeze()
{
    emit notifyTray(mIconId);
}

void TrayIcon::emitIconId()
{
    emit iconIsMoving(mIconId);
}

QRect TrayIcon::caculateMenuWindowPos(const QPoint &absolutePos, const QSize &windowSize)
{
    int x = absolutePos.x(), y = absolutePos.y();

    QRect res(QPoint(x, y), windowSize);

    QRect screen = QApplication::desktop()->screenGeometry(this);
    // NOTE: We cannot use AvailableGeometry() which returns the work area here because when in a
    // multihead setup with different resolutions. In this case, the size of the work area is limited
    // by the smallest monitor and may be much smaller than the current screen and we will place the
    // menu at the wrong place. This is very bad for UX. So let's use the full size of the screen.
    if (res.right() > screen.right())
        res.moveRight(screen.right());

    if (res.bottom() > screen.bottom())
        res.moveBottom(screen.bottom());

    if (res.left() < screen.left())
        res.moveLeft(screen.left());

    if (res.top() < screen.top())
        res.moveTop(screen.top());

    return res;
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
    emit iconIsMoving(mIconId);
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

    QPoint panel_click= e->pos();
    x_panel=panel_click.x();
    y_panel=panel_click.y();
    trayButtonCoordinateMapping(x_panel,y_panel);

    QToolButton::mousePressEvent(e);
}
#endif
