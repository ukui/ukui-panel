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
    installEventFilter(this);
    ui->setupUi(this);
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeUp,this,&frmLunarCalendarWidget::changeUpSize);
    connect(ui->lunarCalendarWidget,&LunarCalendarWidget::yijiChangeDown,this,&frmLunarCalendarWidget::changeDownSize);
    this->initForm();
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);//去掉标题栏
//    this->setWindowFlags(Qt::Popup);
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
        //公历/农历切换
        connect(calendar_gsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key == LUNAR_KEY){
                ckShowLunar_stateChanged(calendar_gsettings->get(LUNAR_KEY).toString() == "lunar");
            }
            if (key == FIRST_DAY_KEY) {
                cboxWeekNameFormat_currentIndexChanged(calendar_gsettings->get(FIRST_DAY_KEY).toString() == "sunday");
            }
        });
    } else {
        ckShowLunar_stateChanged(false);
        cboxWeekNameFormat_currentIndexChanged(false);
    }
}

frmLunarCalendarWidget::~frmLunarCalendarWidget()
{
    delete ui;
}

void frmLunarCalendarWidget::changeUpSize()
{
    this->setFixedSize(440, 652);
    Q_EMIT yijiChangeUp();
}

void frmLunarCalendarWidget::changeDownSize()
{
    this->setFixedSize(440, 600);
    Q_EMIT yijiChangeDown();
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
    QRect rect = this->rect();
    QPainter p(this);
    double tran =1;
    const QByteArray transparency_id(TRANSPARENCY_SETTINGS);
    if(QGSettings::isSchemaInstalled(transparency_id)){
       tran=transparency_gsettings->get(TRANSPARENCY_KEY).toDouble()*255;
    }

    QColor color = palette().color(QPalette::Base);
    color.setAlpha(tran);
    QBrush brush =QBrush(color);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*
 * 事件过滤，检测鼠标点击外部活动区域则收回收纳栏
*/
bool frmLunarCalendarWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this)
    {
        if (event->type() == QEvent::MouseButtonPress)
           {
               QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
               if (mouseEvent->button() == Qt::LeftButton)
               {
//                   this->hide();
//                   status=ST_HIDE;
                   return true;
               }
               else if(mouseEvent->button() == Qt::RightButton)
               {
                   return true;
               }
           }
        else if(event->type() == QEvent::ContextMenu)
        {
            return false;
        }
        else if (event->type() == QEvent::WindowDeactivate)
        {
            //qDebug()<<"激活外部窗口";
            this->hide();
            return true;
        } else if (event->type() == QEvent::StyleChange) {
        }
    }

    if (!isActiveWindow())
    {
        activateWindow();
    }
    return false;
}
