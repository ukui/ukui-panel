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
#include <QSettings>
#include <QDir>

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

#define HOUR_SYSTEM_KEY  "hoursystem"
#define SYSTEM_FONT_SIZE "systemFontSize"
#define SYSTEM_FONT_SET  "org.ukui.style"

QString calendar_version;

IndicatorCalendar::IndicatorCalendar(const IUKUIPanelPluginStartupInfo &startupInfo):
    QWidget(),
    IUKUIPanelPlugin(startupInfo),
    mTimer(new QTimer(this)),
    mUpdateInterval(1),
    mbActived(false),
    mbHasCreatedWebView(false),
    mViewWidht(WEBVIEW_WIDTH),
    mViewHeight(0),
    mWebViewDiag(NULL)
{

    mMainWidget = new QWidget();
    mContent = new CalendarActiveLabel(this);
    mWebViewDiag = new UkuiWebviewDialog(this);

    QVBoxLayout *borderLayout = new QVBoxLayout(this);

    mLayout = new UKUi::GridLayout(mMainWidget);
    setLayout(mLayout);
    mLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setSpacing(0);
    mLayout->setAlignment(Qt::AlignCenter);
    mLayout->addWidget(mContent);

    mContent->setObjectName(QLatin1String("WorldClockContent"));
    mContent->setAlignment(Qt::AlignCenter);

    mTimer->setTimerType(Qt::VeryCoarseTimer);
    const QByteArray id(HOUR_SYSTEM_CONTROL);
    gsettings = new QGSettings(id);
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &keys){
            updateTimeText();
    });
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

    QString delaytime=QTime::currentTime().toString();
    QList<QString> pathresult=delaytime.split(":");
    int second=pathresult.at(2).toInt();
    connect(mTimer, &QTimer::timeout, [this]{updateTimeText(); mTimer->stop(); mTimer->start(60*1000);});
    mTimer->start((60 +1 -second)*1000);

    const QByteArray _id(SYSTEM_FONT_SET);
    fgsettings = new QGSettings(_id);
    connect(fgsettings, &QGSettings::changed, this, [=] (const QString &keys){
        if(keys == SYSTEM_FONT_SIZE){
            updateTimeText();
        }
    });

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
    connect(mContent,&CalendarActiveLabel::pressTimeText,[=]{CalendarWidgetShow();});

    initializeCalendar();
    setTimeShowStyle();
    mContent->setWordWrap(true);

    ListenGsettings *m_ListenGsettings = new ListenGsettings();
    QObject::connect(m_ListenGsettings,&ListenGsettings::iconsizechanged,[this]{updateTimeText();});
    QObject::connect(m_ListenGsettings,&ListenGsettings::panelpositionchanged,[this]{updateTimeText();});
    updateTimeText();
    QTimer::singleShot(10000,[this] { updateTimeText();});

    //读取配置文件中CalendarVersion 的值
    QString filename = QDir::homePath() + "/.config/ukui/panel-commission.ini";
    QSettings m_settings(filename, QSettings::IniFormat);
    m_settings.setIniCodec("UTF-8");

    m_settings.beginGroup("Calendar");
    calendar_version = m_settings.value("CalendarVersion", "").toString();
    if (calendar_version.isEmpty()) {
        calendar_version = "old";
    }
    m_settings.endGroup();
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
    gsettings->deleteLater();
    fgsettings->deleteLater();
}

void IndicatorCalendar::updateTimeText()
{
    qDebug()<<"IndicatorCalendar::updateTimeText"<<QDateTime::currentDateTime().toString();
    QDateTime tzNow = QDateTime::currentDateTime();

    QString str;
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

    QString style;
    int font_size = fgsettings->get(SYSTEM_FONT_SIZE).toInt() + mContent->mPlugin->panel()->panelSize() / 23 - 1;
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

void IndicatorCalendar::CalendarWidgetShow()
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
        if(qgetenv("XDG_SESSION_TYPE")=="wayland") mWebViewDiag->setGeometry(calculatePopupWindowPos(QSize(mViewWidht+POPUP_BORDER_SPACING,mViewHeight+POPUP_BORDER_SPACING)));
        else modifyCalendarWidget();
        mWebViewDiag->show();
        mWebViewDiag->activateWindow();
        if(!mbActived)
        {
            mWebViewDiag->setHidden(false);
//            mWebViewDiag->webview()->reload();
            mbActived = true;
        }
        else
        {
            mWebViewDiag->setHidden(true);
//            mWebViewDiag->webview()->reload();
            mbActived = false;
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
    w=new frmLunarCalendarWidget();
    QTimer::singleShot(1000,[this] {setToolTip(tr("Time and Date")); });
}

void CalendarActiveLabel::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button()){
        if(calendar_version == "old"){
            Q_EMIT pressTimeText();
        }else{
            w->setGeometry(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal(event->pos()), w->size()));
            w->show();
        }
    }
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

