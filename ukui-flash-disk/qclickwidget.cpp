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
#include <KWindowEffects>
#include <QtConcurrent/QtConcurrent>

void frobnitz_force_result_func(GDrive *source_object,GAsyncResult *res,QClickWidget *p_this)
{
    auto env = qgetenv("QT_QPA_PLATFORMTHEME");
    qDebug()<<"env"<<env;
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);
    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }
    else
    {
        int volumeNum = g_list_length(g_drive_get_volumes(source_object));
        for(int eachVolume = 0 ; eachVolume < volumeNum ;eachVolume++)
        {
            p_this->flagType = 0;

            if(g_mount_can_unmount(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))))
            {
                QtConcurrent::run([=](){
                char *dataPath = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))));
                qDebug()<<"dataPath"<<dataPath;
                QProcess p;
                p.setProgram("pkexec");
                p.setArguments(QStringList()<<"eject"<<QString(dataPath));
                p.start();
                p_this->ifSucess = p.waitForFinished();
                });
            }
        }
    }
}

void frobnitz_result_func(GDrive *source_object,GAsyncResult *res,QClickWidget *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);

    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }

    else /*if(g_drive_can_stop(source_object) == true)*/
    {
        if (p_this->chooseDialog == nullptr)
        {
            p_this->chooseDialog = new interactiveDialog(p_this);
        }
        qDebug()<<__FUNCTION__;
//        p_this->chooseDialog->raise();
        p_this->chooseDialog->show();
        p_this->chooseDialog->setFocus();
        p_this->connect(p_this->chooseDialog,&interactiveDialog::FORCESIG,p_this,[=]()
        {
            g_drive_eject_with_operation(source_object,
                                         G_MOUNT_UNMOUNT_FORCE,
                                         NULL,
                                         NULL,
                                         GAsyncReadyCallback(frobnitz_force_result_func),
                                         p_this
                                         );
            p_this->chooseDialog->close();
        });
    }
}

void frobnitz_force_result_tele(GVolume *source_object,GAsyncResult *res,QClickWidget *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_mount_unmount_with_operation_finish(g_volume_get_mount(source_object),res, &err);
    if(!err)
    {
        qDebug()<<"unload successful";
        findTeleGVolumeList()->removeOne(source_object);
        findGVolumeList()->removeOne(source_object);
        if(findTeleGVolumeList()->size() == 1)
        {
            if(findGVolumeList()->size() == 1)
            {
                Q_EMIT p_this->noDeviceSig();
            }
        }
    }
}

QClickWidget::QClickWidget(QWidget *parent,
                           int num,
                           GDrive *Drive,
                           GVolume *Volume,
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
      m_Drive(Drive),
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
/*
 * it's the to set the title interface,we get the drive name and add picture of a u disk
*/
    const QByteArray id(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(id))
    {
        fontSettings = new QGSettings(id);
    }

    const QByteArray idd(THEME_QT_SCHEMA);

    if(QGSettings::isSchemaInstalled(idd))
    {
        qtSettings = new QGSettings(idd);
    }

    initFontSize();
    initThemeMode();

    QHBoxLayout *drivename_H_BoxLayout = new QHBoxLayout();
    drivename_H_BoxLayout = new QHBoxLayout();
    image_show_label = new QLabel(this);
    image_show_label->setFocusPolicy(Qt::NoFocus);
    image_show_label->installEventFilter(this);
    //to get theme picture for label
    imgIcon = QIcon::fromTheme("drive-removable-media-usb");
    QPixmap pixmap = imgIcon.pixmap(QSize(25, 25));
    image_show_label->setPixmap(pixmap);
    image_show_label->setFixedSize(40,40);
    m_driveName_label = new QLabel(this);
    m_driveName_label->setFont(QFont("Noto Sans CJK SC",fontSize));
    QString DriveName = getElidedText(m_driveName_label->font(), m_driveName, 180);
    m_driveName_label->setText(DriveName);
    m_driveName_label->setFixedSize(180,40);
    m_driveName_label->setObjectName("driveNameLabel");

    m_eject_button = new QPushButton(this);
    m_eject_button->setFlat(true);   //this property set that when the mouse is hovering in the icon the icon will move up a litte
    m_eject_button->move(m_eject_button->x()+234,m_eject_button->y()+2);
    //->setObjectName("Button");
    m_eject_button->installEventFilter(this);
    m_eject_button->setIcon(drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("media-eject-symbolic").pixmap(24,24).toImage())));
    m_eject_button->setFixedSize(40,40);
    m_eject_button->setToolTip(tr("弹出"));

    drivename_H_BoxLayout->addSpacing(8);
    drivename_H_BoxLayout->addWidget(image_show_label);
    drivename_H_BoxLayout->addWidget(m_driveName_label);
    drivename_H_BoxLayout->addStretch();

    QVBoxLayout *main_V_BoxLayout = new QVBoxLayout(this);
    main_V_BoxLayout->setContentsMargins(0,0,0,0);

    connect(m_eject_button,SIGNAL(clicked()),SLOT(switchWidgetClicked()));  // this signal-slot function is to emit a signal which
                                                                            //is to trigger a slot in mainwindow
    connect(m_eject_button, &QPushButton::clicked,this,[=]()
    {
        if(Drive != NULL)
        {
            g_drive_eject_with_operation(Drive,
                         G_MOUNT_UNMOUNT_NONE,
                         NULL,
                         NULL,
                         GAsyncReadyCallback(frobnitz_result_func),
                         this);
        }
        else
        {
            g_mount_unmount_with_operation(g_volume_get_mount(Volume),
                        G_MOUNT_UNMOUNT_NONE,
                        NULL,
                        NULL,
                        GAsyncReadyCallback(frobnitz_force_result_tele),
                        this);
        }
    });

//when the drive only has one vlolume
//we set the information and set all details of the U disk in main interface
    if(m_Num == 1)
    {
        disWidgetNumOne = new QWidget(this);
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(disWidgetNumOne);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeName = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->setText("- "+VolumeName+":");
        m_capacityDis1_label = new QLabel(disWidgetNumOne);

        QString str_capacityDis1 = size_human(m_capacityDis1);
//        QString str_capacityDis1Show = getElidedText(m_capacityDis1_label->font(),str_capacityDis1,200);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setMargin(0);   //使得widget上的label得以居中显示
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addSpacing(10);
        onevolume_h_BoxLayout->addStretch();

        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);
        disWidgetNumOne->setFixedHeight(30);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if (m_pathDis1.isEmpty()) {
            m_capacityDis1_label->setText(tr("Unmounted"));
        }
        main_V_BoxLayout->addWidget(disWidgetNumOne);
        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,68);
    }
//when the drive has two volumes
    if(m_Num == 2)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel();
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
        m_nameDis1_label->installEventFilter(this);
        m_capacityDis1_label = new QLabel(this);
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->setMargin(0);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        m_capacityDis2_label = new QLabel(this);
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->setMargin(0);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        main_V_BoxLayout->addStretch();
        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,97);
    }
//when the drive has three volumes
    if(m_Num == 3)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(this);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis2, 120);
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->installEventFilter(this);
        //m_nameDis1_label->setText("- "+m_nameDis1+":");
        m_capacityDis1_label = new QLabel(this);
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        onevolume_h_BoxLayout->setMargin(0);

        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        m_capacityDis2_label = new QLabel(this);
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        twovolume_h_BoxLayout->setMargin(0);

        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis3_label = new ClickLabel(this);
        m_nameDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
        m_nameDis3_label->setText("- "+VolumeNameDis3+":");
        m_nameDis3_label->adjustSize();
        m_nameDis3_label->installEventFilter(this);
        m_capacityDis3_label = new QLabel(this);
        QString str_capacityDis3 = size_human(m_capacityDis3);
        m_capacityDis3_label->setText("("+str_capacityDis3+")");
        m_capacityDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis3_label->setObjectName("capacityLabel");
        threevolume_h_BoxLayout->addSpacing(50);
        threevolume_h_BoxLayout->setSpacing(0);
        threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
        threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
        threevolume_h_BoxLayout->addStretch();
        threevolume_h_BoxLayout->setMargin(0);

        disWidgetNumThree = new QWidget;
        disWidgetNumThree->setFixedHeight(30);
        disWidgetNumThree->setObjectName("OriginObjectOnly");
        disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
        disWidgetNumThree->installEventFilter(this);


        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        if(m_pathDis3 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumThree);
        }

        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,136);
    }
//when the drive has four volumes
    if(m_Num == 4)
    {
        QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis1_label = new ClickLabel(this);
        m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis1 = getElidedText(m_nameDis1_label->font(), m_nameDis1, 120);
        m_nameDis1_label->setText("- "+VolumeNameDis1+":");
        m_nameDis1_label->adjustSize();
        m_nameDis1_label->installEventFilter(this);
        //m_nameDis1_label->setText("- "+m_nameDis1+":");
        m_capacityDis1_label = new QLabel(this);
        QString str_capacityDis1 = size_human(m_capacityDis1);
        m_capacityDis1_label->setText("("+str_capacityDis1+")");
        m_capacityDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis1_label->setObjectName("capacityLabel");
        onevolume_h_BoxLayout->addSpacing(50);
        onevolume_h_BoxLayout->setSpacing(0);
        onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
        onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);
        onevolume_h_BoxLayout->addStretch();
        onevolume_h_BoxLayout->setMargin(0);

        disWidgetNumOne = new QWidget;
        disWidgetNumOne->setFixedHeight(30);
        disWidgetNumOne->setObjectName("OriginObjectOnly");
        disWidgetNumOne->setLayout(onevolume_h_BoxLayout);
        disWidgetNumOne->installEventFilter(this);

        QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis2_label = new ClickLabel(this);
        m_nameDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis2 = getElidedText(m_nameDis2_label->font(), m_nameDis2, 120);
        m_nameDis2_label->setText("- "+VolumeNameDis2+":");
        m_nameDis2_label->installEventFilter(this);
        m_nameDis2_label->adjustSize();
        m_nameDis2_label->installEventFilter(this);
        //m_nameDis2_label->setText("- "+m_nameDis2+":");
        m_capacityDis2_label = new QLabel(this);
        QString str_capacityDis2 = size_human(m_capacityDis2);
        m_capacityDis2_label->setText("("+str_capacityDis2+")");
        m_capacityDis2_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis2_label->setObjectName("capacityLabel");
        twovolume_h_BoxLayout->addSpacing(50);
        twovolume_h_BoxLayout->setSpacing(0);
        twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
        twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);
        twovolume_h_BoxLayout->addStretch();
        twovolume_h_BoxLayout->setMargin(0);

        disWidgetNumTwo = new QWidget;
        disWidgetNumTwo->setFixedHeight(30);
        disWidgetNumTwo->setObjectName("OriginObjectOnly");
        disWidgetNumTwo->setLayout(twovolume_h_BoxLayout);
        disWidgetNumTwo->installEventFilter(this);

        QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis3_label = new ClickLabel(this);
        m_nameDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis3 = getElidedText(m_nameDis3_label->font(), m_nameDis3, 120);
        m_nameDis3_label->setText("- "+VolumeNameDis3+":");
        m_nameDis3_label->installEventFilter(this);
        m_capacityDis3_label = new QLabel(this);
        QString str_capacityDis3 = size_human(m_capacityDis3);
        m_capacityDis3_label->setText("("+str_capacityDis3+")");
        m_capacityDis3_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis3_label->setObjectName("capacityLabel");
        threevolume_h_BoxLayout->addSpacing(50);
        threevolume_h_BoxLayout->setSpacing(0);
        threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
        threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);
        threevolume_h_BoxLayout->addStretch(0);
        threevolume_h_BoxLayout->setMargin(0);

        disWidgetNumThree = new QWidget;
        disWidgetNumThree->setFixedHeight(30);
        disWidgetNumThree->setObjectName("OriginObjectOnly");
        disWidgetNumThree->setLayout(threevolume_h_BoxLayout);
        disWidgetNumThree->installEventFilter(this);

        QHBoxLayout *fourvolume_h_BoxLayout = new QHBoxLayout();
        m_nameDis4_label = new ClickLabel(this);
        m_nameDis4_label->setFont(QFont("Microsoft YaHei",fontSize));
        QString VolumeNameDis4 = getElidedText(m_nameDis4_label->font(), m_nameDis4, 120);
        m_nameDis4_label->setText("- "+VolumeNameDis4+":");
        m_nameDis4_label->adjustSize();
        m_nameDis4_label->installEventFilter(this);
        //m_nameDis4_label->setText("- "+m_nameDis4+":");
        m_capacityDis4_label = new QLabel(this);
        QString str_capacityDis4 = size_human(m_capacityDis4);
        m_capacityDis4_label->setText("("+str_capacityDis4+")");
        m_capacityDis4_label->setFont(QFont("Microsoft YaHei",fontSize));
        m_capacityDis4_label->setObjectName("capacityLabel");
        fourvolume_h_BoxLayout->addSpacing(50);
        fourvolume_h_BoxLayout->setSpacing(0);
        fourvolume_h_BoxLayout->addWidget(m_nameDis4_label);
        fourvolume_h_BoxLayout->addWidget(m_capacityDis4_label);
        fourvolume_h_BoxLayout->addStretch();
        fourvolume_h_BoxLayout->setMargin(0);

        disWidgetNumFour = new QWidget;
        disWidgetNumFour->setFixedHeight(30);
        disWidgetNumFour->setObjectName("OriginObjectOnly");
        disWidgetNumFour->setLayout(fourvolume_h_BoxLayout);
        disWidgetNumFour->installEventFilter(this);

        main_V_BoxLayout->setContentsMargins(0,0,0,0);
        main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
        if(m_pathDis1 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumOne);
        }

        if(m_pathDis2 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumTwo);
        }

        if(m_pathDis3 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumThree);
        }

        if(m_pathDis4 != "")
        {
            main_V_BoxLayout->addWidget(disWidgetNumFour);
        }

        this->setLayout(main_V_BoxLayout);
        this->setFixedSize(276,165);
    }
    qDebug()<<"4444";
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    qDebug()<<"qlcked overeend";
}

void QClickWidget::initFontSize()
{
    if (!fontSettings)
    {
       fontSize = 11;
       return;
    }

    QStringList keys = fontSettings->keys();
    if (keys.contains("systemFont") || keys.contains("systemFontSize"))
    {
        fontSize = fontSettings->get("system-font").toInt();
    }
}

void QClickWidget::initThemeMode()
{
    if(!qtSettings)
    {
        currentThemeMode = "ukui-white";
    }
    QStringList keys = qtSettings->keys();
    if(keys.contains("styleName"))
    {
        currentThemeMode = qtSettings->get("style-name").toString();
    }
}

QClickWidget::~QClickWidget()
{
    if(chooseDialog)
        delete chooseDialog;
    if(gpartedface)
        delete gpartedface;
}

void QClickWidget::mouseClicked()
{
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

//click the first area to show the interface
void QClickWidget::on_volume1_clicked()
{
    if (!m_pathDis1.isEmpty()) {
        QString aaa = "peony "+m_pathDis1;
        QProcess::startDetached(aaa.toUtf8().data());
        this->topLevelWidget()->hide();
    }
}

//click the second area to show the interface
void QClickWidget::on_volume2_clicked()
{
    if (!m_pathDis2.isEmpty()) {
        QString aaa = "peony "+m_pathDis2;
        QProcess::startDetached(aaa.toUtf8().data());
        this->topLevelWidget()->hide();
    }
}

//click the third area to show the interface
void QClickWidget::on_volume3_clicked()
{
    if (!m_pathDis3.isEmpty()) {
        QProcess::startDetached("peony "+m_pathDis3);
        this->topLevelWidget()->hide();
    }
}

//click the forth area to show the interface
void QClickWidget::on_volume4_clicked()
{
    if (!m_pathDis4.isEmpty()) {
        QProcess::startDetached("peony "+m_pathDis4);
        this->topLevelWidget()->hide();
    }
}

void QClickWidget::switchWidgetClicked()
{
    Q_EMIT clickedConvert();
}

QPixmap QClickWidget::drawSymbolicColoredPixmap(const QPixmap &source)
{
    if(currentThemeMode == "ukui-light" || currentThemeMode == "ukui-white")
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(0);
                        color.setGreen(0);
                        color.setBlue(0);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }

    else if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default" )
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(255);
                        color.setGreen(255);
                        color.setBlue(255);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }

    else
    {
        QImage img = source.toImage();
        for (int x = 0; x < img.width(); x++)
        {
            for (int y = 0; y < img.height(); y++)
            {
                auto color = img.pixelColor(x, y);
                if (color.alpha() > 0)
                {
                        color.setRed(0);
                        color.setGreen(0);
                        color.setBlue(0);
                        img.setPixelColor(x, y, color);
                }
            }
        }
        return QPixmap::fromImage(img);
    }
}

//to convert the capacity by another type
QString QClickWidget::size_human(qlonglong capacity)
{
    //    float capacity = this->size();
    if(capacity != 0 && capacity != 1)
    {
        int conversionNum = 0;
        QStringList list;
        list << "KB" << "MB" << "GB" << "TB";

        QStringListIterator i(list);
        QString unit("bytes");

        qlonglong conversion = capacity;

        while(conversion >= 1000.0 && i.hasNext())
        {
            unit = i.next();
            conversion /= 1000.0;
            conversionNum++;
        }

        qlonglong remain  = capacity - conversion * qPow(1000,conversionNum);
        float showRemain;
        if(conversionNum == 3)
        {
            showRemain = (float)remain /1000/1000/1000;
        }
        if(conversionNum == 2)
        {
            showRemain = (float)remain /1000/1000;
        }
        if(conversionNum == 1)
        {
            showRemain = (float)remain /1000;
        }

        double showValue = conversion + showRemain;

        QString str2=QString::number(showValue,'f',1);

        QString str_capacity=QString(" %1%2").arg(str2).arg(unit);
        return str_capacity;
     //   return QString().setNum(capacity,'f',2)+" "+unit;
    }
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    if(capacity == NULL)
    {
       QString str_capaticity = tr("the capacity is empty");
       return str_capaticity;
    }
#endif
    if(capacity == 1)
    {
        QString str_capacity = tr("blank CD");
        return str_capacity;
    }
     QString str_capacity = tr("other user device");
     return str_capacity;
}

//set the style of the eject button and label when the mouse doing some different operations
bool QClickWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_eject_button)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                m_eject_button->setIconSize(QSize(14,14));
                m_eject_button->setFixedSize(38,38);
                m_eject_button->setStyleSheet(
                        "background:rgba(255,255,255,0.08);"
                        "border-radius:4px;"
                        );
            }
            else
            {
                m_eject_button->setIconSize(QSize(14,14));
                m_eject_button->setFixedSize(38,38);
                m_eject_button->setStyleSheet(
                        "background:rgba(0,0,0,0.08);"
                        "border-radius:4px;"
                        );
            }

        }

        if(event->type() == QEvent::Enter)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(255,255,255,0.91);"
                        "background:rgba(255,255,255,0.12);"
                        "border-radius:4px;");
            }
            else
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(0,0,0,0.91);"
                        "background:rgba(0,0,0,0.12);"
                        "border-radius:4px;");
            }

        }

        if(event->type() == QEvent::Leave)
        {
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black")
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(255,255,255,0.75);"
                        "background-color:rgba(255,255,255,0.57);"
                        "background:rgba(255,255,255,0);"
                        "border-radius:4px;");
            }
            else
            {
                m_eject_button->setIconSize(QSize(16,16));
                m_eject_button->setFixedSize(40,40);
                m_eject_button->setStyleSheet(
                        "background-color:rgba(0,0,0,0.75);"
                        "background-color:rgba(0,0,0,0.57);"
                        "background:rgba(0,0,0,0);"
                        "border-radius:4px;");
            }
        }
    }

    if(obj == disWidgetNumOne)
    {
       if(event->type() == QEvent::Enter)
       {
           if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
           {
               disWidgetNumOne->setStyleSheet(
                           "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
           }
           else
           {
               disWidgetNumOne->setStyleSheet(
                           "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
           }
       }

       if(event->type() == QEvent::Leave)
       {
           disWidgetNumOne->setStyleSheet("");

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
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumTwo->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumTwo->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumTwo->setStyleSheet("");
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
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumThree->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumThree->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumThree->setStyleSheet("");
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
            if(currentThemeMode == "ukui-dark" || currentThemeMode == "ukui-black" || currentThemeMode == "ukui-default")
            {
                disWidgetNumFour->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(255,255,255,0.12);}");
            }
            else
            {
                disWidgetNumFour->setStyleSheet(
                            "QWidget#OriginObjectOnly{background:rgba(0,0,0,0.12);}");
            }
        }

        if(event->type() == QEvent::Leave)
        {
            disWidgetNumFour->setStyleSheet("");
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
