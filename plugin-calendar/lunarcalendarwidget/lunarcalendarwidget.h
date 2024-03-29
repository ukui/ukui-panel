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

#ifndef LUNARCALENDARWIDGET_H
#define LUNARCALENDARWIDGET_H

#include <QGSettings/QGSettings>
#include <QWidget>
#include <QDate>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "qfontdatabase.h"
#include "qdatetime.h"
#include "qlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qtoolbutton.h"
#include "qcombobox.h"
#include "qdebug.h"
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include "picturetowhite.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QRadioButton>
#include <QLabel>
#include <QObject>
#include <QMouseEvent>
#include "../../panel/pluginsettings.h"
#include "../../panel/iukuipanelplugin.h"
#include "lunarcalendarinfo.h"
#include "lunarcalendaritem.h"
#include "lunarcalendaryearitem.h"
#include "lunarcalendarmonthitem.h"
#include "customstylePushbutton.h"
#include <QCheckBox>
#include <QLocale>
#include <QProcess>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <unistd.h>


class QLabel;
class statelabel;
class QComboBox;
class LunarCalendarYearItem;
class LunarCalendarMonthItem;
class LunarCalendarItem;

class m_PartLineWidget;

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT LunarCalendarWidget : public QWidget
#else
class LunarCalendarWidget : public QWidget
#endif

{
    Q_OBJECT
    Q_ENUMS(CalendarStyle)
    Q_ENUMS(WeekNameFormat)
    Q_ENUMS(SelectType)

    Q_PROPERTY(CalendarStyle calendarStyle READ getCalendarStyle WRITE setCalendarStyle)
    Q_PROPERTY(QDate date READ getDate WRITE setDate)

    Q_PROPERTY(QColor weekTextColor READ getWeekTextColor WRITE setWeekTextColor)
    Q_PROPERTY(QColor weekBgColor READ getWeekBgColor WRITE setWeekBgColor)

    Q_PROPERTY(bool showLunar READ getShowLunar WRITE setShowLunar)
    Q_PROPERTY(QString bgImage READ getBgImage WRITE setBgImage)
    Q_PROPERTY(SelectType selectType READ getSelectType WRITE setSelectType)

    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)
    Q_PROPERTY(QColor weekColor READ getWeekColor WRITE setWeekColor)
    Q_PROPERTY(QColor superColor READ getSuperColor WRITE setSuperColor)
    Q_PROPERTY(QColor lunarColor READ getLunarColor WRITE setLunarColor)

    Q_PROPERTY(QColor currentTextColor READ getCurrentTextColor WRITE setCurrentTextColor)
    Q_PROPERTY(QColor otherTextColor READ getOtherTextColor WRITE setOtherTextColor)
    Q_PROPERTY(QColor selectTextColor READ getSelectTextColor WRITE setSelectTextColor)
    Q_PROPERTY(QColor hoverTextColor READ getHoverTextColor WRITE setHoverTextColor)

    Q_PROPERTY(QColor currentLunarColor READ getCurrentLunarColor WRITE setCurrentLunarColor)
    Q_PROPERTY(QColor otherLunarColor READ getOtherLunarColor WRITE setOtherLunarColor)
    Q_PROPERTY(QColor selectLunarColor READ getSelectLunarColor WRITE setSelectLunarColor)
    Q_PROPERTY(QColor hoverLunarColor READ getHoverLunarColor WRITE setHoverLunarColor)

    Q_PROPERTY(QColor currentBgColor READ getCurrentBgColor WRITE setCurrentBgColor)
    Q_PROPERTY(QColor otherBgColor READ getOtherBgColor WRITE setOtherBgColor)
    Q_PROPERTY(QColor selectBgColor READ getSelectBgColor WRITE setSelectBgColor)
    Q_PROPERTY(QColor hoverBgColor READ getHoverBgColor WRITE setHoverBgColor)

public:
    enum CalendarStyle {
        CalendarStyle_Red = 0
    };

    enum WeekNameFormat {
        WeekNameFormat_Short = 0,   //短名称
        WeekNameFormat_Normal = 1,  //普通名称
        WeekNameFormat_Long = 2,    //长名称
        WeekNameFormat_En = 3       //英文名称
    };

    enum SelectType {
        SelectType_Rect = 0,        //矩形背景
        SelectType_Circle = 1,      //圆形背景
        SelectType_Triangle = 2,    //带三角标
        SelectType_Image = 3        //图片背景
    };

    explicit LunarCalendarWidget(QWidget *parent = 0);
    ~LunarCalendarWidget();

private:
    QLabel *datelabel;
    QLabel *timelabel;
    QLabel *lunarlabel;
    QTimer *timer;
    QVBoxLayout *timeShow;
    QWidget *widgetTime;
    QPushButton *btnYear;
    QPushButton *btnMonth;
    QPushButton *btnToday;
    QWidget *labWidget;
    QLabel *labBottom;
    QHBoxLayout *labLayout;
    m_PartLineWidget *lineUp;
    m_PartLineWidget *lineDown;
    statelabel *btnPrevYear;
    statelabel *btnNextYear;
    QLabel *yijichooseLabel;
    QCheckBox *yijichoose;
    QVBoxLayout *yijiLayout;
    QWidget *yijiWidget;
    QLabel *yiLabel;
    QLabel *jiLabel;
    QWidget *widgetWeek;
    QWidget *widgetDayBody;
    QWidget *widgetYearBody;
    QWidget *widgetmonthBody;
    QProcess *myProcess;

    QString timemodel = 0;
    bool yijistate = false;
    bool lunarstate =false;
    bool oneRun = true;
//    IUKUIPanelPlugin *mPlugin;
    QString dateShowMode;
    QMap<QString,QString> worktimeinside;
    QMap<QString,QMap<QString,QString>> worktime;
    void analysisWorktimeJs();       //解析js文件
    void downLabelHandle(const QDate &date);
    QFont iconFont;                     //图形字体
    bool btnClick;                      //按钮单击,避开下拉选择重复触发
    QComboBox *cboxYearandMonth;        //年份下拉框
    QLabel *cboxYearandMonthLabel;
    QList<QLabel *> labWeeks;           //顶部星期名称
    QList<LunarCalendarItem *> dayItems;            //日期元素
    QList<LunarCalendarYearItem *> yearItems;       //年份元素
    QList<LunarCalendarMonthItem *> monthItems;     //月份元素
    QFont m_font;

    CalendarStyle calendarStyle;        //整体样式
    bool FirstdayisSun;                 //首日期为周日
    QDate date;                         //当前日期
    QDate clickDate;                    //保存点击日期

    QColor weekTextColor;               //星期名称文字颜色
    QColor weekBgColor;                 //星期名称背景色

    bool showLunar;                     //显示农历
    QString bgImage;                    //背景图片
    SelectType selectType;              //选中模式

    QColor borderColor;                 //边框颜色
    QColor weekColor;                   //周末颜色
    QColor superColor;                  //角标颜色
    QColor lunarColor;                  //农历节日颜色

    QColor currentTextColor;            //当前月文字颜色
    QColor otherTextColor;              //其他月文字颜色
    QColor selectTextColor;             //选中日期文字颜色
    QColor hoverTextColor;              //悬停日期文字颜色

    QColor currentLunarColor;           //当前月农历文字颜色
    QColor otherLunarColor;             //其他月农历文字颜色
    QColor selectLunarColor;            //选中日期农历文字颜色
    QColor hoverLunarColor;             //悬停日期农历文字颜色

    QColor currentBgColor;              //当前月背景颜色
    QColor otherBgColor;                //其他月背景颜色
    QColor selectBgColor;               //选中日期背景颜色
    QColor hoverBgColor;                //悬停日期背景颜色

    QGSettings *calendar_gsettings;

    void setColor(bool mdark_style);
    void _timeUpdate();
    void yijihandle(const QDate &date);
    QString getSettings();
    void setSettings(QString arg);
    QGSettings *style_settings;
    bool dark_style;

    QStringList getLocale();
    void setLocaleCalendar();

protected :
    void wheelEvent(QWheelEvent *event);

private Q_SLOTS:
    void initWidget();
    void initStyle();
    void initDate();
    void changeDate(const QDate &date);
    void yearChanged(const QString &arg1);
    void monthChanged(const QString &arg1);
    void clicked(const QDate &date, const LunarCalendarItem::DayType &dayType);
    void dayChanged(const QDate &date,const QDate &m_date);
    void dateChanged(int year, int month, int day);
    void timerUpdate();
    void customButtonsClicked(int x);
    void yearWidgetChange();
    void monthWidgetChange();


public:
    CalendarStyle getCalendarStyle()    const;
    QDate getDate()                     const;

    QColor getWeekTextColor()           const;
    QColor getWeekBgColor()             const;

    bool getShowLunar()                 const;
    QString getBgImage()                const;
    SelectType getSelectType()          const;

    QColor getBorderColor()             const;
    QColor getWeekColor()               const;
    QColor getSuperColor()              const;
    QColor getLunarColor()              const;

    QColor getCurrentTextColor()        const;
    QColor getOtherTextColor()          const;
    QColor getSelectTextColor()         const;
    QColor getHoverTextColor()          const;

    QColor getCurrentLunarColor()       const;
    QColor getOtherLunarColor()         const;
    QColor getSelectLunarColor()        const;
    QColor getHoverLunarColor()         const;

    QColor getCurrentBgColor()          const;
    QColor getOtherBgColor()            const;
    QColor getSelectBgColor()           const;
    QColor getHoverBgColor()            const;

    QSize sizeHint()                    const;
    QSize minimumSizeHint()             const;
    QString locale                     ;

public Q_SLOTS:

    void updateYearClicked(const QDate &date, const LunarCalendarYearItem::DayType &dayType);
    void updateMonthClicked(const QDate &date, const LunarCalendarMonthItem::DayType &dayType);
    //上一年,下一年
    void showPreviousYear();
    void showNextYear();

    //上一月,下一月
    void showPreviousMonth(bool date_clicked = true);
    void showNextMonth(bool date_clicked = true);

    //转到今天
    void showToday();

    //设置整体样式
    void setCalendarStyle(const CalendarStyle &calendarStyle);
    //设置星期名称格式
    void setWeekNameFormat(bool FirstDayisSun);

    //设置日期
    void setDate(const QDate &date);

    //设置顶部星期名称文字颜色+背景色
    void setWeekTextColor(const QColor &weekTextColor);
    void setWeekBgColor(const QColor &weekBgColor);

    //设置是否显示农历信息
    void setShowLunar(bool showLunar);
    //设置背景图片
    void setBgImage(const QString &bgImage);
    //设置选中背景样式
    void setSelectType(const SelectType &selectType);

    //设置边框颜色
    void setBorderColor(const QColor &borderColor);
    //设置周末颜色
    void setWeekColor(const QColor &weekColor);
    //设置角标颜色
    void setSuperColor(const QColor &superColor);
    //设置农历节日颜色
    void setLunarColor(const QColor &lunarColor);

    //设置当前月文字颜色
    void setCurrentTextColor(const QColor &currentTextColor);
    //设置其他月文字颜色
    void setOtherTextColor(const QColor &otherTextColor);
    //设置选中日期文字颜色
    void setSelectTextColor(const QColor &selectTextColor);
    //设置悬停日期文字颜色
    void setHoverTextColor(const QColor &hoverTextColor);

    //设置当前月农历文字颜色
    void setCurrentLunarColor(const QColor &currentLunarColor);
    //设置其他月农历文字颜色
    void setOtherLunarColor(const QColor &otherLunarColor);
    //设置选中日期农历文字颜色
    void setSelectLunarColor(const QColor &selectLunarColor);
    //设置悬停日期农历文字颜色
    void setHoverLunarColor(const QColor &hoverLunarColor);

    //设置当前月背景颜色
    void setCurrentBgColor(const QColor &currentBgColor);
    //设置其他月背景颜色
    void setOtherBgColor(const QColor &otherBgColor);
    //设置选中日期背景颜色
    void setSelectBgColor(const QColor &selectBgColor);
    //设置悬停日期背景颜色
    void setHoverBgColor(const QColor &hoverBgColor);

Q_SIGNALS:
    void clicked(const QDate &date);
    void selectionChanged();
    void yijiChangeUp();
    void yijiChangeDown();
};


class m_PartLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit m_PartLineWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);

};

class statelabel : public QLabel
{
    Q_OBJECT
public:
    statelabel();

protected:
    void mousePressEvent(QMouseEvent *event);
Q_SIGNALS :
    void labelclick();
};


#endif // LUNARCALENDARWIDGET_H
