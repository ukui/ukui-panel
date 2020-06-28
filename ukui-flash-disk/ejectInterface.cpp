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
#include <qgsettings.h>

ejectInterface::ejectInterface(QWidget *parent,QString mount_name) : QWidget(parent),eject_image_button(nullptr),show_text_label(nullptr),
    mount_name_label(nullptr)
{
//interface layout
    EjectScreen = qApp->primaryScreen();
    eject_image_button = new QPushButton();
    eject_image_button->setFixedSize(30,30);
    //QPixmap pixmap("kylin-media-removable-symbolic");
    eject_image_icon = QIcon::fromTheme("kylin-media-removable-symbolic");

    //add it to show the eject button
    if(eject_image_button)
    {
        eject_image_button->setIcon(eject_image_icon);
    }

    //set the size of the picture
    eject_image_button->setIconSize(QSize(25,25));

    show_text_label = new QLabel;
    //add the text of the eject interface
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
        ejectinterface_h_BoxLayout->addWidget(eject_image_button);
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

    //set the mian style of the ejecet interface
    this->setStyleSheet("QWidget{"
                        "background:rgba(19,19,20,1);"
                        "border:1px solid rgba(255, 255, 255, 0.05);"
                        "border-radius:6px;"
                        "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
                        "border-radius:6px"
                        "}");

    //set the style of the ejecet-image-button

    eject_image_button->setStyleSheet(
                //正常状态样式
                "QPushButton{"
                "color:rgba(255,255,255,1);"
                "line-height:24px;"
                "opacity:1;"
                "border:none;"
                "}"
                );
    //set the syle of show_text_label
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

    //set the main signal-slot function to complete the eject interface to let it disappear automatically
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

//slot function to hide eject interface
void ejectInterface::on_interface_hide()
{
    this->hide();
}

//set the location of the eject interface
void ejectInterface::moveEjectInterfaceRight()
{
//    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
//    {
//        qDebug()<<"the position of panel is down";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
//                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y() +
//                   EjectScreen->availableSize().height() - this->height() - DistanceToPanel);
//    }

//    if(EjectScreen->availableGeometry().x() < EjectScreen->availableGeometry().y() && EjectScreen->availableSize().height() < EjectScreen->size().height())
//    {
//        qDebug()<<"this position of panel is up";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->size().width() -
//                   this->width() - DistanceToPanel,EjectScreen->availableGeometry().y());
//    }

//    if(EjectScreen->availableGeometry().x() > EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
//    {
//        qDebug()<<"this position of panel is left";
//        this->move(EjectScreen->availableGeometry().x() + DistanceToPanel,EjectScreen->availableGeometry().y()
//                   + EjectScreen->availableSize().height() - this->height());
//    }

//    if(EjectScreen->availableGeometry().x() == EjectScreen->availableGeometry().y() && EjectScreen->availableSize().width() < EjectScreen->size().width())
//    {
//        qDebug()<<"this position of panel is right";
//        this->move(EjectScreen->availableGeometry().x() + EjectScreen->availableSize().width() -
//                   DistanceToPanel - this->width(),EjectScreen->availableGeometry().y() +
//                   EjectScreen->availableSize().height() - (this->height())*(DistanceToPanel - 1));
//    }

//show the ejectinterface by primaryscreen()
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

    if(position==0)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+QApplication::primaryScreen()->geometry().height()-panelSize-this->height(),this->width(),this->height()));
    else if(position==1)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+panelSize,this->width(),this->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        this->setGeometry(QRect(x+panelSize,y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
    else
        this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-this->width(),y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
}

int ejectInterface::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);

    return reply;
}

/*
    use the dbus to get the height of the panel
*/
int ejectInterface::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}
