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
//Qt
#include <QAction>
#include <QtX11Extras/QX11Info>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>
//Kf5
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/NETWM>
#include <QApplication>
//panel
#include "showdesktop.h"
#include "../panel/pluginsettings.h"

#define DEFAULT_SHORTCUT "Control+Alt+D"

#define DESKTOP_WIDTH   (12)
#define DESKTOP_WIDGET_HIGHT 100

ShowDesktop::ShowDesktop(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QWidget(),
    IUKUIPanelPlugin(startupInfo)
{
    translator();
    m_state=NORMAL;
    this->setToolTip(tr("Show Desktop"));
    realign();
    this->setContextMenuPolicy(Qt::PreventContextMenu);

}

void ShowDesktop::translator(){
    m_translator = new QTranslator(this);
     QString locale = QLocale::system().name();
     if (locale == "zh_CN"){
         if (m_translator->load(QM_INSTALL))
             qApp->installTranslator(m_translator);
         else
             qDebug() <<PLUGINNAME<<"Load translations file" << locale << "failed!";
     }
}

void ShowDesktop::realign()
{
    if(panel()->isHorizontal())
    {
        this->setFixedSize(DESKTOP_WIDTH,panel()->panelSize());
        m_xEndPoint=0;
        m_yEndPoint=100;
    }
    else
    {
       this->setFixedSize(panel()->panelSize(),DESKTOP_WIDTH);
        m_xEndPoint=100;
        m_yEndPoint=0;
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
    QPainter p(this);
    /*设置画笔的颜色，此处画笔作用与Line，所以必须在drawLine　之前调用*/
    p.setPen(QColor(0x62,0x6C,0x6E,0xcc));
    switch (m_state) {
    case NORMAL:

        p.drawLine(0,0,m_xEndPoint,m_yEndPoint);
        break;
    case HOVER:
        p.setBrush(QBrush(QColor(0xff,0xff,0xff,0x0f)));
        p.drawLine(0,0,m_xEndPoint,m_yEndPoint);
        break;
    default:
        break;
    }
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRect(opt.rect);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ShowDesktop::enterEvent(QEvent *event)
{
    m_state=HOVER;
    update();
}

void ShowDesktop::leaveEvent(QEvent *event)
{
    m_state=NORMAL;
    update();
}
#undef DEFAULT_SHORTCUT
