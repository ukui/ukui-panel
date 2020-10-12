#include "frmlunarcalendarwidget.h"
#include "ui_frmlunarcalendarwidget.h"
#include <QPainter>

#define TRANSPARENCY_SETTINGS       "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY            "transparency"

frmLunarCalendarWidget::frmLunarCalendarWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmLunarCalendarWidget)
{
    ui->setupUi(this);
    this->initForm();
    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    this->setWindowFlags(Qt::Popup);
    this->setFixedSize(500,550);
    setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明

    ui->labCalendarStyle->setHidden(true);
    ui->cboxCalendarStyle->setHidden(true);
    ui->labSelectType->setHidden(true);
    ui->cboxSelectType->setHidden(true);
    ui->labWeekNameFormat->setHidden(true);
    ui->cboxWeekNameFormat->setHidden(true);
    ui->ckShowLunar->setHidden(true);

    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
        transparency_gsettings = new QGSettings(transparency_id);
        }
    connect(transparency_gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==TRANSPARENCY_KEY){
            this->update();
        }
    });

}

frmLunarCalendarWidget::~frmLunarCalendarWidget()
{
    delete ui;
}

void frmLunarCalendarWidget::initForm()
{
    ui->cboxWeekNameFormat->setCurrentIndex(0);
}

void frmLunarCalendarWidget::on_cboxCalendarStyle_currentIndexChanged(int index)
{
    ui->lunarCalendarWidget->setCalendarStyle((LunarCalendarWidget::CalendarStyle)index);
}

void frmLunarCalendarWidget::on_cboxSelectType_currentIndexChanged(int index)
{
    ui->lunarCalendarWidget->setSelectType((LunarCalendarWidget::SelectType)index);
}

void frmLunarCalendarWidget::on_cboxWeekNameFormat_currentIndexChanged(int index)
{
    ui->lunarCalendarWidget->setWeekNameFormat((LunarCalendarWidget::WeekNameFormat)index);
}

void frmLunarCalendarWidget::on_ckShowLunar_stateChanged(int arg1)
{
    ui->lunarCalendarWidget->setShowLunar(arg1 != 0);
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
