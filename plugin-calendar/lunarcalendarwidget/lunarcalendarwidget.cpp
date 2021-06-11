#pragma execution_character_set("utf-8")

#include "lunarcalendarwidget.h"


#include <QJsonParseError>
#include <QJsonObject>

#define PANEL_CONTROL_IN_CALENDAR "org.ukui.control-center.panel.plugins"
#define LUNAR_KEY                 "calendar"
#define FIRST_DAY_KEY "firstday"

#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"
#define ICON_COLOR_LOGHT      255
#define ICON_COLOR_DRAK       0




LunarCalendarWidget::LunarCalendarWidget(QWidget *parent) : QWidget(parent)
{
    analysisWorktimeJs();
    const QByteArray calendar_id(PANEL_CONTROL_IN_CALENDAR);
    if(QGSettings::isSchemaInstalled(calendar_id)){
        calendar_gsettings = new QGSettings(calendar_id);
        //农历切换监听与日期显示格式
        connect(calendar_gsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key == LUNAR_KEY){
                if(calendar_gsettings->get("calendar").toString() == "lunar") {
                    //农历
                    lunarstate = true;
                    labWidget->setVisible(true);
                    yijiWidget->setVisible(true);
                } else {
                    //公历
                    lunarstate = false;
                    labWidget->setVisible(false);
                    yijiWidget->setVisible(false);
                }
                _timeUpdate();
             }
            if(key == "date") {
                if(calendar_gsettings->get("date").toString() == "cn"){
                    dateShowMode = "yyyy/MM/dd    dddd";
                } else {
                    dateShowMode = "yyyy-MM-dd    dddd";
                }
            }
        });

        if(calendar_gsettings->get("date").toString() == "cn"){
            dateShowMode = "yyyy/MM/dd    dddd";
        } else {
            dateShowMode = "yyyy-MM-dd    dddd";
        }

        //监听12/24小时制
        connect(calendar_gsettings, &QGSettings::changed, this, [=] (const QString &keys){
                timemodel = calendar_gsettings->get("hoursystem").toString();
                _timeUpdate();
        });
        timemodel = calendar_gsettings->get("hoursystem").toString();

    } else {
        dateShowMode = "yyyy/MM/dd    dddd";
        //无设置默认公历
        lunarstate = true;
    }
    setWindowOpacity(0.7);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setProperty("useSystemStyleBlur", true);   //设置毛玻璃效果
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/image/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    btnYear = new QPushButton;
    btnMonth = new QPushButton;
    btnToday = new QPushButton;
    btnClick = false;

    calendarStyle = CalendarStyle_Red;
    date = QDate::currentDate();

    widgetTime = new QWidget;
    timeShow = new QVBoxLayout(widgetTime);

    datelabel =new QLabel(this);
    timelabel = new QLabel(this);
    lunarlabel = new QLabel(this);

    widgetTime->setObjectName("widgetTime");
    timeShow->setContentsMargins(0, 0, 0, 0);
    initWidget();

     if(QGSettings::isSchemaInstalled(calendar_id)){
         //初始化农历/公历显示方式
         if(calendar_gsettings->get("calendar").toString() == "lunar") {
             //农历
             lunarstate = true;
             labWidget->setVisible(true);
             yijiWidget->setVisible(true);
         } else {
             //公历
             lunarstate = false;
             labWidget->setVisible(false);
             yijiWidget->setVisible(false);
         }
     }


    //切换主题
    const QByteArray style_id(ORG_UKUI_STYLE);
    QStringList stylelist;
    stylelist<<STYLE_NAME_KEY_DARK<<STYLE_NAME_KEY_BLACK;
    if(QGSettings::isSchemaInstalled(style_id)){
        style_settings = new QGSettings(style_id);
        dark_style=stylelist.contains(style_settings->get(STYLE_NAME).toString());
        setColor(dark_style);
        }
    connect(style_settings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==STYLE_NAME){
            dark_style=stylelist.contains(style_settings->get(STYLE_NAME).toString());
            _timeUpdate();
            setColor(dark_style);
            QPixmap pixmap1 = QIcon::fromTheme("strIconPath", QIcon::fromTheme("ukui-up-symbolic")).pixmap(QSize(24, 24));
            PictureToWhite pictToWhite1;
            btnPrevYear->setPixmap(pictToWhite1.drawSymbolicColoredPixmap(pixmap1));

            QPixmap pixmap2 = QIcon::fromTheme("strIconPath", QIcon::fromTheme("ukui-down-symbolic")).pixmap(QSize(24, 24));
            PictureToWhite pictToWhite2;
            btnNextYear->setPixmap(pictToWhite2.drawSymbolicColoredPixmap(pixmap2));
        }
    });

    //实时监听系统字体的改变
    const QByteArray id("org.ukui.style");
    QGSettings * fontSetting = new QGSettings(id, QByteArray(), this);
    connect(fontSetting, &QGSettings::changed,[=](QString key) {
        if ("systemFont" == key || "systemFontSize" ==key) {
            QFont font = this->font();
            btnToday->setFont(font);
            cboxYearandMonth->setFont(font);
            for (int i = 0; i < 42; i++) {
                dayItems.value(i)->setFont(font);
                dayItems.value(i)->repaint();
            }
            for (int i = 0; i < 7; i++) {
                labWeeks.value(i)->setFont(font);
                labWeeks.value(i)->repaint();
            }
        }
    });

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);

    if(QGSettings::isSchemaInstalled(calendar_id)){
        setWeekNameFormat(calendar_gsettings->get(FIRST_DAY_KEY).toString() == "sunday");
        setShowLunar(calendar_gsettings->get(LUNAR_KEY).toString() == "lunar");
    }
}

LunarCalendarWidget::~LunarCalendarWidget()
{
}

/*
 * @brief 设置日历的背景及文字颜色
 * 参数：
 * weekColor 周六及周日文字颜色
*/

void LunarCalendarWidget::setColor(bool mdark_style)
{
    const QByteArray calendar_id(PANEL_CONTROL_IN_CALENDAR);
    if(mdark_style){
        weekTextColor = QColor(0, 0, 0);
        weekBgColor = QColor(180, 180, 180);

        if(QGSettings::isSchemaInstalled(calendar_id)){
            showLunar = calendar_gsettings->get(LUNAR_KEY).toString() == "lunar";
        }

        bgImage = ":/image/bg_calendar.png";
        selectType = SelectType_Rect;

        borderColor = QColor(180, 180, 180);
        weekColor = QColor(255, 255, 255);
        superColor = QColor(255, 129, 6);
        lunarColor = QColor(233, 90, 84);

        currentTextColor = QColor(255, 255, 255);
        otherTextColor = QColor(125, 125, 125);
        selectTextColor = QColor(255, 255, 255);
        hoverTextColor = QColor(0, 0, 0);

        currentLunarColor = QColor(150, 150, 150);
        otherLunarColor = QColor(200, 200, 200);
        selectLunarColor = QColor(255, 255, 255);
        hoverLunarColor = QColor(250, 250, 250);

        currentBgColor = QColor(0, 0, 0);
        otherBgColor = QColor(240, 240, 240);
        selectBgColor = QColor(80, 100, 220);
        hoverBgColor = QColor(80, 190, 220);
    }else{
        weekTextColor = QColor(255, 255, 255);
        weekBgColor = QColor(0, 0, 0);

        if(QGSettings::isSchemaInstalled(calendar_id)){
            showLunar = calendar_gsettings->get(LUNAR_KEY).toString() == "lunar";
        }

        bgImage = ":/image/bg_calendar.png";
        selectType = SelectType_Rect;

        borderColor = QColor(180, 180, 180);
        weekColor = QColor(0, 0, 0);
        superColor = QColor(255, 129, 6);
        lunarColor = QColor(233, 90, 84);

        currentTextColor = QColor(0, 0, 0);
        otherTextColor = QColor(125, 125, 125);
        selectTextColor = QColor(255, 255, 255);
        hoverTextColor = QColor(0, 0, 0);

        currentLunarColor = QColor(150, 150, 150);
        otherLunarColor = QColor(200, 200, 200);
        selectLunarColor = QColor(255, 255, 255);
        hoverLunarColor = QColor(250, 250, 250);

        currentBgColor = QColor(250, 250, 250);
        otherBgColor = QColor(240, 240, 240);
        selectBgColor = QColor(80, 100, 220);
        hoverBgColor = QColor(80, 190, 220);
    }
       initStyle();
}

void LunarCalendarWidget::_timeUpdate() {
    QDateTime time = QDateTime::currentDateTime();
    QLocale locale = (QLocale::system().name() == "zh_CN" ? (QLocale::Chinese) : (QLocale::English));
    QString _time;
    if(timemodel == "12") {
        _time = locale.toString(time,"Ahh:mm:ss");
    } else {
        _time = locale.toString(time,"hh:mm:ss");
    }

    QFont font;
    datelabel->setText(_time);
    font.setPointSize(22);
    datelabel->setFont(font);
    datelabel->setAlignment(Qt::AlignHCenter);

    QString strHoliday;
    QString strSolarTerms;
    QString strLunarFestival;
    QString strLunarYear;
    QString strLunarMonth;
    QString strLunarDay;

    LunarCalendarInfo::Instance()->getLunarCalendarInfo(locale.toString(time,"yyyy").toInt(),
                                                        locale.toString(time,"MM").toInt(),
                                                        locale.toString(time,"dd").toInt(),
                                                        strHoliday,
                                                        strSolarTerms,
                                                        strLunarFestival,
                                                        strLunarYear,
                                                        strLunarMonth,
                                                        strLunarDay);

    QString _date = locale.toString(time,dateShowMode);
    if (lunarstate) {
        _date = _date + "    "+strLunarMonth + strLunarDay;
    }
    timelabel->setText(_date);
    font.setPointSize(12);
    timelabel->setFont(font);
    timelabel->setAlignment(Qt::AlignHCenter);


}

void LunarCalendarWidget::timerUpdate()
{
    _timeUpdate();
}

void LunarCalendarWidget::initWidget()
{
    setObjectName("lunarCalendarWidget");

    //顶部widget
    QWidget *widgetTop = new QWidget;
    widgetTop->setObjectName("widgetTop");
    widgetTop->setMinimumHeight(35);

    //上个月的按钮
    btnPrevYear = new statelabel;
    btnPrevYear->setObjectName("btnPrevYear");
    btnPrevYear->setFixedWidth(35);
    btnPrevYear->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QPixmap pixmap1 = QIcon::fromTheme("strIconPath", QIcon::fromTheme("ukui-up-symbolic")).pixmap(QSize(24, 24));
    PictureToWhite pictToWhite1;
    btnPrevYear->setPixmap(pictToWhite1.drawSymbolicColoredPixmap(pixmap1));


    //下个月按钮
    btnNextYear = new statelabel;
    btnNextYear->setObjectName("btnNextYear");
    btnNextYear->setFixedWidth(35);
    btnNextYear->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QPixmap pixmap2 = QIcon::fromTheme("strIconPath", QIcon::fromTheme("ukui-down-symbolic")).pixmap(QSize(24, 24));
    PictureToWhite pictToWhite2;
    btnNextYear->setPixmap(pictToWhite2.drawSymbolicColoredPixmap(pixmap2));


    //转到年显示 暂不用此
    btnYear->setObjectName("btnYear");
    btnYear->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnYear->setText(tr("Year"));

    //转到月显示 暂不用此
    btnMonth->setObjectName("btnMonth");
    btnMonth->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnMonth->setText(tr("Month"));

    //转到今天
    btnToday->setObjectName("btnToday");
    btnToday->setFixedWidth(40);
    btnToday->setStyle(new CustomStyle_pushbutton("ukui-default"));
    btnToday->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    btnToday->setText(tr("今天"));

    //年份与月份下拉框 暂不用此
    cboxYearandMonth = new QComboBox;
    cboxYearandMonth->setStyleSheet("QComboBox{background: transparent;font-size: 20px;}"
                                    "QComboBox::down-down{width: 1px;}");
    cboxYearandMonth->setFixedWidth(100);
    cboxYearandMonth->setObjectName("cboxYearandMonth");
    for (int i = 1901; i <= 2099; i++) {
        for (int j = 1; j <= 12; j++) {
            cboxYearandMonth->addItem(QString("%1.%2").arg(i).arg(j));
        }
    }

    cboxYearandMonthLabel = new QLabel();
    cboxYearandMonthLabel->setFixedWidth(100);

    //中间用个空widget隔开
    QWidget *widgetBlank1 = new QWidget;
    widgetBlank1->setFixedWidth(180);
    QWidget *widgetBlank2 = new QWidget;
    widgetBlank2->setFixedWidth(5);

    //顶部横向布局
    QHBoxLayout *layoutTop = new QHBoxLayout(widgetTop);
    layoutTop->setContentsMargins(0, 0, 0, 9);
    layoutTop->addItem(new QSpacerItem(5,1));
//    layoutTop->addWidget(cboxYearandMonth);
    layoutTop->addWidget(cboxYearandMonthLabel);
    layoutTop->addWidget(btnNextYear);
    layoutTop->addWidget(btnPrevYear);


    layoutTop->addWidget(widgetBlank1);
    layoutTop->addWidget(widgetBlank2);
//    layoutTop->addWidget(btnYear);
//    layoutTop->addWidget(btnMonth);
    layoutTop->addWidget(btnToday);
    layoutTop->addItem(new QSpacerItem(10,1));

    //时间
    widgetTime->setMinimumHeight(50);
    timeShow->addWidget(datelabel);//, Qt::AlignHCenter);
    timeShow->addWidget(timelabel);//,Qt::AlignHCenter);


    //星期widget
    QWidget *widgetWeek = new QWidget;
    widgetWeek->setObjectName("widgetWeek");
    widgetWeek->setMinimumHeight(30);

    //星期布局
    QHBoxLayout *layoutWeek = new QHBoxLayout(widgetWeek);
    layoutWeek->setMargin(0);
    layoutWeek->setSpacing(0);

    for (int i = 0; i < 7; i++) {
        QLabel *lab = new QLabel;
        lab->setAlignment(Qt::AlignCenter);
        layoutWeek->addWidget(lab);
        labWeeks.append(lab);
    }

    //日期标签widget
    QWidget *widgetBody = new QWidget;
    widgetBody->setObjectName("widgetBody");

    //日期标签布局
    QGridLayout *layoutBody = new QGridLayout(widgetBody);
    layoutBody->setMargin(1);
    layoutBody->setHorizontalSpacing(0);
    layoutBody->setVerticalSpacing(0);

    //逐个添加日标签
    for (int i = 0; i < 42; i++) {
        LunarCalendarItem *lab = new LunarCalendarItem;
        lab->worktime = worktime;
        connect(lab, SIGNAL(clicked(QDate, LunarCalendarItem::DayType)), this, SLOT(clicked(QDate, LunarCalendarItem::DayType)));
        layoutBody->addWidget(lab, i / 7, i % 7);
        dayItems.append(lab);
    }

    //
    labWidget = new QWidget();
    labBottom = new QLabel();
    yijichooseLabel = new QLabel();
    yijichooseLabel->setText("宜忌");

    QFont font;
    font.setPointSize(12);
    labBottom->setFont(font);
    yijichoose = new QCheckBox();

    labLayout = new QHBoxLayout();
    labLayout->addWidget(labBottom);
    labLayout->addItem(new QSpacerItem(100,5,QSizePolicy::Expanding,QSizePolicy::Minimum));
    labLayout->addWidget(yijichooseLabel);
    labLayout->addWidget(yijichoose);
    labWidget->setLayout(labLayout);

    yijiLayout = new QVBoxLayout;
    yijiWidget = new QWidget;
    yijiWidget->setFixedHeight(60);

    yiLabel = new QLabel();
    jiLabel = new QLabel();

    yijiLayout->addWidget(yiLabel);
    yijiLayout->addWidget(jiLabel);
    yijiWidget->setLayout(yijiLayout);
    yiLabel->setVisible(false);
    jiLabel->setVisible(false);

    connect(yijichoose,&QRadioButton::clicked,this,&LunarCalendarWidget::customButtonsClicked);

    //主布局
    lineUp = new m_PartLineWidget();
    lineDown = new m_PartLineWidget();

    lineUp->setFixedSize(440, 1);
    lineDown->setFixedSize(440, 1);
    QVBoxLayout *verLayoutCalendar = new QVBoxLayout(this);
    verLayoutCalendar->setMargin(0);
    verLayoutCalendar->setSpacing(0);
    verLayoutCalendar->addWidget(widgetTime);
    verLayoutCalendar->addItem(new QSpacerItem(10,10));
    verLayoutCalendar->addWidget(lineUp);
    verLayoutCalendar->addItem(new QSpacerItem(10,10));
    verLayoutCalendar->addWidget(widgetTop);
    verLayoutCalendar->addWidget(widgetWeek);
    verLayoutCalendar->addWidget(widgetBody, 1);
    verLayoutCalendar->addWidget(lineDown);
    verLayoutCalendar->addWidget(labWidget);
    verLayoutCalendar->addWidget(yijiWidget);


    //绑定按钮和下拉框信号
//    connect(btnPrevYear, SIGNAL(clicked(bool)), this, SLOT(showPreviousYear()));
//    connect(btnNextYear, SIGNAL(clicked(bool)), this, SLOT(showNextYear()));
    connect(btnPrevYear, SIGNAL(labelclick()), this, SLOT(showPreviousMonth()));
    connect(btnNextYear, SIGNAL(labelclick()), this, SLOT(showNextMonth()));
    connect(btnToday, SIGNAL(clicked(bool)), this, SLOT(showToday()));
    connect(cboxYearandMonth, SIGNAL(currentIndexChanged(QString)), this, SLOT(yearChanged(QString)));
//    connect(cboxMonth, SIGNAL(currentIndexChanged(QString)), this, SLOT(monthChanged(QString)));
}

void LunarCalendarWidget::initStyle()
{
    //设置样式
    QStringList qss;

    //自定义日控件颜色
    QString strSelectType;
    if (selectType == SelectType_Rect) {
        strSelectType = "SelectType_Rect";
    } else if (selectType == SelectType_Circle) {
        strSelectType = "SelectType_Circle";
    } else if (selectType == SelectType_Triangle) {
        strSelectType = "SelectType_Triangle";
    } else if (selectType == SelectType_Image) {
        strSelectType = "SelectType_Image";
    }

    //计划去掉qss,保留农历切换的设置
    qss.append(QString("LunarCalendarItem{qproperty-showLunar:%1;}").arg(showLunar));

    this->setStyleSheet(qss.join(""));
}

void LunarCalendarWidget::analysisWorktimeJs()
{
    /*解析json文件*/
    QFile file("/usr/share/ukui-panel/plugin-calendar/html/jiejiari.js");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug()<<tr("解析json文件错误！");
        return;
    }
    QJsonObject jsonObject = document.object();
    QStringList args = jsonObject.keys();

    for (int i=0;i<args.count();i++) {
        if(jsonObject.contains(args.at(i)))
        {
            QJsonValue jsonValueList = jsonObject.value(args.at(i));
            QJsonObject item = jsonValueList.toObject();
            QStringList arg2 = item.keys();
            for (int j=0;j<arg2.count();j++) {
                worktimeinside.insert(arg2.at(j),item[arg2.at(j)].toString());
            }
        }
        worktime.insert(args.at(i),worktimeinside);
        worktimeinside.clear();
    }
}


//初始化日期面板
void LunarCalendarWidget::initDate()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if(oneRun) {
        downLabelHandle(date);
        yijihandle(date);
        oneRun = false;
    }


    //设置为今天,设置变量防止重复触发
    btnClick = true;
    cboxYearandMonth->setCurrentIndex(cboxYearandMonth->findText(QString("%1.%2").arg(year).arg(month)));
    btnClick = false;

    cboxYearandMonthLabel->setText(QString("   %1.%2").arg(year).arg(month));

    //首先判断当前月的第一天是星期几
    int week = LunarCalendarInfo::Instance()->getFirstDayOfWeek(year, month, FirstdayisSun);
    //当前月天数
    int countDay = LunarCalendarInfo::Instance()->getMonthDays(year, month);
    //上月天数
    int countDayPre = LunarCalendarInfo::Instance()->getMonthDays(1 == month ? year - 1 : year, 1 == month ? 12 : month - 1);

    //如果上月天数上月刚好一周则另外处理
    int startPre, endPre, startNext, endNext, index, tempYear, tempMonth, tempDay;
    if (0 == week) {
        startPre = 0;
        endPre = 7;
        startNext = 0;
        endNext = 42 - (countDay + 7);
    } else {
        startPre = 0;
        endPre = week;
        startNext = week + countDay;
        endNext = 42;
    }

    //纠正1月份前面部分偏差,1月份前面部分是上一年12月份
    tempYear = year;
    tempMonth = month - 1;
    if (tempMonth < 1) {
        tempYear--;
        tempMonth = 12;
    }

    //显示上月天数
    for (int i = startPre; i < endPre; i++) {
        index = i;
        tempDay = countDayPre - endPre + i + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        dayItems.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthPre);
    }

    //纠正12月份后面部分偏差,12月份后面部分是下一年1月份
    tempYear = year;
    tempMonth = month + 1;
    if (tempMonth > 12) {
        tempYear++;
        tempMonth = 1;
    }

    //显示下月天数
    for (int i = startNext; i < endNext; i++) {
        index = 42 - endNext + i;
        tempDay = i - startNext + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        dayItems.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthNext);
    }

    //重新置为当前年月
    tempYear = year;
    tempMonth = month;

    //显示当前月
    for (int i = week; i < (countDay + week); i++) {
        index = (0 == week ? (i + 7) : i);
        tempDay = i - week + 1;

        QDate date(tempYear, tempMonth, tempDay);
        QString lunar = LunarCalendarInfo::Instance()->getLunarDay(tempYear, tempMonth, tempDay);
        if (0 == (i % 7) || 6 == (i % 7)) {
            dayItems.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_WeekEnd);
        } else {
            dayItems.at(index)->setDate(date, lunar, LunarCalendarItem::DayType_MonthCurrent);
        }
    }

}

void LunarCalendarWidget::customButtonsClicked(int x)
{
    if (x) {
        yiLabel->setVisible(true);
        jiLabel->setVisible(true);
        yijistate = true;
    } else {
        yiLabel->setVisible(false);
        jiLabel->setVisible(false);
        yijistate = false;
    }
}

QString LunarCalendarWidget::getSettings()
{
    QString arg = "配置文件";
    return  arg;

}

void LunarCalendarWidget::setSettings(QString arg)
{

}

void LunarCalendarWidget::downLabelHandle(const QDate &date)
{

    QString strHoliday;
    QString strSolarTerms;
    QString strLunarFestival;
    QString strLunarYear;
    QString strLunarMonth;
    QString strLunarDay;

    LunarCalendarInfo::Instance()->getLunarCalendarInfo(date.year(),
                                                        date.month(),
                                                        date.day(),
                                                        strHoliday,
                                                        strSolarTerms,
                                                        strLunarFestival,
                                                        strLunarYear,
                                                        strLunarMonth,
                                                        strLunarDay);

    QString labBottomarg =  "     " + strLunarYear + "  " + strLunarMonth + strLunarDay;
    labBottom->setText(labBottomarg);

}

void LunarCalendarWidget::yijihandle(const QDate &date)
{
    /*解析json文件*/
    QFile file(QString("/usr/share/ukui-panel/plugin-calendar/html/hlnew/hl%1.js").arg(date.year()));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug()<<tr("解析json文件错误！");
        return;
    }
    QJsonObject jsonObject = document.object();

    if(jsonObject.contains(QString("d%1").arg(date.toString("MMdd"))))
    {
        QJsonValue jsonValueList = jsonObject.value(QString("d%1").arg(date.toString("MMdd")));
        QJsonObject item = jsonValueList.toObject();
        QString yiString = "     宜：" + item["y"].toString();
        QString jiString = "     忌：" + item["j"].toString();


        yiLabel->setText(yiString);
        jiLabel->setText(jiString);
    }
}

void LunarCalendarWidget::yearChanged(const QString &arg1)
{
    //如果是单击按钮切换的日期变动则不需要触发
    if (btnClick) {
        return;
    }
    int nIndex = arg1.indexOf(".");
    if(-1 == nIndex){
        return;
    }
    int year = arg1.mid(0,nIndex).toInt();
    int month = arg1.mid(nIndex + 1).toInt();
    int day = date.day();
    dateChanged(year, month, day);
}

void LunarCalendarWidget::monthChanged(const QString &arg1)
{
    //如果是单击按钮切换的日期变动则不需要触发
    if (btnClick) {
        return;
    }

    int year = date.year();
    int month = arg1.mid(0, arg1.length()).toInt();
    int day = date.day();
    dateChanged(year, month, day);
}

void LunarCalendarWidget::clicked(const QDate &date, const LunarCalendarItem::DayType &dayType)
{
    this->date = date;
    dayChanged(this->date);
    if (LunarCalendarItem::DayType_MonthPre == dayType)
        showPreviousMonth(false);
    else if (LunarCalendarItem::DayType_MonthNext == dayType)
        showNextMonth(false);
}

void LunarCalendarWidget::dayChanged(const QDate &date)
{
    //计算星期几,当前天对应标签索引=日期+星期几-1
    int year = date.year();
    int month = date.month();
    int day = date.day();
    int week = LunarCalendarInfo::Instance()->getFirstDayOfWeek(year, month, FirstdayisSun);
    //选中当前日期,其他日期恢复,这里还有优化空间,比方说类似单选框机制
    for (int i = 0; i < 42; i++) {
        //当月第一天是星期天要另外计算
        int index = day + week - 1;
        if (week == 0) {
            index = day + 6;
        }
        dayItems.at(i)->setSelect(i == index);
        if (i == index) {
            downLabelHandle(dayItems.at(i)->getDate());
            yijihandle(dayItems.at(i)->getDate());
        }

    }


    //发送日期单击信号
    Q_EMIT clicked(date);
    //发送日期更新信号
    Q_EMIT selectionChanged();
}

void LunarCalendarWidget::dateChanged(int year, int month, int day)
{
    //如果原有天大于28则设置为1,防止出错
    date.setDate(year, month, day > 28 ? 1 : day);
    initDate();
}

LunarCalendarWidget::CalendarStyle LunarCalendarWidget::getCalendarStyle() const
{
    return this->calendarStyle;
}

QDate LunarCalendarWidget::getDate() const
{
    return this->date;
}

QColor LunarCalendarWidget::getWeekTextColor() const
{
    return this->weekTextColor;
}

QColor LunarCalendarWidget::getWeekBgColor() const
{
    return this->weekBgColor;
}

bool LunarCalendarWidget::getShowLunar() const
{
    return this->showLunar;
}

QString LunarCalendarWidget::getBgImage() const
{
    return this->bgImage;
}

LunarCalendarWidget::SelectType LunarCalendarWidget::getSelectType() const
{
    return this->selectType;
}

QColor LunarCalendarWidget::getBorderColor() const
{
    return this->borderColor;
}

QColor LunarCalendarWidget::getWeekColor() const
{
    return this->weekColor;
}

QColor LunarCalendarWidget::getSuperColor() const
{
    return this->superColor;
}

QColor LunarCalendarWidget::getLunarColor() const
{
    return this->lunarColor;
}

QColor LunarCalendarWidget::getCurrentTextColor() const
{
    return this->currentTextColor;
}

QColor LunarCalendarWidget::getOtherTextColor() const
{
    return this->otherTextColor;
}

QColor LunarCalendarWidget::getSelectTextColor() const
{
    return this->selectTextColor;
}

QColor LunarCalendarWidget::getHoverTextColor() const
{
    return this->hoverTextColor;
}

QColor LunarCalendarWidget::getCurrentLunarColor() const
{
    return this->currentLunarColor;
}

QColor LunarCalendarWidget::getOtherLunarColor() const
{
    return this->otherLunarColor;
}

QColor LunarCalendarWidget::getSelectLunarColor() const
{
    return this->selectLunarColor;
}

QColor LunarCalendarWidget::getHoverLunarColor() const
{
    return this->hoverLunarColor;
}

QColor LunarCalendarWidget::getCurrentBgColor() const
{
    return this->currentBgColor;
}

QColor LunarCalendarWidget::getOtherBgColor() const
{
    return this->otherBgColor;
}

QColor LunarCalendarWidget::getSelectBgColor() const
{
    return this->selectBgColor;
}

QColor LunarCalendarWidget::getHoverBgColor() const
{
    return this->hoverBgColor;
}

QSize LunarCalendarWidget::sizeHint() const
{
    return QSize(600, 500);
}

QSize LunarCalendarWidget::minimumSizeHint() const
{
    return QSize(200, 150);
}

//显示上一年
void LunarCalendarWidget::showPreviousYear()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year <= 1901) {
        return;
    }

    year--;
    dateChanged(year, month, day);
}

//显示下一年
void LunarCalendarWidget::showNextYear()
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year >= 2099) {
        return;
    }

    year++;
    dateChanged(year, month, day);
}

//显示上月日期
void LunarCalendarWidget::showPreviousMonth(bool date_clicked)
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year <= 1901 && month == 1) {
        return;
    }

    //extra:
    if (date_clicked) month--;
    if (month < 1) {
               month = 12;
               year--;
    }

    dateChanged(year, month, day);
}

//显示下月日期
void LunarCalendarWidget::showNextMonth(bool date_clicked)
{
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (year >= 2099 ) {
        return;
    }

    //extra
    if (date_clicked)month++;
    if (month > 12) {
               month = 1;
               year++;
    }

    dateChanged(year, month, day);
}

//转到今天
void LunarCalendarWidget::showToday()
{
    date = QDate::currentDate();
    initDate();
    dayChanged(date);
}

void LunarCalendarWidget::setCalendarStyle(const LunarCalendarWidget::CalendarStyle &calendarStyle)
{
    if (this->calendarStyle != calendarStyle) {
        this->calendarStyle = calendarStyle;
    }
}

void LunarCalendarWidget::setWeekNameFormat(bool FirstDayisSun)
{
    FirstdayisSun = FirstDayisSun;
    if (FirstdayisSun) {
//        listWeek << "日" << "一" << "二" << "三" << "四" << "五" << "六";
//        listWeek << "周日" << "周一" << "周二" << "周三" << "周四" << "周五" << "周六";
//        listWeek << "星期天" << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六";
//          listWeek << "Sun" << "Mon" << "Tue" << "Wed" << "Thur" << "Fri" << "Sat";
          labWeeks.at(0)->setText((tr("周日")));
          labWeeks.at(1)->setText((tr("周一")));
          labWeeks.at(2)->setText((tr("周二")));
          labWeeks.at(3)->setText((tr("周三")));
          labWeeks.at(4)->setText((tr("周四")));
          labWeeks.at(5)->setText((tr("周五")));
          labWeeks.at(6)->setText((tr("周六")));
    } else {
        labWeeks.at(0)->setText((tr("周一")));
        labWeeks.at(1)->setText((tr("周二")));
        labWeeks.at(2)->setText((tr("周三")));
        labWeeks.at(3)->setText((tr("周四")));
        labWeeks.at(4)->setText((tr("周五")));
        labWeeks.at(5)->setText((tr("周六")));
        labWeeks.at(6)->setText((tr("周日")));
    }
        initDate();
}

void LunarCalendarWidget::setDate(const QDate &date)
{
    if (this->date != date) {
        this->date = date;
        initDate();
    }
}

void LunarCalendarWidget::setWeekTextColor(const QColor &weekTextColor)
{
    if (this->weekTextColor != weekTextColor) {
        this->weekTextColor = weekTextColor;
        initStyle();
    }
}

void LunarCalendarWidget::setWeekBgColor(const QColor &weekBgColor)
{
    if (this->weekBgColor != weekBgColor) {
        this->weekBgColor = weekBgColor;
        initStyle();
    }
}

void LunarCalendarWidget::setShowLunar(bool showLunar)
{
        this->showLunar = showLunar;
        initStyle();
}

void LunarCalendarWidget::setBgImage(const QString &bgImage)
{
    if (this->bgImage != bgImage) {
        this->bgImage = bgImage;
        initStyle();
    }
}

void LunarCalendarWidget::setSelectType(const LunarCalendarWidget::SelectType &selectType)
{
    if (this->selectType != selectType) {
        this->selectType = selectType;
        initStyle();
    }
}

void LunarCalendarWidget::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        initStyle();
    }
}

void LunarCalendarWidget::setWeekColor(const QColor &weekColor)
{
    if (this->weekColor != weekColor) {
        this->weekColor = weekColor;
        initStyle();
    }
}

void LunarCalendarWidget::setSuperColor(const QColor &superColor)
{
    if (this->superColor != superColor) {
        this->superColor = superColor;
        initStyle();
    }
}

void LunarCalendarWidget::setLunarColor(const QColor &lunarColor)
{
    if (this->lunarColor != lunarColor) {
        this->lunarColor = lunarColor;
        initStyle();
    }
}

void LunarCalendarWidget::setCurrentTextColor(const QColor &currentTextColor)
{
    if (this->currentTextColor != currentTextColor) {
        this->currentTextColor = currentTextColor;
        initStyle();
    }
}

void LunarCalendarWidget::setOtherTextColor(const QColor &otherTextColor)
{
    if (this->otherTextColor != otherTextColor) {
        this->otherTextColor = otherTextColor;
        initStyle();
    }
}

void LunarCalendarWidget::setSelectTextColor(const QColor &selectTextColor)
{
    if (this->selectTextColor != selectTextColor) {
        this->selectTextColor = selectTextColor;
        initStyle();
    }
}

void LunarCalendarWidget::setHoverTextColor(const QColor &hoverTextColor)
{
    if (this->hoverTextColor != hoverTextColor) {
        this->hoverTextColor = hoverTextColor;
        initStyle();
    }
}

void LunarCalendarWidget::setCurrentLunarColor(const QColor &currentLunarColor)
{
    if (this->currentLunarColor != currentLunarColor) {
        this->currentLunarColor = currentLunarColor;
        initStyle();
    }
}

void LunarCalendarWidget::setOtherLunarColor(const QColor &otherLunarColor)
{
    if (this->otherLunarColor != otherLunarColor) {
        this->otherLunarColor = otherLunarColor;
        initStyle();
    }
}

void LunarCalendarWidget::setSelectLunarColor(const QColor &selectLunarColor)
{
    if (this->selectLunarColor != selectLunarColor) {
        this->selectLunarColor = selectLunarColor;
        initStyle();
    }
}

void LunarCalendarWidget::setHoverLunarColor(const QColor &hoverLunarColor)
{
    if (this->hoverLunarColor != hoverLunarColor) {
        this->hoverLunarColor = hoverLunarColor;
        initStyle();
    }
}

void LunarCalendarWidget::setCurrentBgColor(const QColor &currentBgColor)
{
    if (this->currentBgColor != currentBgColor) {
        this->currentBgColor = currentBgColor;
        initStyle();
    }
}

void LunarCalendarWidget::setOtherBgColor(const QColor &otherBgColor)
{
    if (this->otherBgColor != otherBgColor) {
        this->otherBgColor = otherBgColor;
        initStyle();
    }
}

void LunarCalendarWidget::setSelectBgColor(const QColor &selectBgColor)
{
    if (this->selectBgColor != selectBgColor) {
        this->selectBgColor = selectBgColor;
        initStyle();
    }
}

void LunarCalendarWidget::setHoverBgColor(const QColor &hoverBgColor)
{
    if (this->hoverBgColor != hoverBgColor) {
        this->hoverBgColor = hoverBgColor;
        initStyle();
    }
}

void LunarCalendarWidget::wheelEvent(QWheelEvent *event) {
    if (event->delta() > 0)
        showPreviousMonth();
    else
        showNextMonth();
}


m_PartLineWidget::m_PartLineWidget(QWidget *parent) : QWidget(parent)
{

}

void m_PartLineWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QRect rect = this->rect();

    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    QColor color=qApp->palette().color(QPalette::Base);
    if(color.red() == 255 && color.green() == 255 && color.blue() == 255){
        color.setRgb(1,1,1,255);
    } else if (color.red() == 31 && color.green() == 32 && color.blue() == 34) {
        color.setRgb(255,255,255,255);
    }
    p.setBrush(color);
    p.setOpacity(0.05);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,0,0);

    QWidget::paintEvent(event);
}

statelabel::statelabel() : QLabel()
{


}

//鼠标点击事件
void statelabel::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton){
        Q_EMIT labelclick();

    }
    return;
}

