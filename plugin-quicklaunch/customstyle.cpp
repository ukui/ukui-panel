#include "customstyle.h"

#include <QStyleOptionToolButton>
#include <QPainter>
#include <QApplication>
CustomStyle::CustomStyle(const QString &proxyStyleName, QObject *parent) : QProxyStyle (proxyStyleName)
{

}

CustomStyle::~CustomStyle()
{

}
void CustomStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{

    if(control == CC_ToolButton)
    {
        /// 我们参考qt的官方文档和源码，可以知道ToolButton需要绘制的子控件似乎只有两个
        /// QStyle::SC_ToolButton
        /// QStyle::SC_ToolButtonMenu

        /// 我们需要获取ToolButton的详细信息，通过qstyleoption_cast可以得到
        /// 对应的option，通过拷贝构造函数得到一份备份用于绘制子控件
        /// 我们一般不用在意option是怎么得到的，大部分的Qt控件都能够提供了option的init方法
//        const QStyleOptionToolButton opt = *qstyleoption_cast<const QStyleOptionToolButton*>(option);
//        QStyleOption tmp = opt;

        /// 我们可以通过subControlRect获取对应子控件的rect
//        auto subLineRect = subControlRect(CC_ToolButton, option, SC_ToolButton);
//        tmp.rect = subLineRect;
//        drawControl(CE_ToolBoxTabLabel, &tmp, painter, widget);

    }
    return QProxyStyle::drawComplexControl(control, option, painter, widget);


}

void CustomStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element)
    {
    case CE_ToolBoxTabShape:{
        painter->save();
        painter->setPen(Qt::black);
        painter->setBrush(Qt::green);
        painter->drawRect(option->rect.adjusted(0, 0, -1, 0));
        painter->restore();
        return;
    }
    case CE_ToolBoxTabLabel:{
        QIcon icon;
        if (option->state.testFlag(State_Horizontal)) {
            icon = QIcon::fromTheme("pan-start-symbolic");
        } else {
            icon = QIcon::fromTheme("pan-up-symolic");
        }

        /// 这里我使用调色板绘制图标的底色
        painter->setBrush(option->palette.foreground());
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
        icon.paint(painter, option->rect, Qt::AlignCenter);
        return;
    }
    case CE_HeaderEmptyArea:{
        painter->save();
        painter->setPen(Qt::black);
        painter->setBrush(Qt::green);
        painter->drawRect(option->rect.adjusted(0, 0, -1, 0));
        painter->restore();
        return;
    }
    }
    return QProxyStyle::drawControl(element, option, painter, widget);
}

void CustomStyle::drawItemPixmap(QPainter *painter, const QRect &rectangle, int alignment, const QPixmap &pixmap) const
{
    return QProxyStyle::drawItemPixmap(painter, rectangle, alignment, pixmap);
}

void CustomStyle::drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    return QProxyStyle::drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
}

void CustomStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    //绘制 ToolButton 圆角
    case PE_PanelButtonTool:{
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing,true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        if (option->state & State_MouseOver) {
        if (option->state & State_Sunken) {
        painter->setPen(option->palette.color(QPalette::Highlight));
        painter->setBrush(option->palette.color(QPalette::Highlight));
        } else {
        painter->setPen(option->palette.color(QPalette::Highlight));
        painter->setBrush(option->palette.color(QPalette::Highlight));
        }
        }
        painter->drawRoundedRect(option->rect,10,10);
        painter->restore();
        return;
    }

    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

QPixmap CustomStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *option) const
{
    return QProxyStyle::generatedIconPixmap(iconMode, pixmap, option);
}

QStyle::SubControl CustomStyle::hitTestComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, const QPoint &position, const QWidget *widget) const
{
    return QProxyStyle::hitTestComplexControl(control, option, position, widget);
}

QRect CustomStyle::itemPixmapRect(const QRect &rectangle, int alignment, const QPixmap &pixmap) const
{
    return QProxyStyle::itemPixmapRect(rectangle, alignment, pixmap);
}

QRect CustomStyle::itemTextRect(const QFontMetrics &metrics, const QRect &rectangle, int alignment, bool enabled, const QString &text) const
{
    return QProxyStyle::itemTextRect(metrics, rectangle, alignment, enabled, text);
}

//
int CustomStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric){
    case PM_ToolBarIconSize:{
        return (int)48*qApp->devicePixelRatio();
    }
    default:
        break;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

//
void CustomStyle::polish(QWidget *widget)
{
    return QProxyStyle::polish(widget);
}

void CustomStyle::polish(QApplication *application)
{
    return QProxyStyle::polish(application);
}

//
void CustomStyle::polish(QPalette &palette)
{
//    return QProxyStyle::polish(palette);
//    QProxyStyle::polish(palette);
//    palette.setBrush(QPalette::Foreground, Qt::black);
    QColor lightBlue(200, 0, 0);
    palette.setBrush(QPalette::Highlight, lightBlue);
}

void CustomStyle::unpolish(QWidget *widget)
{
    return QProxyStyle::unpolish(widget);
}

void CustomStyle::unpolish(QApplication *application)
{
    return QProxyStyle::unpolish(application);
}

QSize CustomStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
}

QIcon CustomStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

QPalette CustomStyle::standardPalette() const
{
    return QProxyStyle::standardPalette();
}

//如果需要背景透明也许需要用到这个函数
int CustomStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint) {
    /// 让ScrollView viewport的绘制区域包含scrollbar和corner widget
    /// 这个例子中没有什么作用，如果我们需要绘制一个背景透明的滚动条
    /// 这个style hint对我们的意义应该很大，因为我们希望视图能够帮助
    /// 我们填充滚动条的背景区域，否则当背景透明时底下会出现明显的分割
    case SH_ScrollView_FrameOnlyAroundContents: {
        return false;
    }
    default:
        break;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

QRect CustomStyle::subControlRect(QStyle::ComplexControl control, const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    return QProxyStyle::subControlRect(control, option, subControl, widget);
}

QRect CustomStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    return QProxyStyle::subElementRect(element, option, widget);
}
