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
#include "qclickwidget.h"

QClickWidget::QClickWidget(QWidget *parent,
                           int num,
                           QString driveName,
                           QString nameDis1,
                           QString nameDis2,
                           QString nameDis3,
                           QString nameDis4,
                           qlonglong capacityDis1,
                           qlonglong capacityDis2,
                           qlonglong capacityDis3,
                           qlonglong capacityDis4,
                           QString pathDis1,
                           QString pathDis2,
                           QString pathDis3,
                           QString pathDis4)
    : QWidget(parent),
      m_Num(num),
      m_driveName(driveName),
      m_nameDis1(nameDis1),
      m_nameDis2(nameDis2),
      m_nameDis3(nameDis3),
      m_nameDis4(nameDis4),
      m_capacityDis1(capacityDis1),
      m_capacityDis2(capacityDis2),
      m_capacityDis3(capacityDis3),
      m_capacityDis4(capacityDis4),
      m_pathDis1(pathDis1),
      m_pathDis2(pathDis2),
      m_pathDis3(pathDis3),
      m_pathDis4(pathDis4)

{
//union layout
        QHBoxLayout *drivename_H_BoxLayout = new QHBoxLayout();
        drivename_H_BoxLayout = new QHBoxLayout();
        image_show_label = new QLabel();
        QPixmap pixmap(":picture/drive-removable-media-usb.png");
        image_show_label->setPixmap(pixmap);
        image_show_label->setFixedSize(40,40);
        m_driveName_label = new QLabel();
        m_driveName_label->setFont(QFont("Noto Sans CJK SC",14));
        QString DriveName = getElidedText(m_driveName_label->font(), m_driveName, 180);
        m_driveName_label->setText(DriveName);
        m_driveName_label->setFixedSize(180,40);
        m_driveName_label->setObjectName("driveNameLabel");

        m_eject_button = new QPushButton(this);
        m_eject_button->setFlat(true);
        m_eject_button->move(m_eject_button->x()+234,m_eject_button->y()+2);
        //->setObjectName("Button");
        m_eject_button->installEventFilter(this);
        QIcon eject_icon(":picture/media-eject-symbolic.svg");
        m_eject_button->setFixedSize(40,40);
        m_eject_button->setIcon(eject_icon);
        m_eject_button->setToolTip(tr("弹出"));

        drivename_H_BoxLayout->addSpacing(8);
        drivename_H_BoxLayout->addWidget(image_show_label);
        drivename_H_BoxLayout->addWidget(m_driveName_label);
        drivename_H_BoxLayout->addStretch();

        QVBoxLayout *main_V_BoxLayout = new QVBoxLayout();
        main_V_BoxLayout->setContentsMargins(0,0,0,0);

        connect(m_eject_button,SIGNAL(clicked()),SLOT(switchWidgetClicked()));
//when the drive only has one vlolume
        if(m_Num == 1)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel(this);
            m_nameDis1_label->setFont(QFont("Microsoft YaHei",14));
            //m_nameDis1_label->setFixedSize(120,18);
            QString VolumeName = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
            m_nameDis1_label->adjustSize();
            qDebug()<<m_nameDis1_label->width()<<"-----------------------"<<m_nameDis1_label->height();
            m_nameDis1_label->setText("- "+VolumeName+":");

            m_nameDis1_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel"
                                    "{"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            //m_nameDis1_label->setAlignment(Qt::AlignCenter);
            m_nameDis1_label->installEventFilter(this);

            //m_driveName_label->setText(VolumeName);
            //m_nameDis1_label->setFixedWidth(140);
            m_capacityDis1_label = new QLabel();
            //m_capacityDis1_label->setFixedSize(60,18);
            QString str_capacityDis1 = size_human(m_capacityDis1);
            //m_capacityDis1_label->setFont(QFont("Microsoft YaHei",14));
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            //m_capacityDis1_label->setAlignment(Qt::AlignLeft);
            //m_capacityDis1_label->setFixedWidth(70);
            m_capacityDis1_label->setObjectName("capacityLabel");
            //m_capacityDis1_label->adjustSize();
            qDebug()<<m_capacityDis1_label->width()<<"++++++++++++++++++++++++"<<m_capacityDis1_label->height();

            onevolume_h_BoxLayout->addSpacing(40);
            onevolume_h_BoxLayout->setSpacing(0);
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
            onevolume_h_BoxLayout->addStretch();
            disWidgetNumOne = new QWidget;
            //setObjectName to add stylesheet for parent object only
            disWidgetNumOne->setObjectName("OriginObjectOnly");
            disWidgetNumOne->setFixedHeight(32);
            disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
            disWidgetNumOne->installEventFilter(this);

            main_V_BoxLayout->setContentsMargins(0,0,0,0);
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addWidget(disWidgetNumOne);

            this->setLayout(main_V_BoxLayout);
            this->setFixedSize(276,68);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
        }
//when the drive has two volumes
        if(m_Num == 2)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel(this);
            m_nameDis1_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
            m_nameDis1_label->adjustSize();
            m_nameDis1_label->setText("- "+VolumeNameDis1+":");
            m_nameDis1_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );

            //m_nameDis1_label->setAlignment(Qt::AlignRight);
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
//            m_capacityDis1_label->setAlignment(Qt::AlignLeft);
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addSpacing(40);
            onevolume_h_BoxLayout->setSpacing(0);
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
//            QVBoxLayout *capacityDis1_V_BoxLayout = new QVBoxLayout;
//            capacityDis1_V_BoxLayout->addSpacing(2);
//            capacityDis1_V_BoxLayout->addWidget(m_capacityDis1_label);
//            onevolume_h_BoxLayout->addLayout(capacityDis1_V_BoxLayout);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
            onevolume_h_BoxLayout->addStretch();
            disWidgetNumOne = new QWidget;
            disWidgetNumOne->setFixedHeight(32);
            disWidgetNumOne->setObjectName("OriginObjectOnly");
            disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
            disWidgetNumOne->installEventFilter(this);

            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel(this);
            m_nameDis2_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
            m_nameDis2_label->setText("- "+VolumeNameDis2+":");
            m_nameDis2_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis2_label->adjustSize();
            m_nameDis2_label->installEventFilter(this);
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
//            m_capacityDis2_label->setAlignment(Qt::AlignLeft);
            m_capacityDis2_label->setObjectName("capacityLabel");
            twovolume_h_BoxLayout->addSpacing(40);
            twovolume_h_BoxLayout->setSpacing(0);
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
//            QVBoxLayout *capacityDis2_V_BoxLayout = new QVBoxLayout;
//            capacityDis2_V_BoxLayout->addWidget(m_capacityDis2_label);
//            twovolume_h_BoxLayout->addLayout(capacityDis2_V_BoxLayout);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
            twovolume_h_BoxLayout->addStretch();
            disWidgetNumTwo = new QWidget;
            disWidgetNumTwo->setFixedHeight(32);
            disWidgetNumTwo->setObjectName("OriginObjectOnly");
            disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
            disWidgetNumTwo->installEventFilter(this);

            main_V_BoxLayout->setContentsMargins(0,0,0,0);
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addWidget(disWidgetNumOne);
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
            main_V_BoxLayout->addStretch();
            this->setLayout(main_V_BoxLayout);
            this->setFixedSize(276,97);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
        }
//when the drive has three volumes
        if(m_Num == 3)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel(this);
            m_nameDis1_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis2, 120);
            m_nameDis1_label->setText("- "+VolumeNameDis1+":");
            m_nameDis1_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis1_label->adjustSize();
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
//            m_capacityDis1_label->setAlignment(Qt::AlignLeft);
            onevolume_h_BoxLayout->addSpacing(40);
            onevolume_h_BoxLayout->setSpacing(0);
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
            onevolume_h_BoxLayout->addStretch();

            disWidgetNumOne = new QWidget;
            disWidgetNumOne->setFixedHeight(32);
            disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
            disWidgetNumOne->setObjectName("OriginObjectOnly");
            disWidgetNumOne->installEventFilter(this);

            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel(this);
            m_nameDis2_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
            m_nameDis2_label->setText("- "+VolumeNameDis2+":");
            m_nameDis2_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis2_label->adjustSize();
            m_nameDis2_label->installEventFilter(this);
            //m_nameDis2_label->setText("- "+m_nameDis2+":");
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
            m_capacityDis2_label->setObjectName("capacityLabel");
//            m_capacityDis2_label->setAlignment(Qt::AlignLeft);
            twovolume_h_BoxLayout->addSpacing(40);
            twovolume_h_BoxLayout->setSpacing(0);
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
            twovolume_h_BoxLayout->addStretch();

            disWidgetNumTwo = new QWidget;
            disWidgetNumTwo->setFixedHeight(32);
            disWidgetNumTwo->setObjectName("OriginObjectOnly");
            disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
            disWidgetNumTwo->installEventFilter(this);

            QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis3_label = new ClickLabel(this);
            m_nameDis3_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
            m_nameDis3_label->setText("- "+VolumeNameDis3+":");
            m_nameDis3_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis3_label->adjustSize();
            m_nameDis3_label->installEventFilter(this);
            //m_nameDis3_label->setText("- "+m_nameDis3+":");
            m_capacityDis3_label = new QLabel();
            QString str_capacityDis3 = size_human(m_capacityDis3);
            m_capacityDis3_label->setText("("+str_capacityDis3+")");
            m_capacityDis3_label->setObjectName("capacityLabel");
            threevolume_h_BoxLayout->addSpacing(40);
            threevolume_h_BoxLayout->setSpacing(0);
            threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
            threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
            threevolume_h_BoxLayout->addStretch();

            disWidgetNumThree = new QWidget;
            disWidgetNumThree->setFixedHeight(32);
            disWidgetNumThree->setObjectName("OriginObjectOnly");
            disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
            disWidgetNumThree->installEventFilter(this);


            main_V_BoxLayout->setContentsMargins(0,0,0,0);
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addWidget(disWidgetNumOne);
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
            main_V_BoxLayout->addWidget(disWidgetNumThree);

            this->setLayout(main_V_BoxLayout);
            this->setFixedSize(276,136);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
//            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
        }
//when the drive has four volumes
        if(m_Num == 4)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel(this);
            m_nameDis1_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
            m_nameDis1_label->setText("- "+VolumeNameDis1+":");
            m_nameDis1_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis1_label->adjustSize();
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addSpacing(40);
            onevolume_h_BoxLayout->setSpacing(0);
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
            onevolume_h_BoxLayout->addStretch();

            disWidgetNumOne = new QWidget;
            disWidgetNumOne->setFixedHeight(32);
            disWidgetNumOne->setObjectName("OriginObjectOnly");
            disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
            disWidgetNumOne->installEventFilter(this);

            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel(this);
            m_nameDis2_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
            m_nameDis2_label->setText("- "+VolumeNameDis2+":");
            m_nameDis2_label->installEventFilter(this);
            m_nameDis2_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Micrifosoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis2_label->adjustSize();
            m_nameDis2_label->installEventFilter(this);
            //m_nameDis2_label->setText("- "+m_nameDis2+":");
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
            m_capacityDis2_label->setObjectName("capacityLabel");
            twovolume_h_BoxLayout->addSpacing(40);
            twovolume_h_BoxLayout->setSpacing(0);
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
            twovolume_h_BoxLayout->addStretch();

            disWidgetNumTwo = new QWidget;
            disWidgetNumTwo->setFixedHeight(32);
            disWidgetNumTwo->setObjectName("OriginObjectOnly");
            disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
            disWidgetNumTwo->installEventFilter(this);

            QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis3_label = new ClickLabel(this);
            m_nameDis3_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
            m_nameDis3_label->setText("- "+VolumeNameDis3+":");
            m_nameDis3_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis3_label->installEventFilter(this);
            //m_nameDis3_label->setText("- "+m_nameDis3+":");
            m_capacityDis3_label = new QLabel();
            QString str_capacityDis3 = size_human(m_capacityDis3);
            m_capacityDis3_label->setText("("+str_capacityDis3+")");
            m_capacityDis3_label->setObjectName("capacityLabel");
            threevolume_h_BoxLayout->addSpacing(40);
            threevolume_h_BoxLayout->setSpacing(0);
            threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
            threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
            threevolume_h_BoxLayout->addStretch(0);

            disWidgetNumThree = new QWidget;
            disWidgetNumThree->setFixedHeight(32);
            disWidgetNumThree->setObjectName("OriginObjectOnly");
            disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
            disWidgetNumThree->installEventFilter(this);

            QHBoxLayout *fourvolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis4_label = new ClickLabel(this);
            m_nameDis4_label->setFont(QFont("Microsoft YaHei",14));
            QString VolumeNameDis4 = getElidedText(m_nameDis4_label->font(), m_nameDis4, 120);
            m_nameDis4_label->setText("- "+VolumeNameDis4+":");
            m_nameDis4_label->setStyleSheet(
                                    //正常状态样式
                                    "QLabel{"
                                    "width:111px;"
                                    "height:14px;"
                                    "font-size:14px;"
                                    "font-family:Microsoft YaHei;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.35);"
                                    "line-height:28px;"
                                    "opacity:0.35;"
                                    "}"
                                    );
            m_nameDis4_label->adjustSize();
            m_nameDis4_label->installEventFilter(this);
            //m_nameDis4_label->setText("- "+m_nameDis4+":");
            m_capacityDis4_label = new QLabel();
            QString str_capacityDis4 = size_human(m_capacityDis4);
            m_capacityDis4_label->setText("("+str_capacityDis4+")");
            m_capacityDis4_label->setObjectName("capacityLabel");
            fourvolume_h_BoxLayout->addSpacing(40);
            fourvolume_h_BoxLayout->setSpacing(0);
            fourvolume_h_BoxLayout->addWidget(m_nameDis4_label);
            fourvolume_h_BoxLayout->addWidget(m_capacityDis4_label);
            fourvolume_h_BoxLayout->addStretch();

            disWidgetNumFour = new QWidget;
            disWidgetNumFour->setFixedHeight(32);
            disWidgetNumFour->setObjectName("OriginObjectOnly");
            disWidgetNumFour->setLayout(fourvolume_h_BoxLayout);
            disWidgetNumFour->installEventFilter(this);

            main_V_BoxLayout->setContentsMargins(0,0,0,0);
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addWidget(disWidgetNumOne);
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
            main_V_BoxLayout->addWidget(disWidgetNumThree);
            main_V_BoxLayout->addWidget(disWidgetNumFour);
            this->setLayout(main_V_BoxLayout);
            this->setFixedSize(276,165);
//            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
//            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
//            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
//            connect(m_nameDis4_label,SIGNAL(clicked()),this,SLOT(on_volume4_clicked()));
        }
        //connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));

    }




QClickWidget::~QClickWidget()
{

}

QString QClickWidget::getElidedText(QFont font, QString str, int MaxWidth)
{
    if (str.isEmpty())
    {
        return "";
    }

    QFontMetrics fontWidth(font);

    //计算字符串宽度
    //calculat the width of the string
    int width = fontWidth.width(str);

    //当字符串宽度大于最大宽度时进行转换
    //Convert when string width is greater than maximum width
    if (width >= MaxWidth)
    {
        //右部显示省略号
        str = fontWidth.elidedText(str, Qt::ElideRight, MaxWidth);
    }
    //返回处理后的字符串
    return str;
}

void QClickWidget::mouseClicked()
{
    //处理代码
//        std::string str = m_path.toStdString();
//        const char* ch = str.c_str();
    QProcess::startDetached("peony "+m_pathDis1);
    this->topLevelWidget()->hide();
}

void QClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) Q_EMIT clicked();
}

//void QClickWidget::paintEvent(QPaintEvent *)
// {
//     //解决QClickWidget类设置样式不生效的问题
//     QStyleOption opt;
//     opt.init(this);
//     QPainter p(this);
//     p.setBrush(QColor(0x00,0x00,0x00));
//     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
// }

void QClickWidget::on_volume1_clicked()
{
    QProcess::startDetached("peony "+m_pathDis1);
    this->topLevelWidget()->hide();
}

void QClickWidget::on_volume2_clicked()
{
    QProcess::startDetached("peony "+m_pathDis2);
    this->topLevelWidget()->hide();
}

void QClickWidget::on_volume3_clicked()
{
    QProcess::startDetached("peony "+m_pathDis3);
    this->topLevelWidget()->hide();
}

void QClickWidget::on_volume4_clicked()
{
    QProcess::startDetached("peony "+m_pathDis4);
    this->topLevelWidget()->hide();
}

void QClickWidget::switchWidgetClicked()
{
    qDebug()<<"出发信号";
    Q_EMIT clickedConvert();

}

QString QClickWidget::size_human(qlonglong capacity)
{
    //    float capacity = this->size();
        QStringList list;
        list << "KB" << "MB" << "GB" << "TB";

        QStringListIterator i(list);
        QString unit("bytes");

        while(capacity >= 1024.0 && i.hasNext())
        {
            unit = i.next();
            capacity /= 1024.0;
        }
        QString str_capacity=QString(" %1%2").arg(capacity).arg(unit);
        return str_capacity;
     //   return QString().setNum(capacity,'f',2)+" "+unit;
}

bool QClickWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_eject_button)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            m_eject_button->setIconSize(QSize(14,14));
            m_eject_button->setFixedSize(38,38);
            m_eject_button->setStyleSheet(
                    "background:rgba(255,255,255,0.08);"
                    "border-radius:4px;"
                    );
        }

        if(event->type() == QEvent::Enter)
        {
            m_eject_button->setIconSize(QSize(16,16));
            m_eject_button->setFixedSize(40,40);
            m_eject_button->setStyleSheet(
                    "background-color:rgba(255,255,255,0.91);"
                    "background:rgba(255,255,255,0.12);"
                    "border-radius:4px;");
        }

        if(event->type() == QEvent::Leave)
        {
            m_eject_button->setIconSize(QSize(16,16));
            m_eject_button->setFixedSize(40,40);
            m_eject_button->setStyleSheet(
                    "background-color:rgba(255,255,255,0.75);"
                    "background-color:rgba(255,255,255,0.57);"
                    "background:rgba(255,255,255,0);"
                    "border-radius:4px;");

        }
    }

    if(obj == disWidgetNumOne)
    {
       if(event->type() == QEvent::Enter)
       {
           disWidgetNumOne->setStyleSheet(
                       "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
           if(m_nameDis1_label)
           {
           m_nameDis1_label->setStyleSheet(
           //正常状态样式
           "QLabel{"
           "font-size:14px;"
           "font-family:Microsoft YaHei;"
           "font-weight:400;"
           "color:rgba(255,255,255,1);"
           "line-height:28px;"
           "}");
           }
           m_capacityDis1_label->setStyleSheet(
           //正常状态样式
           "QLabel{"
           "font-size:14px;"
           "font-family:Microsoft YaHei;"
           "font-weight:400;"
           "color:rgba(255,255,255,1);"
           "line-height:28px;"
           "}");
       }

       if(event->type() == QEvent::Leave)
       {
           disWidgetNumOne->setStyleSheet(
                       "");
           m_nameDis1_label->setStyleSheet(
           //正常状态样式
           "QLabel{"
           "font-size:14px;"
           "font-family:Microsoft YaHei;"
           "font-weight:400;"
           "color:rgba(255,255,255,0.35);"
           "line-height:28px;"
           "opacity:0.35;"
           "}");
           m_capacityDis1_label->setStyleSheet(
           //正常状态样式
           "QLabel{"
           "font-size:14px;"
           "font-family:Microsoft YaHei;"
           "font-weight:400;"
           "color:rgba(255,255,255,0.35);"
           "line-height:28px;"
           "opacity:0.35;"
           "}");
       }

       if(event->type() == QEvent::MouseButtonPress)
       {
           on_volume1_clicked();
       }
    }

    if(obj == disWidgetNumTwo)
    {
        if(event->type() == QEvent::Enter )
        {
            disWidgetNumTwo->setStyleSheet(
                        "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            m_nameDis2_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");

            m_capacityDis2_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumTwo->setStyleSheet(
                        "");
            m_nameDis2_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");

            m_capacityDis2_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume2_clicked();
        }
    }

    if(obj == disWidgetNumThree)
    {
        if(event->type() == QEvent::Enter )
        {
            disWidgetNumThree->setStyleSheet(
                        "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            m_nameDis3_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");

            m_capacityDis3_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumThree->setStyleSheet(
                        "");
            m_nameDis3_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");

            m_capacityDis3_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume3_clicked();
        }
    }

    if(obj == disWidgetNumFour)
    {
        if(event->type() == QEvent::Enter )
        {
            disWidgetNumFour->setStyleSheet(
                        "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            m_nameDis4_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");

            m_capacityDis4_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}");
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumFour->setStyleSheet(
                        "");
            m_nameDis4_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");

            m_capacityDis4_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,0.35);"
            "line-height:28px;"
            "opacity:0.35;"
            "}");
        }

        if(event->type() == QEvent::MouseButtonPress)
        {
            on_volume4_clicked();
        }
    }

    return false;
}

void QClickWidget::resizeEvent(QResizeEvent *event)
{
}
