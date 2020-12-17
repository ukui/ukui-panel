/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012-2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "ukuicalendar.h"

#include <QCalendarWidget>
#include <QDate>
#include <QDesktopWidget>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QLocale>
#include <QScopedArrayPointer>
#include <QTimer>
#include <QWheelEvent>
#include <QProcess>
#include "../panel/pluginsettings.h"
#include <QDebug>
#include <QApplication>
#include <QtWebKit/qwebsettings.h>
#include <QStyleOption>
#include <glib.h>
#include <gio/gio.h>
#include <QSize>
#include <QScreen>

#define CALENDAR_HEIGHT (46)
#define CALENDAR_WIDTH (104)

#define WEBVIEW_WIDTH (454)
#define WEBVIEW_MAX_HEIGHT (704)
#define WEBVIEW_MIN_HEIGHT (600)
#define HOUR_SYSTEM_CONTROL "org.ukui.control-center.panel.plugins"
#define HOUR_SYSTEM_24_Horizontal "hh:mm ddd  yyyy/MM/dd"
#define HOUR_SYSTEM_24_Vertical "hh:mm ddd  MM/dd"
#define HOUR_SYSTEM_12_Horizontal   "Ahh:mm ddd  yyyy/MM/dd"
#define HOUR_SYSTEM_12_Vertical   "Ahh:mm ddd  MM/dd"
#define CURRENT_DATE "yyyy/MM/dd dddd"

#define HOUR_SYSTEM_24_Horizontal_CN "hh:mm ddd  yyyy-MM-dd"
#define HOUR_SYSTEM_24_Vertical_CN "hh:mm ddd  MM-dd"
#define HOUR_SYSTEM_12_Horizontal_CN   "Ahh:mm ddd  yyyy-MM-dd"
#define HOUR_SYSTEM_12_Vertical_CN   "Ahh:mm ddd  MM-dd"
#define CURRENT_DATE_CN "yyyy-MM-dd dddd"

#define HOUR_SYSTEM_KEY "hoursystem"
IndicatorCalendar::IndicatorCalendar(const IUKUIPanelPluginStartupInfo &startupInfo):
    QWidget(),
    IUKUIPanelPlugin(startupInfo),
    mTimer(new QTimer(this)),
    mUpdateInterval(1),
    mAutoRotate(true),
    mbActived(false),
    mbIsNeedUpdate(false),
    mbHasCreatedWebView(false),
    mViewWidht(WEBVIEW_WIDTH),
    mViewHeight(0),
    mPopupContent(NULL)
{

    mMainWidget = new QWidget();
    mContent = new CalendarActiveLabel(this);

    QVBoxLayout *borderLayout = new QVBoxLayout(mMainWidget);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->setAlignment(Qt::AlignCenter);
    borderLayout->addWidget(mContent);

    mContent->setObjectName(QLatin1String("WorldClockContent"));
    mContent->setAlignment(Qt::AlignCenter);

    settingsChanged();
    initializeCalendar();
    mTimer->setTimerType(Qt::PreciseTimer);

    hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal;
    hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical;
    hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal;
    hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical;
    current_date=CURRENT_DATE;

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));
    connect(mContent, SIGNAL(wheelScrolled(int)), SLOT(wheelScrolled(int)));
//    connect(mWebViewDiag, SIGNAL(deactivated()), SLOT(hidewebview()));
    const QByteArray id(HOUR_SYSTEM_CONTROL);
    if(QGSettings::isSchemaInstalled(id)) {
    gsettings = new QGSettings(id);

    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key)
    {
        if (key == HOUR_SYSTEM_KEY)
        {
            if(gsettings->keys().contains("hoursystem"))
            {
            hourSystemMode=gsettings->get("hoursystem").toString();
            }
            else
                hourSystemMode=24;
        }
        else if(key == "calendar")
        {

            mbHasCreatedWebView = false;
            initializeCalendar();
        }
        else if(key == "firstday")
        {
            qDebug()<<"key == firstday";
            mbHasCreatedWebView = false;
            initializeCalendar();
        }
        else if(key == "date")
        {
            qDebug()<<"key == date";
            if(gsettings->keys().contains("date"))
            {
                if(QString::compare(gsettings->get("date").toString(),"cn"))
                {
                    qDebug()<<" date   en ";
                    hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal_CN;
                    hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical_CN;
                    hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal_CN;
                    hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical_CN;
                    current_date=CURRENT_DATE_CN;
                }
                else
                {
                    qDebug()<<" date   cn ";
                    hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal;
                    hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical;
                    hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal;
                    hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical;
                    current_date=CURRENT_DATE;
                }
            }
            updateTimeText();
        }
    });
    }

    setTimeShowStyle();
    mContent->setWordWrap(true);
    setToolTip();
}

IndicatorCalendar::~IndicatorCalendar()
{
    if(mMainWidget != NULL)
    {
        mMainWidget->deleteLater();
    }
    if(mContent != NULL)
    {
        mContent->deleteLater();
    }
    if(mPopupContent != NULL)
    {
        mPopupContent->deleteLater();
    }
}

void IndicatorCalendar::setToolTip()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timeZoneName = mActiveTimeZone;
    if (timeZoneName == QLatin1String("local"))
        timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());
    QTimeZone timeZone(timeZoneName.toLatin1());
    QDateTime tzNow = now.toTimeZone(timeZone);
    mContent->setToolTip(tzNow.toString(CURRENT_DATE));
}

void IndicatorCalendar::timeout()
{
    if (QDateTime{}.time().msec() > 500)
        restartTimer();
    updateTimeText();
    setToolTip();
}

void IndicatorCalendar::updateTimeText()
{
    QDateTime now = QDateTime::currentDateTime();
    QString timeZoneName = mActiveTimeZone;
    if (timeZoneName == QLatin1String("local"))
        timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());
    QTimeZone timeZone(timeZoneName.toLatin1());
    QDateTime tzNow = now.toTimeZone(timeZone);


    bool isUpToDate(true);
    if (!mShownTime.isValid()) // first time or forced update
    {
        isUpToDate = false;
        if (mUpdateInterval < 60000)
            mShownTime = tzNow.addSecs(-tzNow.time().msec()); // s
        else if (mUpdateInterval < 3600000)
            mShownTime = tzNow.addSecs(-tzNow.time().second()); // m
        else
            mShownTime = tzNow.addSecs(-tzNow.time().minute() * 60 - tzNow.time().second()); // h
    }
    else
    {
        qint64 diff = mShownTime.secsTo(tzNow);
        if (mUpdateInterval < 60000)
        {
            if (diff < 0 || diff >= 1)
            {
                isUpToDate = false;
                mShownTime = tzNow.addSecs(-tzNow.time().msec());
            }
        }
        else if (mUpdateInterval < 3600000)
        {
            if (diff < 0 || diff >= 60)
            {
                isUpToDate = false;
                mShownTime = tzNow.addSecs(-tzNow.time().second());
            }
        }
        else if (diff < 0 || diff >= 3600)
        {
            isUpToDate = false;
            mShownTime = tzNow.addSecs(-tzNow.time().minute() * 60 - tzNow.time().second());
        }
    }

    //    if (!isUpToDate || mbIsNeedUpdate)
    //    {
    const QSize old_size = mContent->sizeHint();
    QString str;
    const QByteArray id(HOUR_SYSTEM_CONTROL);
    if(QGSettings::isSchemaInstalled(id))
    {
        gsettings = new QGSettings(id);
        QStringList keys = gsettings->keys();
        if(keys.contains("hoursystem"))
            hourSystemMode=gsettings->get("hoursystem").toString();
        if(!gsettings)
            return;
        if(!QString::compare("24",hourSystemMode))
        {
            if(panel()->isHorizontal())
                str=tzNow.toString(hourSystem_24_horzontal);
            else
                str=tzNow.toString(hourSystem_24_vartical);
        }
        else
        {
            if(panel()->isHorizontal())
            {
                str=tzNow.toString(hourSystem_12_horzontal);
            }
            else
            {
                str = tzNow.toString(hourSystem_12_vartical);
                str.replace("AM","AM ");
                str.replace("PM","PM ");
            }
        }
    }
    else
    {
        if(panel()->isHorizontal())
            str=tzNow.toString(hourSystem_24_horzontal);
        else
            str=tzNow.toString(hourSystem_24_vartical);
    }
    mContent->setText(str);
    mContent->setStyleSheet(
                //正常状态样式
                "QLabel{"
                "border-width:  0px;"                     //边框宽度像素
                "border-radius: 6px;"                       //边框圆角半径像素
                "font-size:     14px;"                      //字体，字体大小
                "padding:       0px;"                       //填衬
                "text-align:center;"                        //文本居中
                "}"
                //鼠标悬停样式
                "QLabel:hover{"
                "background-color:rgba(190,216,239,20%);"
                "border-radius:6px;"                       //边框圆角半径像素
                "}"
                //鼠标按下样式
                "QLabel:pressed{"
                "background-color:rgba(190,216,239,12%);"
                "}"
                );
    updatePopupContent();
    mbIsNeedUpdate = false;
}

void IndicatorCalendar::setTimeText()
{
    mShownTime = QDateTime(); // force an update
    updateTimeText();
}

void IndicatorCalendar::restartTimer()
{
    mTimer->stop();
    // check the time every second even if the clock doesn't show seconds
    // because otherwise, the shown time might be vey wrong after resume
    mTimer->setInterval(1000);

    int delay = static_cast<int>(1000 - (static_cast<long long>(QTime::currentTime().msecsSinceStartOfDay()) % 1000));
    QTimer::singleShot(delay, Qt::PreciseTimer, this, &IndicatorCalendar::updateTimeText);
    QTimer::singleShot(delay, Qt::PreciseTimer, mTimer, SLOT(start()));
}

void IndicatorCalendar::settingsChanged()
{
    PluginSettings *_settings = settings();

    QString oldFormat = mFormat;

    mTimeZones.clear();
    QList<QMap<QString, QVariant> > array = _settings->readArray(QLatin1String("timeZones"));
    for (const auto &map : array)
    {
        QString timeZoneName = map.value(QLatin1String("timeZone"), QString()).toString();
        mTimeZones.append(timeZoneName);
        mTimeZoneCustomNames[timeZoneName] = map.value(QLatin1String("customName"),
                                                       QString()).toString();
    }

    if (mTimeZones.isEmpty())
        mTimeZones.append(QLatin1String("local"));

    mDefaultTimeZone = _settings->value(QLatin1String("defaultTimeZone"), QString()).toString();
    if (mDefaultTimeZone.isEmpty())
        mDefaultTimeZone = mTimeZones[0];
    mActiveTimeZone = mDefaultTimeZone;


    bool longTimeFormatSelected = false;

    QString formatType = _settings->value(QLatin1String("formatType"), QString()).toString();
    QString dateFormatType = _settings->value(QLatin1String("dateFormatType"), QString()).toString();
    bool advancedManual = _settings->value(QLatin1String("useAdvancedManualFormat"), false).toBool();

    // backward compatibility
    if (formatType == QLatin1String("custom"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = true;
    }
    else if (formatType == QLatin1String("short"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = false;
    }
    else if ((formatType == QLatin1String("full")) ||
             (formatType == QLatin1String("long")) ||
             (formatType == QLatin1String("medium")))
    {
        formatType = QLatin1String("long-timeonly");
        dateFormatType = QLatin1String("long");
        advancedManual = false;
    }

    if (formatType == QLatin1String("long-timeonly"))
        longTimeFormatSelected = true;

    bool timeShowSeconds = _settings->value(QLatin1String("timeShowSeconds"), false).toBool();
    bool timePadHour = _settings->value(QLatin1String("timePadHour"), false).toBool();
    bool timeAMPM = _settings->value(QLatin1String("timeAMPM"), false).toBool();

    // timezone
    bool showTimezone = _settings->value(QLatin1String("showTimezone"), false).toBool() && !longTimeFormatSelected;

    QString timezonePosition = _settings->value(QLatin1String("timezonePosition"), QString()).toString();
    QString timezoneFormatType = _settings->value(QLatin1String("timezoneFormatType"), QString()).toString();

    // date
    bool showDate = _settings->value(QLatin1String("showDate"), false).toBool();

    QString datePosition = _settings->value(QLatin1String("datePosition"), QString()).toString();

    bool dateShowYear = _settings->value(QLatin1String("dateShowYear"), false).toBool();
    bool dateShowDoW = _settings->value(QLatin1String("dateShowDoW"), false).toBool();
    bool datePadDay = _settings->value(QLatin1String("datePadDay"), false).toBool();
    bool dateLongNames = _settings->value(QLatin1String("dateLongNames"), false).toBool();

    // advanced
    QString customFormat = _settings->value(QLatin1String("customFormat"), tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'")).toString();

    if (advancedManual)
        mFormat = customFormat;
    else
    {

        QLocale locale = QLocale(QLocale::AnyLanguage, QLocale().country());


        if (formatType == QLatin1String("short-timeonly"))
            mFormat = locale.timeFormat(QLocale::ShortFormat);
        else if (formatType == QLatin1String("long-timeonly"))
            mFormat = locale.timeFormat(QLocale::LongFormat);
        else // if (formatType == QLatin1String("custom-timeonly"))
            mFormat = QString(QLatin1String("%1:mm%2%3")).arg(timePadHour ? QLatin1String("hh") : QLatin1String("h")).arg(timeShowSeconds ? QLatin1String(":ss") : QLatin1String("")).arg(timeAMPM ? QLatin1String(" A") : QLatin1String(""));

        if (showTimezone)
        {
            QString timezonePortion;
            if (timezoneFormatType == QLatin1String("short"))
                timezonePortion = QLatin1String("TTTT");
            else if (timezoneFormatType == QLatin1String("long"))
                timezonePortion = QLatin1String("TTTTT");
            else if (timezoneFormatType == QLatin1String("offset"))
                timezonePortion = QLatin1String("T");
            else if (timezoneFormatType == QLatin1String("abbreviation"))
                timezonePortion = QLatin1String("TTT");
            else if (timezoneFormatType == QLatin1String("iana"))
                timezonePortion = QLatin1String("TT");
            else // if (timezoneFormatType == QLatin1String("custom"))
                timezonePortion = QLatin1String("TTTTTT");

            if (timezonePosition == QLatin1String("below"))
                mFormat = mFormat + QLatin1String("'<br/>'") + timezonePortion;
            else if (timezonePosition == QLatin1String("above"))
                mFormat = timezonePortion + QLatin1String("'<br/>'") + mFormat;
            else if (timezonePosition == QLatin1String("before"))
                mFormat = timezonePortion + QLatin1String(" ") + mFormat;
            else // if (timezonePosition == QLatin1String("after"))
                mFormat = mFormat + QLatin1String(" ") + timezonePortion;
        }


        QString datePortion;
        if (dateFormatType == QLatin1String("short"))
            datePortion = locale.dateFormat(QLocale::ShortFormat);
        else if (dateFormatType == QLatin1String("long"))
            datePortion = locale.dateFormat(QLocale::LongFormat);
        else if (dateFormatType == QLatin1String("iso"))
            datePortion = QLatin1String("yyyy-MM-dd");
        else // if (dateFormatType == QLatin1String("custom"))
        {
            QString datePortionOrder;
            QString dateLocale = locale.dateFormat(QLocale::ShortFormat).toLower();
            int yearIndex = dateLocale.indexOf("y");
            int monthIndex = dateLocale.indexOf("m");
            int dayIndex = dateLocale.indexOf("d");
            if (yearIndex < dayIndex)
            // Big-endian (year, month, day) (yyyy MMMM dd, dddd) -> in some Asia countires like China or Japan
                datePortionOrder = QLatin1String("%1%2%3 %4%5%6");
            else if (monthIndex < dayIndex)
            // Middle-endian (month, day, year) (dddd, MMMM dd yyyy) -> USA
                datePortionOrder = QLatin1String("%6%5%3 %4%2%1");
            else
            // Little-endian (day, month, year) (dddd, dd MMMM yyyy) -> most of Europe
                datePortionOrder = QLatin1String("%6%5%4 %3%2%1");
            datePortion = datePortionOrder.arg(dateShowYear ? QLatin1String("yyyy") : QLatin1String("")).arg(dateShowYear ? QLatin1String(" ") : QLatin1String("")).arg(dateLongNames ? QLatin1String("MMMM") : QLatin1String("MMM")).arg(datePadDay ? QLatin1String("dd") : QLatin1String("d")).arg(dateShowDoW ? QLatin1String(", ") : QLatin1String("")).arg(dateShowDoW ? (dateLongNames ? QLatin1String("dddd") : QLatin1String("ddd")) : QLatin1String(""));
        }

        //mFormat = datePortion + QLatin1String(" ") + mFormat;//date show before time
        mFormat = mFormat + QLatin1String(" ") + datePortion;//date show before time
    }

    if ((oldFormat != mFormat))
    {
        int update_interval;
        QString format = mFormat;
        format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
        //don't support updating on milisecond basis -> big performance hit
        if (format.contains(QLatin1String("s")))
            update_interval = 1000;
        else if (format.contains(QLatin1String("m")))
            update_interval = 60000;
        else
            update_interval = 3600000;

        if (update_interval != mUpdateInterval)
        {
            mUpdateInterval = update_interval;
            restartTimer();
        }
    }

    bool autoRotate = settings()->value(QLatin1String("autoRotate"), true).toBool();
    if (autoRotate != mAutoRotate)
    {
        mAutoRotate = autoRotate;
        realign();
    }

    setTimeText();
}


void IndicatorCalendar::wheelScrolled(int delta)
{
    if (mTimeZones.count() > 1)
    {
        mActiveTimeZone = mTimeZones[(mTimeZones.indexOf(mActiveTimeZone) + ((delta > 0) ? -1 : 1) + mTimeZones.size()) % mTimeZones.size()];
        setTimeText();
    }
}

/*when widget is loading need initialize here*/
void IndicatorCalendar::initializeCalendar()
{
    QByteArray id(HOUR_SYSTEM_CONTROL);
    CalendarShowMode showCalendar = defaultMode;
    QString lunarOrsolar;
    QString firstDay;
    int iScreenHeight = QApplication::screens().at(0)->size().height() - panel()->panelSize();
    if(iScreenHeight > WEBVIEW_MAX_HEIGHT)
    {
        mViewHeight = WEBVIEW_MAX_HEIGHT;
    }
    else
    {
        mViewHeight = WEBVIEW_MIN_HEIGHT;
    }
    if(QGSettings::isSchemaInstalled(id))
    {
        if(!gsettings)
        {
            qDebug()<<"get gsetting error!!!";
            return;
        }
        if(gsettings->keys().contains("calendar"))
        {
            lunarOrsolar= gsettings->get("calendar").toString();
        }
        if(gsettings->keys().contains("firstday"))
        {
            firstDay= gsettings->get("firstday").toString();
        }
        qDebug()<<"QLocale::system().name():"<<QLocale::system().name();
        if (QLocale::system().name() == "zh_CN")
        {

            if(lunarOrsolar == "lunar")
            {
                if(firstDay == "sunday")
                {
                    showCalendar = lunarSunday;
                }
                else if(firstDay == "monday")
                {
                    showCalendar = lunarMonday;
                }
                if(iScreenHeight > WEBVIEW_MAX_HEIGHT)
                {
                    mViewHeight = WEBVIEW_MAX_HEIGHT;
                }
                else
                {
                    mViewHeight = WEBVIEW_MIN_HEIGHT;
                }
            }
            else if(lunarOrsolar == "solarlunar")
            {
                if(firstDay == "sunday")
                {
                    showCalendar = solarSunday;
                }
                else if(firstDay == "monday")
                {
                    showCalendar = solarMonday;
                }
                mViewHeight = WEBVIEW_MIN_HEIGHT;
            }
        }
        else// for internaitional
        {
            if(firstDay == "sunday")
            {
                showCalendar = solarSunday;
            }
            else if(firstDay == "monday")
            {
                showCalendar = solarMonday;
            }
            mViewHeight = WEBVIEW_MIN_HEIGHT;
        }
    }
}


void IndicatorCalendar::deletePopup()
{
    mPopupContent = NULL;
}

QString IndicatorCalendar::formatDateTime(const QDateTime &datetime, const QString &timeZoneName)
{
    QTimeZone timeZone(timeZoneName.toLatin1());
    QDateTime tzNow = datetime.toTimeZone(timeZone);
    return tzNow.toString(preformat(tzNow,mFormat, timeZone));
}

void IndicatorCalendar::updatePopupContent()
{
    if (mPopupContent)
    {
        QDateTime now = QDateTime::currentDateTime();
        QStringList allTimeZones;
        bool hasTimeZone = formatHasTimeZone(mFormat);

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        QString timeZoneName;
        for (QStringList::iterator it = mTimeZones.begin(); it != mTimeZones.end(); ++it)
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
        for (QString timeZoneName : qAsConst(mTimeZones))
#endif
        {
            if (timeZoneName == QLatin1String("local"))
                timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());

            QString formatted = formatDateTime(now, timeZoneName);

            if (!hasTimeZone)
                formatted += QLatin1String("<br/>") + QString::fromLatin1(QTimeZone(timeZoneName.toLatin1()).id());

            allTimeZones.append(formatted);
        }

        mPopupContent->setText(allTimeZones.join(QLatin1String("<hr/>")));
    }
}

bool IndicatorCalendar::formatHasTimeZone(QString format)
{
    format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
    return format.toLower().contains(QLatin1String("t"));
}

QString IndicatorCalendar::preformat(const QDateTime &dateTime,const QString &format, const QTimeZone &timeZone )
{
    QString result = format;
    int from = 0;
    for (;;)
    {
        int apos = result.indexOf(QLatin1Char('\''), from);
        int tz = result.indexOf(QLatin1Char('T'), from);
        if ((apos != -1) && (tz != -1))
        {
            if (apos > tz)
                apos = -1;
            else
                tz = -1;
        }
        if (apos != -1)
        {
            from = apos + 1;
            apos = result.indexOf(QLatin1Char('\''), from);
            if (apos == -1) // misformat
                break;
            from = apos + 1;
        }
        else if (tz != -1)
        {
            int length = 1;
            for (; result[tz + length] == QLatin1Char('T'); ++length);
            if (length > 6)
                length = 6;
            QString replacement;
            switch (length)
            {
            case 1:
                replacement = timeZone.displayName(dateTime, QTimeZone::OffsetName);
                if (replacement.startsWith(QLatin1String("UTC")))
                    replacement = replacement.mid(3);
                    qDebug()<<"case 1 replacement = "<<replacement;
                break;

            case 2:
                replacement = QString::fromLatin1(timeZone.id());
                 qDebug()<<"case 2 replacement = "<<replacement;
                break;

            case 3:
                replacement = timeZone.abbreviation(dateTime);
                 qDebug()<<"case 3 replacement = "<<replacement;
                break;

            case 4:
                replacement = timeZone.displayName(dateTime, QTimeZone::ShortName);
                 qDebug()<<"case 4 replacement = "<<replacement;
                break;

            case 5:
                replacement = timeZone.displayName(dateTime, QTimeZone::LongName);
                 qDebug()<<"case 5 replacement = "<<replacement;
                break;

            case 6:
                replacement = mTimeZoneCustomNames[QString::fromLatin1(timeZone.id())];
                 qDebug()<<"case 6 replacement = "<<replacement;
                break;
            default:
                break;
            }

            if ((tz > 0) && (result[tz - 1] == QLatin1Char('\'')))
            {
                --tz;
                ++length;
            }
            else
                replacement.prepend(QLatin1Char('\''));

            if (result[tz + length] == QLatin1Char('\''))
                ++length;
            else
                replacement.append(QLatin1Char('\''));
            result.replace(tz, length, replacement);
            from = tz + replacement.length();
        }
        else
            break;
    }
    return result;
}

void IndicatorCalendar::realign()
{
    setTimeShowStyle();
}

void IndicatorCalendar::setTimeShowStyle()
{
    int size = panel()->panelSize() - 3;
    if(panel()->isHorizontal())
    {
       mContent->setFixedSize(CALENDAR_WIDTH, size);
    }
    else
    {
        mContent->setFixedSize(size, CALENDAR_WIDTH);
    }
    mbIsNeedUpdate = true;
    timeout();
}

CalendarActiveLabel::CalendarActiveLabel(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QLabel(parent),
    mPlugin(plugin)
{
    w=new frmLunarCalendarWidget();
    w->hide();
    state=ST_HIDE;
}
CalendarActiveLabel::~CalendarActiveLabel()
{
    delete w;
}
void CalendarActiveLabel::wheelEvent(QWheelEvent *event)
{
    Q_EMIT wheelScrolled(event->delta());

    QLabel::wheelEvent(event);
}

void CalendarActiveLabel::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        Q_EMIT leftMouseButtonClicked();
        break;

    case Qt::MidButton:
        Q_EMIT middleMouseButtonClicked();
        break;

    default:;
    }

    QLabel::mouseReleaseEvent(event);
}

void CalendarActiveLabel::mousePressEvent(QMouseEvent *event)
{
    const Qt::MouseButton b = event->button();

    if (Qt::LeftButton == b)
    {
        w->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), w->size()));
        w->show();
    }
    //QWidget::mousePressEvent(event);
}

void CalendarActiveLabel::contextMenuEvent(QContextMenuEvent *event)
{
    PopupMenu *menuCalender=new PopupMenu(this);
    menuCalender->setAttribute(Qt::WA_DeleteOnClose);

    menuCalender->addAction(QIcon::fromTheme("document-page-setup"),
                   tr("Time and Date Setting"),
                   this, SLOT(setControlTime())
                  );
    menuCalender->addAction(QIcon::fromTheme("document-page-setup"),
                   tr("Config panel"),
                   this, SLOT(setUpPanel())
                  );
    menuCalender->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), menuCalender->sizeHint()));
    menuCalender->show();
}

void CalendarActiveLabel::setControlTime()
{
    QProcess *process =new QProcess(this);
    process->startDetached("ukui-control-center -t");
}

void CalendarActiveLabel::setUpPanel()
{
    QProcess *process =new QProcess(this);
    process->startDetached("ukui-control-center -d");
}

