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
#include "ejectInterface.h"

ejectInterface::ejectInterface(QWidget *parent,QString mount_name) : QWidget(parent),eject_image_label(nullptr),show_text_label(nullptr),
    mount_name_label(nullptr)
{
//interface layout
    EjectScreen = qApp->primaryScreen();
    eject_image_label = new QLabel();
    QPixmap pixmap(":/picture/tip-32.svg");
    if(eject_image_label)
    {
        eject_image_label->setPixmap(pixmap);
    }

    show_text_label = new QLabel;
    if(show_text_label)
    {
        show_text_label->setText(tr("usb has been unplugged safely"));
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::white);
        show_text_label->setPalette(pe);
    }

    ejectinterface_h_BoxLayout = new QHBoxLayout();
    if(ejectinterface_h_BoxLayout)
    {
        ejectinterface_h_BoxLayout->addStretch();
        ejectinterface_h_BoxLayout->addWidget(eject_image_label);
        ejectinterface_h_BoxLayout->addWidget(show_text_label);
        ejectinterface_h_BoxLayout->addStretch();
    }
    mountname_h_BoxLayout = new QHBoxLayout();
    mount_name_label = new QLabel();
    if(mount_name_label)
    {
        mount_name_label->setText(mount_name);
    }
    mountname_h_BoxLayout->addStretch();
    mountname_h_BoxLayout->addWidget(mount_name_label);
    mountname_h_BoxLayout->addStretch();
    main_V_BoxLayput = new QVBoxLayout;
    main_V_BoxLayput->addLayout(ejectinterface_h_BoxLayout);
    main_V_BoxLayput->addLayout(mountname_h_BoxLayout);

    this->resize(210,86);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明

    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(0,0,-0,-0);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));

    this->setStyleSheet("QWidget{"
                        "background:rgba(19,19,20,1);"
                        "border:1px solid rgba(255, 255, 255, 0.05);"
                        "border-radius:6px;"
                        "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
                        "border-radius:6px"
                        "}");
    eject_image_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "color:rgba(255,255,255,1);"
                "line-height:24px;"
                "opacity:1;"
                "border:none;"
                "}"
                );
    show_text_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "font-size:16px;"
                "font-family:NotoSansCJKsc-Regular;"
                "font-weight:400;"
                "color:rgba(255,255,255,1);"
                "line-height:24px;"
                "opacity:1;"
                "border:none;"
                "}");
    mount_name_label->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "width:93px;"
                "height:15px;"
                "font-size:14px;"
                "font-family:NotoSansCJKsc-Regular;"
                "font-weight:400;"
                "color:rgba(255,255,255,0.35);"
                "line-height:28px;"
                "opacity:0.35;"
                "border:none;"
                "}");
    this->setLayout(main_V_BoxLayput);

    interfaceHideTime = new QTimer(this);
    interfaceHideTime->setTimerType(Qt::PreciseTimer);
    connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_interface_hide()));
    interfaceHideTime->start(1000);
    moveEjectInterfaceRight();
}

ejectInterface::~ejectInterface()
{

}


//If the fillet does not take effect
void ejectInterface::paintEvent(QPaintEvent *event)
 {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
 }

void ejectInterface::on_interface_hide()
{
    this->hide();
}

void ejectInterface::moveEjectInterfaceRight()
{
    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
    {
        qDebug()<<"the positon of panel is down";
        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y() +
                   EjectScreen->availableSize().height() - this->height() - DistanceToPanel);
    }

    if(EjectScreen->availableGeometry().x() < EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
    {
        qDebug()<<"this position of panel is up";
        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y());
    }

    if(EjectScreen->availableGeometry().x() > EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
    {
        qDebug()<<"this position of panel is left";
        this->move(EjectScreen->availableGeometry().x() + DistanceToPanel,EjectScreen->availableGeometry().y()
                   + EjectScreen->availableSize().height() - this->height());
    }

    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
    {
        qDebug()<<"this position of panel is right";
        this->move(EjectScreen->availableGeometry().x() + EjectScreen->availableSize().width() -
                   DistanceToPanel - this->width(),EjectScreen->availableGeometry().y() +
                   EjectScreen->availableSize().height() - (this->height())*(DistanceToPanel - 1));
    }
}
