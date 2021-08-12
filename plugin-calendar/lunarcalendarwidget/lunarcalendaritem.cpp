#pragma execution_character_set("utf-8")

#include "lunarcalendaritem.h"
#include "lunarcalendarwidget.h"
#include "qpainter.h"
#include "qevent.h"
#include "qdatetime.h"
#include "qdebug.h"
#include <QGSettings>

LunarCalendarItem::LunarCalendarItem(QWidget *parent) : QWidget(parent)
{
    hover = false;
    pressed = false;

    select = false;
    showLunar = true;
    bgImage = ":/image/bg_calendar.png";
    selectType = SelectType_Rect;

    date = QDate::currentDate();
    lunar = "初一";
    dayType = DayType_MonthCurrent;

    //实时监听主题变化
    const QByteArray id("org.ukui.style");
    QGSettings * fontSetting = new QGSettings(id, QByteArray(), this);
    connect(fontSetting, &QGSettings::changed,[=](QString key) {
        if(fontSetting->get("style-name").toString() == "ukui-default") {
            weekColor = QColor(255, 255, 255);
            currentTextColor = QColor(255, 255, 255);
            otherTextColor = QColor(255, 255, 255,40);
            otherLunarColor = QColor(255, 255, 255,40);
            currentLunarColor = QColor(255, 255, 255,90);
            lunarColor = QColor(255, 255, 255,90);
        } else if(fontSetting->get("style-name").toString() == "ukui-light") {
            weekColor = QColor(0, 0, 0);
            currentTextColor = QColor(0, 0, 0);
            otherTextColor = QColor(0,0,0,40);
            otherLunarColor = QColor(0,0,0,40);
            currentLunarColor = QColor(0,0,0,90);
            lunarColor = QColor(0,0,0,90);
        } else if(fontSetting->get("style-name").toString() == "ukui-dark") {
            weekColor = QColor(255, 255, 255);
            currentTextColor = QColor(255, 255, 255);
            otherTextColor = QColor(255, 255, 255,40);
            otherLunarColor = QColor(255, 255, 255,40);
            currentLunarColor = QColor(255, 255, 255,90);
            lunarColor = QColor(255, 255, 255,90);
        }
    });

    if(fontSetting->get("style-name").toString() == "ukui-light") {
        weekColor = QColor(0, 0, 0);
        currentTextColor = QColor(0, 0, 0);
        otherTextColor = QColor(0,0,0,40);
        otherLunarColor = QColor(0,0,0,40);
        currentLunarColor = QColor(0,0,0,90);
        lunarColor = QColor(0,0,0,90);
    } else {
        weekColor = QColor(255, 255, 255);
        currentTextColor = QColor(255, 255, 255);
        otherTextColor = QColor(255, 255, 255,40);
        otherLunarColor = QColor(255, 255, 255,40);
        currentLunarColor = QColor(255, 255, 255,90);
        lunarColor = QColor(255, 255, 255,90);
    }

    borderColor = QColor(180, 180, 180);
    superColor = QColor(255, 129, 6);

    selectTextColor = QColor(255, 255, 255);
    hoverTextColor = QColor(250, 250, 250);

    selectLunarColor = QColor(255, 255, 255);
    hoverLunarColor = QColor(250, 250, 250);

    currentBgColor = QColor(255, 255, 255);
    otherBgColor = QColor(240, 240, 240);
    selectBgColor = QColor(55,143,250);
    hoverBgColor = QColor(204, 183, 180);
}

void LunarCalendarItem::enterEvent(QEvent *)
{
    hover = true;
    this->update();
}

void LunarCalendarItem::leaveEvent(QEvent *)
{
    hover = false;
    this->update();
}

void LunarCalendarItem::mousePressEvent(QMouseEvent *)
{
    pressed = true;
    this->update();
    Q_EMIT clicked(date, dayType);
}

void LunarCalendarItem::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
    this->update();
}

QString LunarCalendarItem::handleJsMap(QString year,QString month2day)
{
    QString oneNUmber = "worktime.y" + year;
    QString twoNumber = "d" + month2day;

    QMap<QString,QMap<QString,QString>>::Iterator it = worktime.begin();

    while(it!=worktime.end()) {
         if(it.key() == oneNUmber) {
            QMap<QString,QString>::Iterator it1 = it.value().begin();
            while(it1!=it.value().end()) {
                if(it1.key() == twoNumber) {
                    return it1.value();
                }
                it1++;
            }
         }
         it++;
    }
    return "-1";
}


void LunarCalendarItem::paintEvent(QPaintEvent *)
{

    QDate dateNow = QDate::currentDate();

    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景和边框
    drawBg(&painter);

    //对比当前的时间，画选中状态
    if(dateNow == date) {
         drawBgCurrent(&painter, selectBgColor);
    }

    //绘制悬停状态
    if (hover) {
        drawBgHover(&painter, hoverBgColor);
    }

    //绘制选中状态
    if (select) {
        drawBgHover(&painter, hoverBgColor);
    }

    //绘制日期
    drawDay(&painter);

    //绘制农历信息
    drawLunar(&painter);
}

void LunarCalendarItem::drawBg(QPainter *painter)
{
    painter->save();

    //根据当前类型选择对应的颜色
    QColor bgColor = currentBgColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        bgColor = otherBgColor;
    }

    //painter->setPen(borderColor);
    //painter->setBrush(bgColor);
    //painter->drawRect(rect());

    painter->restore();
}

void LunarCalendarItem::drawBgCurrent(QPainter *painter, const QColor &color)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    QRect rect = this->rect();
    painter->drawRoundedRect(rect,4,4);
//    //根据设定绘制背景样式
//    if (selectType == SelectType_Rect) {

//    }
    painter->restore();
}

void LunarCalendarItem::drawBgHover(QPainter *painter, const QColor &color)
{
    painter->save();
    QRect rect = this->rect();
    painter->setPen(QPen(QColor(55,143,250),2));
    painter->drawRoundedRect(rect,4,4);
//    //根据设定绘制背景样式
//    if (selectType == SelectType_Rect) {

//    }
    painter->restore();
}

void LunarCalendarItem::drawDay(QPainter *painter)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    painter->save();

    //根据当前类型选择对应的颜色
    QColor color = currentTextColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        color = otherTextColor;
    } else if (dayType == DayType_WeekEnd) {
        color = weekColor;
    }

/*    if (select) {
        color = selectTextColor;
    } *//*else if (hover) {
        color = hoverTextColor;
    }*/

    painter->setPen(color);

    QFont font;
    font.setPixelSize(side * 0.3);
    //设置文字粗细
    font.setBold(true);
    painter->setFont(font);

    //代码复用率待优化
    if (showLunar) {
        QRect dayRect = QRect(0, 0, width, height / 1.7);
        painter->drawText(dayRect, Qt::AlignHCenter | Qt::AlignBottom, QString::number(date.day()));
        if (handleJsMap(date.toString("yyyy"),date.toString("MMdd")) == "2") {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(244,78,80));
            QRect dayRect1 = QRect(0, 0, width/3.5,height/3.5);
            painter->drawRoundedRect(dayRect1,1,1);
            font.setPixelSize(side / 5);
            painter->setFont(font);
            painter->setPen(Qt::white);
            painter->drawText(dayRect1, Qt::AlignHCenter | Qt::AlignBottom,"休");
        } else if (handleJsMap(date.toString("yyyy"),date.toString("MMdd")) == "1") {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(251,170,42));
            QRect dayRect1 = QRect(0, 0, width/3.5,height/3.5);
            painter->drawRoundedRect(dayRect1,1,1);
            font.setPixelSize(side / 5);
            painter->setFont(font);
            painter->setPen(Qt::white);
            painter->drawText(dayRect1, Qt::AlignHCenter | Qt::AlignBottom,"班");
        }
    }
    else {
        QRect dayRect = QRect(0, 0, width, height);
        painter->drawText(dayRect, Qt::AlignCenter, QString::number(date.day()));
        if (handleJsMap(date.toString("yyyy"),date.toString("MMdd")) == "2") {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255,0,0));
            QRect dayRect1 = QRect(0, 0, width/3.5,height/3.5);
            painter->drawRoundedRect(dayRect1,1,1);
            font.setPixelSize(side / 5);
            painter->setFont(font);
            painter->setPen(Qt::white);
            painter->drawText(dayRect1, Qt::AlignHCenter | Qt::AlignBottom,"休");
        } else if (handleJsMap(date.toString("yyyy"),date.toString("MMdd")) == "1") {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(251,170,42));
            QRect dayRect1 = QRect(0, 0, width/3.5,height/3.5);
            painter->drawRoundedRect(dayRect1,1,1);
            font.setPixelSize(side / 5);
            painter->setFont(font);
            painter->setPen(Qt::white);
            painter->drawText(dayRect1, Qt::AlignHCenter | Qt::AlignBottom,"班");
        }
    }

    painter->restore();
}

void LunarCalendarItem::drawLunar(QPainter *painter)
{
    if (!showLunar) {
        return;
    }

    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);

    painter->save();

    QStringList listDayName;
    listDayName << "*" << "初一" << "初二" << "初三" << "初四" << "初五" << "初六" << "初七" << "初八" << "初九" << "初十"
                << "十一" << "十二" << "十三" << "十四" << "十五" << "十六" << "十七" << "十八" << "十九" << "二十"
                << "廿一" << "廿二" << "廿三" << "廿四" << "廿五" << "廿六" << "廿七" << "廿八" << "廿九" << "三十";

    //判断当前农历文字是否节日,是节日且是当月则用农历节日颜色显示
    bool exist = (!listDayName.contains(lunar) && dayType != DayType_MonthPre && dayType != DayType_MonthNext);

    //根据当前类型选择对应的颜色
    QColor color = currentLunarColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        color = otherLunarColor;
    }

//    if (select) {
//        color = selectTextColor;
//    } /*else if (hover) {
//        color = hoverTextColor;
//    }*/ else if (exist) {
//        color = lunarColor;
//    }

    if (exist) {
           color = lunarColor;
    }

    painter->setPen(color);

    QFont font;
    font.setPixelSize(side * 0.27);
    painter->setFont(font);

    QRect lunarRect(0, height / 2, width, height / 2);
    painter->drawText(lunarRect, Qt::AlignCenter, lunar);
    painter->restore();
}

bool LunarCalendarItem::getSelect() const
{
    return this->select;
}

bool LunarCalendarItem::getShowLunar() const
{
    return this->showLunar;
}

QString LunarCalendarItem::getBgImage() const
{
    return this->bgImage;
}

LunarCalendarItem::SelectType LunarCalendarItem::getSelectType() const
{
    return this->selectType;
}

QDate LunarCalendarItem::getDate() const
{
    return this->date;
}

QString LunarCalendarItem::getLunar() const
{
    return this->lunar;
}

LunarCalendarItem::DayType LunarCalendarItem::getDayType() const
{
    return this->dayType;
}

QColor LunarCalendarItem::getBorderColor() const
{
    return this->borderColor;
}

QColor LunarCalendarItem::getWeekColor() const
{
    return this->weekColor;
}

QColor LunarCalendarItem::getSuperColor() const
{
    return this->superColor;
}

QColor LunarCalendarItem::getLunarColor() const
{
    return this->lunarColor;
}

QColor LunarCalendarItem::getCurrentTextColor() const
{
    return this->currentTextColor;
}

QColor LunarCalendarItem::getOtherTextColor() const
{
    return this->otherTextColor;
}

QColor LunarCalendarItem::getSelectTextColor() const
{
    return this->selectTextColor;
}

QColor LunarCalendarItem::getHoverTextColor() const
{
    return this->hoverTextColor;
}

QColor LunarCalendarItem::getCurrentLunarColor() const
{
    return this->currentLunarColor;
}

QColor LunarCalendarItem::getOtherLunarColor() const
{
    return this->otherLunarColor;
}

QColor LunarCalendarItem::getSelectLunarColor() const
{
    return this->selectLunarColor;
}

QColor LunarCalendarItem::getHoverLunarColor() const
{
    return this->hoverLunarColor;
}

QColor LunarCalendarItem::getCurrentBgColor() const
{
    return this->currentBgColor;
}

QColor LunarCalendarItem::getOtherBgColor() const
{
    return this->otherBgColor;
}

QColor LunarCalendarItem::getSelectBgColor() const
{
    return this->selectBgColor;
}

QColor LunarCalendarItem::getHoverBgColor() const
{
    return this->hoverBgColor;
}

QSize LunarCalendarItem::sizeHint() const
{
    return QSize(100, 100);
}

QSize LunarCalendarItem::minimumSizeHint() const
{
    return QSize(20, 20);
}

void LunarCalendarItem::setSelect(bool select)
{
    if (this->select != select) {
        this->select = select;
        this->update();
    }
}

void LunarCalendarItem::setShowLunar(bool showLunar)
{
        this->showLunar = showLunar;
        this->update();
}

void LunarCalendarItem::setBgImage(const QString &bgImage)
{
    if (this->bgImage != bgImage) {
        this->bgImage = bgImage;
        this->update();
    }
}

void LunarCalendarItem::setSelectType(const LunarCalendarItem::SelectType &selectType)
{
    if (this->selectType != selectType) {
        this->selectType = selectType;
        this->update();
    }
}

void LunarCalendarItem::setDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        this->update();
    }
}

void LunarCalendarItem::setLunar(const QString &lunar)
{
    if (this->lunar != lunar) {
        this->lunar = lunar;
        this->update();
    }
}

void LunarCalendarItem::setDayType(const LunarCalendarItem::DayType &dayType)
{
    if (this->dayType != dayType) {
        this->dayType = dayType;
        this->update();
    }
}

void LunarCalendarItem::setDate(const QDate &date, const QString &lunar, const DayType &dayType)
{
    this->date = date;
    this->lunar = lunar;
    this->dayType = dayType;
    this->update();
}

void LunarCalendarItem::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        this->update();
    }
}

void LunarCalendarItem::setWeekColor(const QColor &weekColor)
{
    if (this->weekColor != weekColor) {
        this->weekColor = weekColor;
        this->update();
    }
}

void LunarCalendarItem::setSuperColor(const QColor &superColor)
{
    if (this->superColor != superColor) {
        this->superColor = superColor;
        this->update();
    }
}

void LunarCalendarItem::setLunarColor(const QColor &lunarColor)
{
    if (this->lunarColor != lunarColor) {
        this->lunarColor = lunarColor;
        this->update();
    }
}

void LunarCalendarItem::setCurrentTextColor(const QColor &currentTextColor)
{
    if (this->currentTextColor != currentTextColor) {
        this->currentTextColor = currentTextColor;
        this->update();
    }
}

void LunarCalendarItem::setOtherTextColor(const QColor &otherTextColor)
{
    if (this->otherTextColor != otherTextColor) {
        this->otherTextColor = otherTextColor;
        this->update();
    }
}

void LunarCalendarItem::setSelectTextColor(const QColor &selectTextColor)
{
    if (this->selectTextColor != selectTextColor) {
        this->selectTextColor = selectTextColor;
        this->update();
    }
}

void LunarCalendarItem::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        this->update();
    }
}

void LunarCalendarItem::setCurrentLunarColor(const QColor &currentLunarColor)
{
    if (this->currentLunarColor != currentLunarColor) {
        this->currentLunarColor = currentLunarColor;
        this->update();
    }
}

void LunarCalendarItem::setOtherLunarColor(const QColor &otherLunarColor)
{
    if (this->otherLunarColor != otherLunarColor) {
        this->otherLunarColor = otherLunarColor;
        this->update();
    }
}

void LunarCalendarItem::setSelectLunarColor(const QColor &selectLunarColor)
{
    if (this->selectLunarColor != selectLunarColor) {
        this->selectLunarColor = selectLunarColor;
        this->update();
    }
}

void LunarCalendarItem::setHoverLunarColor(const QColor &hoverLunarColor)
{
    if (this->hoverLunarColor != hoverLunarColor) {
        this->hoverLunarColor = hoverLunarColor;
        this->update();
    }
}

void LunarCalendarItem::setCurrentBgColor(const QColor &currentBgColor)
{
    if (this->currentBgColor != currentBgColor) {
        this->currentBgColor = currentBgColor;
        this->update();
    }
}

void LunarCalendarItem::setOtherBgColor(const QColor &otherBgColor)
{
    if (this->otherBgColor != otherBgColor) {
        this->otherBgColor = otherBgColor;
        this->update();
    }
}

void LunarCalendarItem::setSelectBgColor(const QColor &selectBgColor)
{
    if (this->selectBgColor != selectBgColor) {
        this->selectBgColor = selectBgColor;
        this->update();
    }
}

void LunarCalendarItem::setHoverBgColor(const QColor &hoverBgColor)
{
    if (this->hoverBgColor != hoverBgColor) {
        this->hoverBgColor = hoverBgColor;
        this->update();
    }
}

bool LunarCalendarItem::event(QEvent *event)
{
    event->type();
    if(event->type()==QEvent::ToolTip){

//        qDebug()<<"选中日期的月份"<<date.month();
//        qDebug()<<"选中日期的天数"<<date.day();

        if(date.month()==11 && date.day()==9 ){
            setToolTip(tr("消防宣传日"));
        }
        if(date.month()==3 && date.day()==5 ){
            setToolTip(tr("志愿者服务日"));
        }
        if(date.month()==6 && date.day()==6 ){
            setToolTip(tr("全国爱眼日"));
        }
        if(date.month()==7 && date.day()==7 ){
            setToolTip(tr("抗战纪念日"));
        }
    }
    QWidget::event(event);
}
