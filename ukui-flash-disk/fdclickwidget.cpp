/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
 * 
 * Authors:
 *  Yang Min yangmin@kylinos.cn
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

#include "fdclickwidget.h"
#include <KWindowEffects>

FDClickWidget::FDClickWidget(QWidget *parent,
                          unsigned diskNo,
                          QString strDriveId,
                          QString strVolumeId,
                          QString strMountId,
                          QString driveName,
                          QString volumeName,
                          quint64 capacityDis,
                          QString strMountUri)
    : QWidget(parent),
      m_uDiskNo(diskNo),
      m_driveName(driveName),
      m_volumeName(volumeName),
      m_capacityDis(capacityDis),
      m_mountUri(strMountUri),
      m_driveId(strDriveId),
      m_volumeId(strVolumeId),
      m_mountId(strMountId)
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
    if (m_uDiskNo <= 1) {
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
        m_eject_button->installEventFilter(this);
        m_eject_button->setIcon(drawSymbolicColoredPixmap(QPixmap::fromImage(QIcon::fromTheme("media-eject-symbolic").pixmap(24,24).toImage())));
        m_eject_button->setFixedSize(40,40);
        m_eject_button->setToolTip(tr("Eject"));
        connect(m_eject_button,SIGNAL(clicked()),SLOT(switchWidgetClicked()));  // this signal-slot function is to emit a signal which
                                                                            //is to trigger a slot in mainwindow
        drivename_H_BoxLayout->addSpacing(8);
        drivename_H_BoxLayout->addWidget(image_show_label);
        drivename_H_BoxLayout->addWidget(m_driveName_label);
        drivename_H_BoxLayout->addStretch();
    }    

    QVBoxLayout *main_V_BoxLayout = new QVBoxLayout(this);
    main_V_BoxLayout->setContentsMargins(0,0,0,0);    

    disWidgetNumOne = new QWidget(this);
    QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
    m_nameDis1_label = new ClickLabel(disWidgetNumOne);
    m_nameDis1_label->setFont(QFont("Microsoft YaHei",fontSize));
    QString VolumeName = getElidedText(m_nameDis1_label->font(), m_volumeName, 120);
    m_nameDis1_label->adjustSize();
    m_nameDis1_label->setText("- "+VolumeName+":");
    m_capacityDis1_label = new QLabel(disWidgetNumOne);

    QString str_capacityDis1 = size_human(m_capacityDis);
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
    if (m_mountUri.isEmpty()) {
        m_capacityDis1_label->setText(tr("Unmounted"));
    }
    main_V_BoxLayout->addWidget(disWidgetNumOne);
    this->setLayout(main_V_BoxLayout);
    if (m_uDiskNo <= 1) {
        this->setFixedSize(276,64);
    } else {
        this->setFixedSize(276,36);
    }
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

void FDClickWidget::initFontSize()
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

void FDClickWidget::initThemeMode()
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

FDClickWidget::~FDClickWidget()
{
    if(chooseDialog)
        delete chooseDialog;
    if(gpartedface)
        delete gpartedface;
}

void FDClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void FDClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) Q_EMIT clicked();
}

//click the first area to show the interface
void FDClickWidget::on_volume_clicked()
{
    if (!m_mountUri.isEmpty()) {
        QString aaa = "peony "+m_mountUri;
        QProcess::startDetached(aaa.toUtf8().data());
        this->topLevelWidget()->hide();
    }
}

void FDClickWidget::switchWidgetClicked()
{
    Q_EMIT clickedEjectItem(this, m_driveId, m_volumeId, m_mountId);
}

QPixmap FDClickWidget::drawSymbolicColoredPixmap(const QPixmap &source)
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
QString FDClickWidget::size_human(qlonglong capacity)
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
bool FDClickWidget::eventFilter(QObject *obj, QEvent *event)
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
           on_volume_clicked();
       }
    }
    return false;
}

void FDClickWidget::resizeEvent(QResizeEvent *event)
{
}
