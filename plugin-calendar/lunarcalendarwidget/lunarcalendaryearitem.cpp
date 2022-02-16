﻿/*
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

#pragma execution_character_set("utf-8")

#include "lunarcalendaryearitem.h"
#include "qpainter.h"
#include "qevent.h"
#include "qdatetime.h"
#include "qdebug.h"
#include <QGSettings>

LunarCalendarYearItem::LunarCalendarYearItem(QWidget *parent) : QWidget(parent)
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
    selectBgColor = QColor(55,144,250);
    hoverBgColor = QColor(204, 183, 180);
}

void LunarCalendarYearItem::enterEvent(QEvent *)
{
    hover = true;
    this->update();
}

void LunarCalendarYearItem::leaveEvent(QEvent *)
{
    hover = false;
    this->update();
}

void LunarCalendarYearItem::mousePressEvent(QMouseEvent *)
{
    pressed = true;
    this->update();
//    Q_EMIT clicked(date, dayType);
    Q_EMIT yearMessage(date, dayType);
}

void LunarCalendarYearItem::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
    this->update();
}


void LunarCalendarYearItem::paintEvent(QPaintEvent *)
{

    QDate dateNow = QDate::currentDate();

    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景和边框
    drawBg(&painter);

    //对比当前的时间，画选中状态
    if(dateNow.year() == date.year()) {
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
    drawYear(&painter);

}

void LunarCalendarYearItem::drawBg(QPainter *painter)
{
    painter->save();

    //根据当前类型选择对应的颜色
    QColor bgColor = currentBgColor;
    if (dayType == DayType_MonthPre || dayType == DayType_MonthNext) {
        bgColor = otherBgColor;
    }
    painter->restore();
}

void LunarCalendarYearItem::drawBgCurrent(QPainter *painter, const QColor &color)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);

    QRect rect = this->rect();
    painter->drawRoundedRect(rect,4,4);

    painter->restore();
}

void LunarCalendarYearItem::drawBgHover(QPainter *painter, const QColor &color)
{
    painter->save();
    QRect rect = this->rect();
    painter->setPen(QPen(QColor(55,143,250),2));

    painter->drawRoundedRect(rect,4,4);

    painter->restore();
}

void LunarCalendarYearItem::drawYear(QPainter *painter)
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

    painter->setPen(color);

    QFont font;
    font.setPixelSize(side * 0.2);
    //设置文字粗细
    font.setBold(true);
    painter->setFont(font);

    QRect dayRect = QRect(0, 0, width, height / 1.7);
    QString arg = QString::number(date.year()) /*+ "年"*/;
    painter->drawText(dayRect, Qt::AlignHCenter | Qt::AlignBottom, arg);
    painter->restore();
}

bool LunarCalendarYearItem::getSelect() const
{
    return this->select;
}

bool LunarCalendarYearItem::getShowLunar() const
{
    return this->showLunar;
}

QString LunarCalendarYearItem::getBgImage() const
{
    return this->bgImage;
}

LunarCalendarYearItem::SelectType LunarCalendarYearItem::getSelectType() const
{
    return this->selectType;
}

QDate LunarCalendarYearItem::getDate() const
{
    return this->date;
}

QString LunarCalendarYearItem::getLunar() const
{
    return this->lunar;
}

LunarCalendarYearItem::DayType LunarCalendarYearItem::getDayType() const
{
    return this->dayType;
}

QColor LunarCalendarYearItem::getBorderColor() const
{
    return this->borderColor;
}

QColor LunarCalendarYearItem::getWeekColor() const
{
    return this->weekColor;
}

QColor LunarCalendarYearItem::getSuperColor() const
{
    return this->superColor;
}

QColor LunarCalendarYearItem::getLunarColor() const
{
    return this->lunarColor;
}

QColor LunarCalendarYearItem::getCurrentTextColor() const
{
    return this->currentTextColor;
}

QColor LunarCalendarYearItem::getOtherTextColor() const
{
    return this->otherTextColor;
}

QColor LunarCalendarYearItem::getSelectTextColor() const
{
    return this->selectTextColor;
}

QColor LunarCalendarYearItem::getHoverTextColor() const
{
    return this->hoverTextColor;
}

QColor LunarCalendarYearItem::getCurrentLunarColor() const
{
    return this->currentLunarColor;
}

QColor LunarCalendarYearItem::getOtherLunarColor() const
{
    return this->otherLunarColor;
}

QColor LunarCalendarYearItem::getSelectLunarColor() const
{
    return this->selectLunarColor;
}

QColor LunarCalendarYearItem::getHoverLunarColor() const
{
    return this->hoverLunarColor;
}

QColor LunarCalendarYearItem::getCurrentBgColor() const
{
    return this->currentBgColor;
}

QColor LunarCalendarYearItem::getOtherBgColor() const
{
    return this->otherBgColor;
}

QColor LunarCalendarYearItem::getSelectBgColor() const
{
    return this->selectBgColor;
}

QColor LunarCalendarYearItem::getHoverBgColor() const
{
    return this->hoverBgColor;
}

QSize LunarCalendarYearItem::sizeHint() const
{
    return QSize(100, 100);
}

QSize LunarCalendarYearItem::minimumSizeHint() const
{
    return QSize(20, 20);
}

void LunarCalendarYearItem::setSelect(bool select)
{
    if (this->select != select) {
        this->select = select;
        this->update();
    }
}

void LunarCalendarYearItem::setShowLunar(bool showLunar)
{
        this->showLunar = showLunar;
        this->update();
}

void LunarCalendarYearItem::setBgImage(const QString &bgImage)
{
    if (this->bgImage != bgImage) {
        this->bgImage = bgImage;
        this->update();
    }
}

void LunarCalendarYearItem::setSelectType(const LunarCalendarYearItem::SelectType &selectType)
{
    if (this->selectType != selectType) {
        this->selectType = selectType;
        this->update();
    }
}

void LunarCalendarYearItem::setDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        this->update();
    }
}

void LunarCalendarYearItem::setLunar(const QString &lunar)
{
    if (this->lunar != lunar) {
        this->lunar = lunar;
        this->update();
    }
}

void LunarCalendarYearItem::setDayType(const LunarCalendarYearItem::DayType &dayType)
{
    if (this->dayType != dayType) {
        this->dayType = dayType;
        this->update();
    }
}

void LunarCalendarYearItem::setDate(const QDate &date, const QString &lunar, const DayType &dayType)
{
    this->date = date;
    this->lunar = lunar;
    this->dayType = dayType;
    this->update();
}

void LunarCalendarYearItem::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        this->update();
    }
}

void LunarCalendarYearItem::setWeekColor(const QColor &weekColor)
{
    if (this->weekColor != weekColor) {
        this->weekColor = weekColor;
        this->update();
    }
}

void LunarCalendarYearItem::setSuperColor(const QColor &superColor)
{
    if (this->superColor != superColor) {
        this->superColor = superColor;
        this->update();
    }
}

void LunarCalendarYearItem::setLunarColor(const QColor &lunarColor)
{
    if (this->lunarColor != lunarColor) {
        this->lunarColor = lunarColor;
        this->update();
    }
}

void LunarCalendarYearItem::setCurrentTextColor(const QColor &currentTextColor)
{
    if (this->currentTextColor != currentTextColor) {
        this->currentTextColor = currentTextColor;
        this->update();
    }
}

void LunarCalendarYearItem::setOtherTextColor(const QColor &otherTextColor)
{
    if (this->otherTextColor != otherTextColor) {
        this->otherTextColor = otherTextColor;
        this->update();
    }
}

void LunarCalendarYearItem::setSelectTextColor(const QColor &selectTextColor)
{
    if (this->selectTextColor != selectTextColor) {
        this->selectTextColor = selectTextColor;
        this->update();
    }
}

void LunarCalendarYearItem::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        this->update();
    }
}

void LunarCalendarYearItem::setCurrentLunarColor(const QColor &currentLunarColor)
{
    if (this->currentLunarColor != currentLunarColor) {
        this->currentLunarColor = currentLunarColor;
        this->update();
    }
}

void LunarCalendarYearItem::setOtherLunarColor(const QColor &otherLunarColor)
{
    if (this->otherLunarColor != otherLunarColor) {
        this->otherLunarColor = otherLunarColor;
        this->update();
    }
}




