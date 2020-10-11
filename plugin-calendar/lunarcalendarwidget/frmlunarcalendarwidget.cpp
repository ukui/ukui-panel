#include "frmlunarcalendarwidget.h"
#include "ui_frmlunarcalendarwidget.h"

frmLunarCalendarWidget::frmLunarCalendarWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmLunarCalendarWidget)
{
    ui->setupUi(this);
    this->initForm();
    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    this->setFixedSize(500,550);

    ui->labCalendarStyle->setHidden(true);
    ui->cboxCalendarStyle->setHidden(true);
    ui->labSelectType->setHidden(true);
    ui->cboxSelectType->setHidden(true);
    ui->labWeekNameFormat->setHidden(true);
    ui->cboxWeekNameFormat->setHidden(true);
    ui->ckShowLunar->setHidden(true);

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
