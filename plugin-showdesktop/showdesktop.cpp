/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QAction>
#include <QtX11Extras/QX11Info>
#include <QStyleOption>
#include <QPainter>
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/NETWM>
#include "showdesktop.h"
#include "../panel/common/ukuinotification.h"
#include "../panel/pluginsettings.h"
#include "../panel/highlight-effect.h"

#define DEFAULT_SHORTCUT "Control+Alt+D"

#define DESKTOP_WIDTH   (50)
#define DESKTOP_WIDGET_HIGHT 100

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"

#define TRAY_ICON_COLOR_LOGHT      255
#define TRAY_ICON_COLOR_DRAK       0

ShowDesktop::ShowDesktop(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QWidget(),
    IUKUIPanelPlugin(startupInfo)
{
    state=NORMAL;
    this->setToolTip(tr("Show Desktop"));

    const QByteArray id(ORG_UKUI_STYLE);
    QStringList stylelist;
    stylelist<<STYLE_NAME_KEY_DARK<<STYLE_NAME_KEY_BLACK<<STYLE_NAME_KEY_DEFAULT;
    if(QGSettings::isSchemaInstalled(id)){
        gsettings = new QGSettings(id);
        if(stylelist.contains(gsettings->get(STYLE_NAME).toString()))
            tray_icon_color=TRAY_ICON_COLOR_LOGHT;
        else
            tray_icon_color=TRAY_ICON_COLOR_DRAK;
        }
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==STYLE_NAME){
            if(stylelist.contains(gsettings->get(STYLE_NAME).toString())){
                tray_icon_color=TRAY_ICON_COLOR_LOGHT;
            }
            else
                tray_icon_color=TRAY_ICON_COLOR_DRAK;
        }
    });

    realign();

}

void ShowDesktop::realign()
{
 if(panel()->isHorizontal())
    {
        this->setFixedSize(panel()->panelSize(),panel()->panelSize());
        xEndPoint=0;
        yEndPoint=100;
    }
    else
    {
       this->setFixedSize(panel()->panelSize(),panel()->panelSize());
        xEndPoint=100;
        yEndPoint=0;
    }
}

void ShowDesktop::mousePressEvent(QMouseEvent *)
{
    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
}

void ShowDesktop::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
     opt.init(this);
 //change li
     QPainter p(this);
     QPainter painter(this);
     QPixmap pix;

     /*设置画笔的颜色，此处画笔作用与Line，所以必须在drawLine　之前调用*/
     //p.setPen(QColor(0x62,0x6C,0x6E,0xcc));
     //p.setOpacity(1.0);
     switch (state) {
     case NORMAL:

       //  p.drawLine(0,0,xEndPoint,yEndPoint);
         pix.load("/usr/share/ukui-panel/panel/img/deskup.png");
         painter.drawPixmap(panel()->panelSize()/3,panel()->panelSize()/3,panel()->panelSize()/3,panel()->panelSize()/3,drawSymbolicColoredPixmap(pix));
         break;
     case HOVER:
         //p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x0f)));
        // p.drawLine(0,0,xEndPoint,yEndPoint);
         pix.load("/usr/share/ukui-panel/panel/img/deskdown.svg");
         drawSymbolicColoredPixmap(pix);
         painter.drawPixmap(panel()->panelSize()/3,panel()->panelSize()/3,panel()->panelSize()/3,panel()->panelSize()/3,drawSymbolicColoredPixmap(pix));

         break;
     default:
         break;
     }
     p.setRenderHint(QPainter::Antialiasing);
    // p.drawRect(opt.rect);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

void ShowDesktop::enterEvent(QEvent *event)
{
    state=HOVER;
    update();
}

void ShowDesktop::leaveEvent(QEvent *event)
{
    state=NORMAL;
    update();
}

QPixmap ShowDesktop::drawSymbolicColoredPixmap(const QPixmap &source)
{
    QColor gray(128,128,128);
    QColor standard (31,32,34);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<255 && qAbs(color.green()-gray.green())<255 && qAbs(color.blue()-gray.blue())<255) {
//                    qDebug()<<"tray_icon_colopr"<<tray_icon_color;
                    color.setRed(tray_icon_color);
                    color.setGreen(tray_icon_color);
                    color.setBlue(tray_icon_color);
                    img.setPixelColor(x, y, color);
                }
                else if(qAbs(color.red()-standard.red())<20 && qAbs(color.green()-standard.green())<20 && qAbs(color.blue()-standard.blue())<20)
                {
                    color.setRed(tray_icon_color);
                    color.setGreen(tray_icon_color);
                    color.setBlue(tray_icon_color);
                    img.setPixelColor(x, y, color);
                }
                else
                {
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}
#undef DEFAULT_SHORTCUT
