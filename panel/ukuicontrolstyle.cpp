#include "ukuicontrolstyle.h"
#include <QStyleOption>
#include <QPainter>
#include <QToolTip>
#include <QPalette>
UKUiMenu::UKUiMenu(){
}

UKUiMenu::~UKUiMenu(){
}

void UKUiMenu::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(Qt::red));
    p.setPen(Qt::black);
    p.drawRoundedRect(opt.rect,6,6);
//    p.drawText(rect(), opt. Qt::AlignCenter,);
    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &p, this);
}

UkuiToolButton::UkuiToolButton(){}
UkuiToolButton::~UkuiToolButton(){}
void UkuiToolButton::paintTooltipStyle()
{
    //设置QToolTip颜色
    QPalette palette = QToolTip::palette();
    palette.setColor(QPalette::Inactive,QPalette::ToolTipBase,Qt::black);   //设置ToolTip背景色
    palette.setColor(QPalette::Inactive,QPalette::ToolTipText, Qt::white); 	//设置ToolTip字体色
    QToolTip::setPalette(palette);
//    QFont font("Segoe UI", -1, 50);
//    font.setPixelSize(12);
//    QToolTip::setFont(font);  //设置ToolTip字体
}
