//Qt
#include <QStyleOption>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>

#include "storagearrow.h"
#include "../panel/customstyle.h"
#include "../panel/iukuipanel.h"
#define PANEL_SETTINGS      "org.ukui.panel.settings"
#define PANEL_POSITION_KEY  "panelposition"
#define ICON_SIZE_KEY       "iconsize"

storageArrowStatus state=NORMAL;
StorageArrow::StorageArrow(QWidget* parent):
    QPushButton(parent)
{
    setStyle(new CustomStyle());
//    setVisible(false);

    const QByteArray id(PANEL_SETTINGS);
    gsetting = new QGSettings(id);
    panelPosition = gsetting->get(PANEL_POSITION_KEY).toInt();
    iconsize=gsetting->get(ICON_SIZE_KEY).toInt();
    connect(gsetting, &QGSettings::changed, this, [=] (const QString &key){
        if(key == PANEL_POSITION_KEY){
            panelPosition=gsetting->get(PANEL_POSITION_KEY).toInt();
            setArrowIcon();
        }
        if(key == ICON_SIZE_KEY){
            iconsize=gsetting->get(ICON_SIZE_KEY).toInt();
            setArrowIcon();
        }
    });

}

StorageArrow::~StorageArrow() { }

//void StorageArrow::paintEvent(QEvent *e)
//{
//    QStyleOption opt;
//    opt.initFrom(this);
//    QPainter p(this);

//    switch(state)
//    {
//    case NORMAL:
//        p.setBrush(QColor(0xff,0xff,0xff,0x0f));
//        p.setPen(Qt::NoPen);
//        break;
//    case HOVER:
//        p.setBrush(QColor(0xff,0xff,0xff,0x1f));
//        p.setPen(Qt::NoPen);
//        break;
//    case PRESS:
////        p.setBrush(Qt::green);
//        p.setPen(Qt::NoPen);
//        break;
//    }
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.drawRoundedRect(opt.rect,4,4);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}

int StorageArrow::GetTaskbarInfo()
{


}

void StorageArrow::setArrowIcon()
{
    switch (panelPosition) {
    case 1:
        setIcon(QIcon::fromTheme("pan-down-symbolic"));
        break;
    case 2:
        setIcon(QIcon::fromTheme("pan-end-symbolic"));
        break;
    case 3:
        setIcon(QIcon::fromTheme("pan-start-symbolic"));
        break;
    default:
        setIcon(QIcon::fromTheme("pan-up-symbolic"));
        break;
    }
    setProperty("useIconHighlightEffect", 0x2);
    setIconSize(QSize(iconsize/2,iconsize/2));
    setFixedSize(iconsize,iconsize * 1.3);
}

void StorageArrow::enterEvent(QEvent *) {
    repaint();
    return;
}

void StorageArrow::leaveEvent(QEvent *) {
    repaint();
    return;
}
