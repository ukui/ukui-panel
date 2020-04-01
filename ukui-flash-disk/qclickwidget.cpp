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
        image_show_label = new QLabel();
        QPixmap pixmap(":picture/drive-removable-media-usb.png");
        image_show_label->setPixmap(pixmap);
        m_driveName_label = new QLabel();
        m_driveName_label->setText(m_driveName);
        m_driveName_label->setObjectName("driveNameLabel");
        m_eject_button = new QPushButton();
        QIcon eject_icon(":picture/media-eject-symbolic.svg");
        m_eject_button->setIcon(eject_icon);
        m_eject_button->setFlat(true);
        m_eject_button->setObjectName("ejectButton");
        drivename_H_BoxLayout->addWidget(image_show_label);
        drivename_H_BoxLayout->addWidget(m_driveName_label);
        drivename_H_BoxLayout->addWidget(m_eject_button);

        connect(m_eject_button,SIGNAL(clicked()),SLOT(switchWidgetClicked()));
//when the drive only has one vlolume
        if(m_Num == 1)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel("- "+m_nameDis1+":",NULL);
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
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText();
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);

            QVBoxLayout *main_V_BoxLayout = new QVBoxLayout();
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addLayout(onevolume_h_BoxLayout);
            this->setLayout(main_V_BoxLayout);
            this->setFixedHeight(75);
            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
        }
//when the drive has two volumes
        if(m_Num == 2)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel("- "+m_nameDis1+":",NULL);
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
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);


            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel("- "+m_nameDis2+":",NULL);
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
            m_nameDis2_label->installEventFilter(this);
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
            m_capacityDis2_label->setObjectName("capacityLabel");
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);

            QVBoxLayout *main_V_BoxLayout = new QVBoxLayout();
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addLayout(onevolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(twovolume_h_BoxLayout);
            this->setLayout(main_V_BoxLayout);
            this->setFixedHeight(110);
            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
        }
//when the drive has three volumes
        if(m_Num == 3)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel("- "+m_nameDis1+":",NULL);
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
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);

            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel("- "+m_nameDis2+":",NULL);
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
            m_nameDis2_label->installEventFilter(this);
            //m_nameDis2_label->setText("- "+m_nameDis2+":");
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
            m_capacityDis2_label->setObjectName("capacityLabel");
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);

            QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis3_label = new ClickLabel("- "+m_nameDis3+":",NULL);
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
            threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
            threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);

            QVBoxLayout *main_V_BoxLayout = new QVBoxLayout();
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addLayout(onevolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(twovolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(threevolume_h_BoxLayout);
            this->setLayout(main_V_BoxLayout);
            this->setFixedHeight(145);
            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
        }
//when the drive has four volumes
        if(m_Num == 4)
        {
            QHBoxLayout *onevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis1_label = new ClickLabel("- "+m_nameDis1+":",NULL);
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
            m_nameDis1_label->installEventFilter(this);
            //m_nameDis1_label->setText("- "+m_nameDis1+":");
            m_capacityDis1_label = new QLabel();
            QString str_capacityDis1 = size_human(m_capacityDis1);
            m_capacityDis1_label->setText("("+str_capacityDis1+")");
            m_capacityDis1_label->setObjectName("capacityLabel");
            onevolume_h_BoxLayout->addWidget(m_nameDis1_label);
            onevolume_h_BoxLayout->addWidget(m_capacityDis1_label);

            QHBoxLayout *twovolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis2_label = new ClickLabel("- "+m_nameDis2+":",NULL);
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
            m_nameDis2_label->installEventFilter(this);
            //m_nameDis2_label->setText("- "+m_nameDis2+":");
            m_capacityDis2_label = new QLabel();
            QString str_capacityDis2 = size_human(m_capacityDis2);
            m_capacityDis2_label->setText("("+str_capacityDis2+")");
            m_capacityDis2_label->setObjectName("capacityLabel");
            twovolume_h_BoxLayout->addWidget(m_nameDis2_label);
            twovolume_h_BoxLayout->addWidget(m_capacityDis2_label);

            QHBoxLayout *threevolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis3_label = new ClickLabel("- "+m_nameDis3+":",NULL);
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
            threevolume_h_BoxLayout->addWidget(m_nameDis3_label);
            threevolume_h_BoxLayout->addWidget(m_capacityDis3_label);

            QHBoxLayout *fourvolume_h_BoxLayout = new QHBoxLayout();
            m_nameDis4_label = new ClickLabel("- "+m_nameDis4+":",NULL);
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
            m_nameDis4_label->installEventFilter(this);
            //m_nameDis4_label->setText("- "+m_nameDis4+":");
            m_capacityDis4_label = new QLabel();
            QString str_capacityDis4 = size_human(m_capacityDis4);
            m_capacityDis4_label->setText("("+str_capacityDis4+")");
            m_capacityDis4_label->setObjectName("capacityLabel");
            fourvolume_h_BoxLayout->addWidget(m_nameDis4_label);
            fourvolume_h_BoxLayout->addWidget(m_capacityDis4_label);

            QVBoxLayout *main_V_BoxLayout = new QVBoxLayout();
            main_V_BoxLayout->addLayout(drivename_H_BoxLayout);
            main_V_BoxLayout->addLayout(onevolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(twovolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(threevolume_h_BoxLayout);
            main_V_BoxLayout->addLayout(fourvolume_h_BoxLayout);
            this->setLayout(main_V_BoxLayout);
            this->setFixedHeight(180);
            connect(m_nameDis1_label,SIGNAL(clicked()),this,SLOT(on_volume1_clicked()));
            connect(m_nameDis2_label,SIGNAL(clicked()),this,SLOT(on_volume2_clicked()));
            connect(m_nameDis3_label,SIGNAL(clicked()),this,SLOT(on_volume3_clicked()));
            connect(m_nameDis4_label,SIGNAL(clicked()),this,SLOT(on_volume4_clicked()));
        }
        connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));

    }




QClickWidget::~QClickWidget()
{

}



void QClickWidget::mouseClicked()
{
    //处理代码
//        std::string str = m_path.toStdString();
//        const char* ch = str.c_str();
    qDebug() << "打开" << m_pathDis1;
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

void QClickWidget::paintEvent(QPaintEvent *)
 {
     //解决QClickWidget类设置样式不生效的问题
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     p.setBrush(QColor(0x00,0x00,0x00));
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

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
    if(obj == m_nameDis1_label)
    {
        if(event->type() == QEvent::Enter)
        {
            m_nameDis1_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "width:111px;"
            "height:14px;"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}"
            );

        }
        else if(event->type() == QEvent::Leave)
        {
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
            "}");
        }
    }
    if(obj == m_nameDis2_label)
    {
        if(event->type() == QEvent::Enter)
        {
            m_nameDis2_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "width:111px;"
            "height:14px;"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}"
            );

        }
        else if(event->type() == QEvent::Leave)
        {
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
            "}");
        }
    }

    if(obj == m_nameDis3_label)
    {
        if(event->type() == QEvent::Enter)
        {
            m_nameDis3_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "width:111px;"
            "height:14px;"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}"
            );

        }
        else if(event->type() == QEvent::Leave)
        {
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
            "}");
        }
    }

    if(obj == m_nameDis4_label)
    {
        if(event->type() == QEvent::Enter)
        {
            m_nameDis4_label->setStyleSheet(
            //正常状态样式
            "QLabel{"
            "width:111px;"
            "height:14px;"
            "font-size:14px;"
            "font-family:Microsoft YaHei;"
            "font-weight:400;"
            "color:rgba(255,255,255,1);"
            "line-height:28px;"
            "}"
            );

        }
        else if(event->type() == QEvent::Leave)
        {
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
            "}");
        }
    }
    return false;
}
