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
#define POPUP_BORDER_SPACING 4
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
    mPopupContent(NULL),
    mWebViewDiag(NULL)
{

    mMainWidget = new QWidget();
    mContent = new CalendarActiveLabel(this);
    mWebViewDiag = new UkuiWebviewDialog(this);

    QVBoxLayout *borderLayout = new QVBoxLayout(mMainWidget);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->setAlignment(Qt::AlignCenter);
    borderLayout->addWidget(mContent);

    mContent->setObjectName(QLatin1String("WorldClockContent"));
    mContent->setAlignment(Qt::AlignCenter);

    mTimer->setTimerType(Qt::PreciseTimer);
    const QByteArray id(HOUR_SYSTEM_CONTROL);
    gsettings = new QGSettings(id);
    if(QString::compare(gsettings->get("date").toString(),"cn"))
    {
            hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal_CN;
            hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical_CN;
            hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal_CN;
            hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical_CN;
            current_date=CURRENT_DATE_CN;
        }
        else
        {
            hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal;
            hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical;
            hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal;
            hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical;
            current_date=CURRENT_DATE;
        }

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));
    connect(mWebViewDiag, SIGNAL(deactivated()), SLOT(hidewebview()));
    if(QGSettings::isSchemaInstalled(id)) {
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
            if(key == "calendar")
            {

                mbHasCreatedWebView = false;
                initializeCalendar();
            }
            if(key == "firstday")
            {
                qDebug()<<"key == firstday";
                mbHasCreatedWebView = false;
                initializeCalendar();
            }
            if(key == "date")
            {
                if(gsettings->keys().contains("date"))
                {
                    if(QString::compare(gsettings->get("date").toString(),"cn"))
                    {
                        hourSystem_24_horzontal=HOUR_SYSTEM_24_Horizontal_CN;
                        hourSystem_24_vartical=HOUR_SYSTEM_24_Vertical_CN;
                        hourSystem_12_horzontal=HOUR_SYSTEM_12_Horizontal_CN;
                        hourSystem_12_vartical=HOUR_SYSTEM_12_Vertical_CN;
                        current_date=CURRENT_DATE_CN;
                    }
                    else
                    {
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

    initializeCalendar();
    setTimeShowStyle();
    mContent->setWordWrap(true);
}

IndicatorCalendar::~IndicatorCalendar()
{
    if(mMainWidget != NULL)
    {
        mMainWidget->deleteLater();
    }
    if(mWebViewDiag != NULL)
    {
        mWebViewDiag->deleteLater();
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

void IndicatorCalendar::timeout()
{
    if (QDateTime{}.time().msec() > 500)
        restartTimer();
    updateTimeText();
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
    //const QSize old_size = mContent->sizeHint();
    QString str;
    const QByteArray _id("org.ukui.style");
    QGSettings *fgsettings = new QGSettings(_id);
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

    QString style;
    int font_size = fgsettings->get("system-font-size").toInt() + mContent->mPlugin->panel()->panelSize() / 23 - 1;
    style.sprintf( //正常状态样式
                   "QLabel{"
                   "border-width:  0px;"                     //边框宽度像素
                   "border-radius: 6px;"                       //边框圆角半径像素
                 "font-size:     %dpx;"                      //字体，字体大小
                   "color:         rgba(255,255,255,100%%);"    //字体颜色
                   "padding:       0px;"                       //填衬
                   "text-align:center;"                        //文本居中
                   "}"
                   //鼠标悬停样式
                   "QLabel:hover{"
                   "background-color:rgba(190,216,239,20%%);"
                   "border-radius:6px;"                       //边框圆角半径像素
                   "}"
                   //鼠标按下样式
                   "QLabel:pressed{"
                   "background-color:rgba(190,216,239,12%%);"
                   "}", font_size);
    mContent->setStyleSheet(style);
    mContent->setText(str);


    mbIsNeedUpdate = false;
}

void IndicatorCalendar::restartTimer()
{
    mTimer->stop();
    // check the time every second even if the clock doesn't show seconds
    // because otherwise, the shown time might be vey wrong after resume
    //    mTimer->setInterval(1000);
    QString delaytime=QTime::currentTime().toString();
    QList<QString> pathresult=delaytime.split(":");
    int second=pathresult.at(2).toInt();
    if(second==0){
        mTimer->setInterval(60*1000);
    }else{
        mTimer->setInterval((60-second)*1000);
    }

    int delay = static_cast<int>(1000 - (static_cast<long long>(QTime::currentTime().msecsSinceStartOfDay()) % 1000));
    QTimer::singleShot(delay, Qt::PreciseTimer, this, &IndicatorCalendar::updateTimeText);
    QTimer::singleShot(delay, Qt::PreciseTimer, mTimer, SLOT(start()));
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

    if(mWebViewDiag != NULL )
    {
        if(!mbHasCreatedWebView)
        {
            mWebViewDiag->creatwebview(showCalendar,panel()->panelSize());
            mbHasCreatedWebView = true;
        }
    }
}

/**
 * @brief IndicatorCalendar::activated
 * @param reason
 * 如下两种方式也可以设置位置，由于ui问题弃用
 * 1.mWebViewDiag->setGeometry(calculatePopupWindowPos(QSize(mViewWidht+POPUP_BORDER_SPACING,mViewHeight+POPUP_BORDER_SPACING)));
 * 2.
//        QRect screen = QApplication::desktop()->availableGeometry();
//        switch (panel()->position()) {
//        case IUKUIPanel::PositionBottom:
//            mWebViewDiag->move(screen.width()-mViewWidht-POPUP_BORDER_SPACING,screen.height()-mViewHeight-POPUP_BORDER_SPACING);
//            break;
//        case IUKUIPanel::PositionTop:
//            mWebViewDiag->move(screen.width()-mViewWidht-POPUP_BORDER_SPACING,panel()->panelSize()+POPUP_BORDER_SPACING);
//            break;
//        case IUKUIPanel::PositionLeft:
//            mWebViewDiag->move(panel()->panelSize()+POPUP_BORDER_SPACING,screen.height()-mViewHeight-POPUP_BORDER_SPACING);
//            break;
//        default:
//            mWebViewDiag->setGeometry(calculatePopupWindowPos(QSize(mViewWidht+POPUP_BORDER_SPACING,mViewHeight+POPUP_BORDER_SPACING)));
//            break;
//        }
 */
void IndicatorCalendar::activated(ActivationReason reason)
{
    if(mWebViewDiag != NULL )
    {
        mViewHeight = WEBVIEW_MAX_HEIGHT;
        if(gsettings->get("calendar").toString() == "solarlunar")
            mViewHeight = WEBVIEW_MIN_HEIGHT;
        if (QLocale::system().name() != "zh_CN")
            mViewHeight = WEBVIEW_MIN_HEIGHT;
        int iScreenHeight = QApplication::screens().at(0)->size().height() - panel()->panelSize();
        if (iScreenHeight < WEBVIEW_MAX_HEIGHT) {
            mViewHeight = iScreenHeight;
            if (iScreenHeight >= WEBVIEW_MIN_HEIGHT)
                mViewHeight = WEBVIEW_MIN_HEIGHT;;
        }
        modifyCalendarWidget();
        mWebViewDiag->show();
        if(!mbActived)
        {
            mWebViewDiag->setHidden(false);
            mWebViewDiag->webview()->reload();
            mbActived = true;
        }
        else
        {
            mWebViewDiag->setHidden(true);
            mWebViewDiag->webview()->reload();
            mbActived = false;
        }
    }
}


void IndicatorCalendar::deletePopup()
{
    mPopupContent = NULL;
}

void IndicatorCalendar::hidewebview()
{
    mWebViewDiag->setHidden(true);
    mbActived = false;
    mWebViewDiag->webview()->reload();
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

/**
 * @brief IndicatorCalendar::modifyCalendarWidget
 * 任务栏上弹出窗口的位置标准为距离屏幕边缘及任务栏边缘分别为4像素
 */
void IndicatorCalendar::modifyCalendarWidget()
{
       int totalHeight = qApp->primaryScreen()->size().height() + qApp->primaryScreen()->geometry().y();
       int totalWidth = qApp->primaryScreen()->size().width() + qApp->primaryScreen()->geometry().x();

       switch (panel()->position()) {
       case IUKUIPanel::PositionBottom:
           mWebViewDiag->setGeometry(totalWidth-mViewWidht-4,totalHeight-panel()->panelSize()-mViewHeight-4,mViewWidht,mViewHeight);
           break;
       case IUKUIPanel::PositionTop:
           mWebViewDiag->setGeometry(totalWidth-mViewWidht-4,qApp->primaryScreen()->geometry().y()+panel()->panelSize()+4,mViewWidht,mViewHeight);
           break;
       case IUKUIPanel::PositionLeft:
           mWebViewDiag->setGeometry(qApp->primaryScreen()->geometry().x()+panel()->panelSize()+4,totalHeight-mViewHeight-4,mViewWidht,mViewHeight);
           break;
       case IUKUIPanel::PositionRight:
           mWebViewDiag->setGeometry(totalWidth-panel()->panelSize()-mViewWidht-4,totalHeight-mViewHeight-4,mViewWidht,mViewHeight);
           break;
       default:
           mWebViewDiag->setGeometry(qApp->primaryScreen()->geometry().x()+panel()->panelSize()+4,totalHeight-mViewHeight,mViewWidht,mViewHeight);
           break;
       }
}

CalendarActiveLabel::CalendarActiveLabel(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QLabel(parent),
    mPlugin(plugin)
{
    QTimer::singleShot(1000,[this] {setToolTip(tr("Time and Date")); });
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

void CalendarActiveLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menuCalender=new QMenu(this);
    menuCalender->setAttribute(Qt::WA_DeleteOnClose);

    menuCalender->addAction(QIcon::fromTheme("document-page-setup-symbolic"),
                   tr("Time and Date Setting"),
                   this, SLOT(setControlTime())
                  );
    menuCalender->addAction(QIcon::fromTheme("document-page-setup-symbolic"),
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

