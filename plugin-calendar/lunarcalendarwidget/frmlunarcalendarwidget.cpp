#include "frmlunarcalendarwidget.h"
#include "ui_frmlunarcalendarwidget.h"
#include <QPainter>

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"
#define PANEL_CONTROL_IN_CALENDAR "org.ukui.control-center.panel.plugins"
#define LUNAR_KEY "calendar"
#define FIRST_DAY_KEY "firstday"

frmLunarCalendarWidget::frmLunarCalendarWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmLunarCalendarWidget)
{
    ui->setupUi(this);
    this->initForm();
    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    this->setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    setProperty("useSystemStyleBlur", true);

    this->setFixedSize(440, 600);

    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
        transparency_gsettings = new QGSettings(transparency_id);
    }
    const QByteArray calendar_id(PANEL_CONTROL_IN_CALENDAR);
    if(QGSettings::isSchemaInstalled(calendar_id)){
        calendar_gsettings = new QGSettings(calendar_id);
    }

    //公历/农历切换
    connect(calendar_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key == LUNAR_KEY){
            ckShowLunar_stateChanged(calendar_gsettings->get(LUNAR_KEY).toString() == "lunar");
        }
        if (key == FIRST_DAY_KEY) {
            cboxWeekNameFormat_currentIndexChanged(calendar_gsettings->get(FIRST_DAY_KEY).toString() == "sunday");
        }
    });

}

frmLunarCalendarWidget::~frmLunarCalendarWidget()
{
    delete ui;
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
    QPainter p(this);
    p.setPen(Qt::gray);
    double tran=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    QColor color = palette().color(QPalette::Base);
    color.setAlpha(tran);
    QBrush brush =QBrush(color);
    p.setBrush(brush);

    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
