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

#include "frmlunarcalendarwidget.h"
#include "ui_frmlunarcalendarwidget.h"
#include <QPainter>
#include <QDBusInterface>
#include <QDBusReply>
#include <KWindowSystem>

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"
#define PANEL_CONTROL_IN_CALENDAR "org.ukui.control-center.panel.plugins"
#define LUNAR_KEY "calendar"
#define FIRST_DAY_KEY "firstday"

frmLunarCalendarWidget::frmLunarCalendarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmLunarCalendarWidget),
    mCalendarDBus(new CalendarDBus(this))
{
    installEventFilter(this);
    ui->setupUi(this);
    this->hide();
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeUp,this,&frmLunarCalendarWidget::changeUpSize);
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeDown,this,&frmLunarCalendarWidget::changeDownSize);
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeUp,this,&frmLunarCalendarWidget::set_window_position);
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeDown,this,&frmLunarCalendarWidget::set_window_position);
    connect(mCalendarDBus,&CalendarDBus::ShowCalendarWidget,this,[=](){
        KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager);
        if (this->isHidden()){
            this->show();
            this->activateWindow();
        }else {
            this->hide();
        }
    });
    this->initForm();
//    this->setWindowFlags(Qt::X11BypassWindowManagerHint);
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager);
//    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setProperty("useSystemStyleBlur", true);

    this->setFixedSize(440, 600);
    set_window_position();

    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
        transparency_gsettings = new QGSettings(transparency_id);
    }
    const QByteArray calendar_id(PANEL_CONTROL_IN_CALENDAR);
    if(QGSettings::isSchemaInstalled(calendar_id)){
        calendar_gsettings = new QGSettings(calendar_id);
        //公历/农历切换
        connect(calendar_gsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key == LUNAR_KEY){
                ckShowLunar_stateChanged(calendar_gsettings->get(LUNAR_KEY).toString() == "lunar");
            }
            if (key == FIRST_DAY_KEY) {
                cboxWeekNameFormat_currentIndexChanged(calendar_gsettings->get(FIRST_DAY_KEY).toString() == "sunday");
            }
        });
    } else {
        ckShowLunar_stateChanged(false);
        cboxWeekNameFormat_currentIndexChanged(false);
    }
}

frmLunarCalendarWidget::~frmLunarCalendarWidget()
{
    delete ui;
}

void frmLunarCalendarWidget::changeUpSize()
{
    this->setFixedSize(440, 652);
    Q_EMIT yijiChangeUp();
}

void frmLunarCalendarWidget::changeDownSize()
{
    this->setFixedSize(440, 600);
    Q_EMIT yijiChangeDown();
}

void frmLunarCalendarWidget::initForm()
{
    //ui->cboxWeekNameFormat->setCurrentIndex(0);
}

void frmLunarCalendarWidget::cboxCalendarStyle_currentIndexChanged(int index)
{
    ui->lunarCalendarWidget->setCalendarStyle((LunarCalendarWidget::CalendarStyle)index);
}

void frmLunarCalendarWidget::cboxSelectType_currentIndexChanged(int index)
{
    ui->lunarCalendarWidget->setSelectType((LunarCalendarWidget::SelectType)index);
}

void frmLunarCalendarWidget::cboxWeekNameFormat_currentIndexChanged(bool FirstDayisSun)
{
    ui->lunarCalendarWidget->setWeekNameFormat(FirstDayisSun);
}

void frmLunarCalendarWidget::ckShowLunar_stateChanged(bool arg1)
{
    ui->lunarCalendarWidget->setShowLunar(arg1);
}

void frmLunarCalendarWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QRect rect = this->rect();
    QPainter p(this);
    double tran =1;
    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
       tran=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    }

    QColor color = palette().color(QPalette::Base);
    color.setAlpha(tran);
    QBrush brush =QBrush(color);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*
 * 事件过滤，检测鼠标点击外部活动区域则收回收纳栏
*/
bool frmLunarCalendarWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Leave)
        {
            qDebug()<<"event->type() == QEvent::ActivationChange";
            if(QApplication::activeWindow() != this)
            {
                qDebug()<<"this->hide()";
                this->hide();
            }
        }
        return QWidget::event(event);
    if (obj == this)
    {
        qDebug()<<"obj == this";
        if (event->type() == QEvent::MouseButtonPress)
           {
            qDebug()<<"QEvent::MouseButtonPress";
               QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
               if (mouseEvent->button() == Qt::LeftButton)
               {
                   qDebug()<<"激活内部窗口";
                   this->hide();
                   return true;
               }
               else if(mouseEvent->button() == Qt::RightButton)
               {
                   return true;
               }
           }
        else if(event->type() == QEvent::ContextMenu)
        {
            return false;
        }
        else if (event->type() == QEvent::WindowDeactivate)
        {
            qDebug()<<"激活外部窗口";
            this->hide();
            return true;
        } else if (event->type() == QEvent::StyleChange) {
        }
    }

    if (!isActiveWindow())
    {
        activateWindow();
    }

//    return false;
}

void frmLunarCalendarWidget::set_window_position(){
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel", QDBusConnection::sessionBus());
    QDBusReply < QVariantList > reply =iface.call("GetPrimaryScreenGeometry");
//    qDebug() << reply.value().at(2).toInt();
//    qDebug() << reply.value().at(3).toInt();
//    qDebug() <<this->width();
//    qDebug() <<this->height();

    switch (reply.value().at(4).toInt()) {
    case 1:
        this->setGeometry(reply.value().at(0).toInt() +
                          reply.value().at(2).toInt() - this->width() -
                          4, reply.value().at(1).toInt() + 4,
                          this->width(), this->height());
        break;
    case 2:
        this->setGeometry(reply.value().at(0).toInt() + 4,
                          reply.value().at(3).toInt()- this->height() - 4,
                          this->width(), this->height());
        break;
    case 3:
        this->setGeometry(reply.value().at(2).toInt() - this->width() - 4,
                          reply.value().at(3).toInt()-this->height()- 4,
                          this->width(), this->height());
        break;
    default:
        this->setGeometry(reply.value().at(0).toInt() +
                          reply.value().at(2).toInt() - this->width() -
                          4,
                          reply.value().at(1).toInt() +
                          reply.value().at(3).toInt() - this->height() -
                          4, this->width(), this->height());

        break;
    }

}

void frmLunarCalendarWidget::mousePressEvent(QMouseEvent *event){
    if (Qt::LeftButton == event->button() ){
        qDebug()<<"leftbutton pressed!!!";
        if(QApplication::activeWindow() != this)
        {
            qDebug()<<"this->hide()";
            qDebug()<<this;
            qDebug()<<QApplication::activeWindow();
            this->hide();
        }

        if(this->isHidden()){
            this->show();
        }else {
            this->hide();
        }
    }


}
