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

#include "gpartedinterface.h"
#include <KWindowEffects>

gpartedInterface::gpartedInterface(QWidget *parent):QWidget(parent)
{
    this->setFixedSize(300,86);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground);
    initWidgets();
    moveChooseDialogRight();
    initTransparentState();
    getTransparentData();
}

void gpartedInterface::initTransparentState()
{
    const QByteArray idtrans(THEME_QT_TRANS);

    if(QGSettings::isSchemaInstalled(idtrans))
    {
        m_transparency_gsettings = new QGSettings(idtrans);
    }
}

void gpartedInterface::getTransparentData()
{
    if (!m_transparency_gsettings)
    {
       m_transparency = 0.95;
       return;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency"))
    {
        m_transparency = m_transparency_gsettings->get("transparency").toDouble();
    }
}

void gpartedInterface::initWidgets()
{
    noticeLabel = new QLabel();
    noticeLabel->setText(tr("gparted has started,can not eject"));
    okButton = new QPushButton(tr("ok"));
    notice_H_BoxLayout = new QHBoxLayout();
    main_V_BoxLayout = new QVBoxLayout();
    button_H_BoxLayout = new QHBoxLayout();
    notice_H_BoxLayout->addWidget(noticeLabel);
    button_H_BoxLayout->addWidget(okButton,1,Qt::AlignRight);
    main_V_BoxLayout->addLayout(notice_H_BoxLayout);
    main_V_BoxLayout->addLayout(button_H_BoxLayout);
    this->setLayout(main_V_BoxLayout);
    connect(okButton,SIGNAL(clicked()),this,SLOT(close()));
}

void gpartedInterface::moveChooseDialogRight()
{
    int position=0;
    int panelSize=0;
    if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
    {
        QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
        if(gsetting->keys().contains(QString("panelposition")))
            position=gsetting->get("panelposition").toInt();
        else
            position=0;
        if(gsetting->keys().contains(QString("panelsize")))
            panelSize=gsetting->get("panelsize").toInt();
        else
            panelSize=SmallPanelSize;
    }
    else
    {
        position=0;
        panelSize=SmallPanelSize;
    }

    int x=QApplication::primaryScreen()->geometry().x();
    int y=QApplication::primaryScreen()->geometry().y();
    qDebug()<<"QApplication::primaryScreen()->geometry().x();"<<QApplication::primaryScreen()->geometry().x();
    qDebug()<<"QApplication::primaryScreen()->geometry().y();"<<QApplication::primaryScreen()->geometry().y();
    if(position==0)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width() - DISTANCEPADDING - DISTANCEMEND,y+QApplication::primaryScreen()->geometry().height()-panelSize-this->height() - DISTANCEPADDING,this->width(),this->height()));
    else if(position==1)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width() - DISTANCEPADDING - DISTANCEMEND,y+panelSize + DISTANCEPADDING,this->width(),this->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        this->setGeometry(QRect(x+panelSize + DISTANCEPADDING,y + QApplication::primaryScreen()->geometry().height() - this->height() - DISTANCEPADDING,this->width(),this->height()));
    else
        this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-this->width() - DISTANCEPADDING,y + QApplication::primaryScreen()->geometry().height() - this->height() - DISTANCEPADDING,this->width(),this->height()));
}
gpartedInterface::~gpartedInterface()
{

}

void gpartedInterface::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(1, 1, -1, -1);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    QRect rectReal = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rectReal, 6, 6);
    QWidget::paintEvent(event);

    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));
}
