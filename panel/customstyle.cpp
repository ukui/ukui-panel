/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
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
 */

#include "customstyle.h"

#include <QStyleOptionToolButton>
#include <QPainter>
#include <QApplication>

//#if QT_CONFIG(toolbutton)
/*以下代码是为了处理toolbutton 的箭头*/
static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
                      const QRect &rect, QPainter *painter, const QWidget *widget = 0)
{
    QStyle::PrimitiveElement pe;
    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }
    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);
}
//#endif // QT_CONFIG(toolbutton)

CustomStyle::CustomStyle(const QString &proxyStyleName, bool multileWins, QObject *parent) : QProxyStyle (proxyStyleName)
{
    pluginName=proxyStyleName;
    multileWindow=multileWins;
}
CustomStyle::~CustomStyle()
{
};

/*Draws the given control using the provided painter with the style options specified by option.*/
void CustomStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{

//    if(control == CC_ToolButton)
//    {
//        /// 我们需要获取ToolButton的详细信息，通过qstyleoption_cast可以得到
//        /// 对应的option，通过拷贝构造函数得到一份备份用于绘制子控件
//        /// 我们一般不用在意option是怎么得到的，大部分的Qt控件都能够提供了option的init方法

//    }
    switch (cc) {
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton
            = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect button, menuarea;
            button = proxy()->subControlRect(cc, toolbutton, SC_ToolButton, widget);
            menuarea = proxy()->subControlRect(cc, toolbutton, SC_ToolButtonMenu, widget);

            State bflags = toolbutton->state & ~State_Sunken;

            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
                    bflags &= ~State_Raised;
                }
            }
            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                mflags |= State_Sunken;
            }

            QStyleOption tool = *toolbutton;
            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button;
                    tool.state = bflags;
                    proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                                                      toolbutton, widget), 0);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fr, p, widget);
            }
            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt, widget);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            proxy()->drawControl(CE_ToolButtonLabel, &label, p, widget);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = mflags;
                if (mflags & (State_Sunken | State_On | State_Raised))
                    proxy()->drawPrimitive(PE_IndicatorButtonDropDown, &tool, p, widget);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, widget);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, widget);
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.right() + 5 - mbi, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                newBtn.rect = visualRect(toolbutton->direction, button, newBtn.rect);
                //proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
            }
        }
        return;
    default:
        break;
    }
    return QProxyStyle::drawComplexControl(cc, opt, p, widget);


}

/*下面对于CE_ToolButtonLabel　的处理是因为quicklaunch 插件出现了箭头*/
void CustomStyle::drawControl(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    switch (element) {
    case CE_ToolButtonLabel:
    {
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect rect = toolbutton->rect;
            int shiftX = 0;
            int shiftY = 0;
//            if (toolbutton->state & (State_Sunken | State_On)) {
//                shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, toolbutton, widget);
//                shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, toolbutton, widget);
//            }
            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())
                || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
                    alignment |= Qt::TextHideMnemonic;
                rect.translate(shiftX, shiftY);
                p->setFont(toolbutton->font);
                proxy()->drawItemText(p, rect, alignment, toolbutton->palette,
                             opt->state & State_Enabled, toolbutton->text,
                             QPalette::ButtonText);
            } else {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (!toolbutton->icon.isNull()) {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                        mode = QIcon::Disabled;
                    else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                        mode = QIcon::Active;
                    else
                        mode = QIcon::Normal;
                    pm = toolbutton->icon.pixmap(widget->window()->windowHandle(), toolbutton->rect.size().boundedTo(toolbutton->iconSize),
                                                 mode, state);
                    pmSize = pm.size() / pm.devicePixelRatio();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
                    p->setFont(toolbutton->font);
                    QRect pr = rect,
                    tr = rect;
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
                        alignment |= Qt::TextHideMnemonic;

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                        pr.setHeight(pmSize.height() + 4); //### 4 is currently hardcoded in QToolButton::sizeHint()
                        tr.adjust(0, pr.height() - 1, 0, -1);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow) {
                            proxy()->drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                        } else {
                            drawArrow(proxy(), toolbutton, pr, p, widget);
                        }
                        alignment |= Qt::AlignCenter;
                    } else {
                        pr.setWidth(pmSize.width() + 4); //### 4 is currently hardcoded in QToolButton::sizeHint()
                        tr.adjust(pr.width(), 0, 0, 0);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow) {
                            proxy()->drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                        } else {
                            drawArrow(proxy(), toolbutton, pr, p, widget);
                        }
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                    }
                    tr.translate(shiftX, shiftY);
                    //const QString text = d->toolButtonElideText(toolbutton, tr, alignment);
                    proxy()->drawItemText(p, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette,
                                 toolbutton->state & State_Enabled, toolbutton->text,
                                 QPalette::ButtonText);
                } else {
                    rect.translate(shiftX, shiftY);
                    if (hasArrow) {
                        drawArrow(proxy(), toolbutton, rect, p, widget);
                    } else {
                        proxy()->drawItemPixmap(p, rect, Qt::AlignCenter, pm);
                    }
                }
            }
        }
        return;
    }
    default:
        break;
    }
    return QProxyStyle::drawControl(element, opt, p, widget);
}

void CustomStyle::drawItemPixmap(QPainter *painter, const QRect &rectangle, int alignment, const QPixmap &pixmap) const
{
    return QProxyStyle::drawItemPixmap(painter, rectangle, alignment, pixmap);
}

void CustomStyle::drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    return QProxyStyle::drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole);
}

//绘制简单的颜色圆角等
void CustomStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    /*绘制 ToolButton
     * 任务栏的不同插件需要的样式存在差异
     * 在同一个PE中有两个toolbutton 的样式
    */
    case PE_PanelButtonTool:{
        if(QString::compare(pluginName,"taskbutton")==0)
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing,true);
            painter->setPen(Qt::NoPen);
	    painter->setBrush(QColor(0xff,0xff,0xff,0x14));
            if (option->state & State_Sunken) {
                painter->setBrush(QColor(0xff,0xff,0xff,0x0f));
            } else if (option->state & State_MouseOver) {
                painter->setBrush(QColor(0xff,0xff,0xff,0x1f));
            } else if (option->state & State_On) {
                painter->setBrush(QColor(0xff,0xff,0xff,0x33));
            }
            painter->drawRoundedRect(option->rect.adjusted(2,2,-2,-2),6,6);
            painter->restore();

            if(multileWindow)
            {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->setPen(Qt::NoPen);
                painter->setBrush(option->palette.color(QPalette::Highlight));
                painter->drawEllipse(option->rect.topLeft() + QPointF(8.5, 4.5), 2.5, 2.5);
                painter->setBrush(option->palette.color(QPalette::Highlight).light(125));
                painter->drawEllipse(option->rect.topLeft() + QPointF(4.5, 4.5), 2.5, 2.5);
                painter->restore();
            }
            return;
        }

        else if(QString::compare(pluginName,"closebutton")==0)
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing,true);
            painter->setPen(Qt::NoPen);
            //        painter->setBrush(QColor(0xff,0xff,0xff,0xff));
            painter->drawRoundedRect(option->rect,6,6);
            if (option->state & State_MouseOver) {
                if (option->state & State_Sunken) {
                    painter->setRenderHint(QPainter::Antialiasing,true);
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(QColor(0xd7,0x34,0x35));
                    painter->drawRoundedRect(option->rect,6,6);
                } else {
                    painter->setRenderHint(QPainter::Antialiasing,true);
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(QColor(0xf0,0x41,0x34));
                    painter->drawRoundedRect(option->rect,4,4);
                }
            }
            painter->restore();
            return;
        }

        else
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing,true);
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(option->rect,6,6);
            if (option->state & State_MouseOver) {
                if (option->state & State_Sunken) {
                    painter->setRenderHint(QPainter::Antialiasing,true);
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(QColor(0xff,0xff,0xff,0x0f));
                    painter->drawRoundedRect(option->rect,6,6);
                } else {
                    painter->setRenderHint(QPainter::Antialiasing,true);
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(QColor(0xff,0xff,0xff,0x1f));
                    painter->drawRoundedRect(option->rect.adjusted(2,2,-2,-2),6,6);
                }
            }
            painter->restore();
            return;
        }
    }


    case PE_PanelButtonCommand:{
        painter->save();
        painter->setRenderHint(QPainter::TextAntialiasing,true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::blue);
        if (option->state & State_MouseOver) {
            if (option->state & State_Sunken) {
                painter->setRenderHint(QPainter::Antialiasing,true);
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(0xff,0x00,0x00));
                painter->drawRoundedRect(option->rect,6,6);
            } else {
                painter->setRenderHint(QPainter::Antialiasing,true);
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(0x00,0xff,0x00));
                painter->drawRoundedRect(option->rect.adjusted(2,2,-2,-2),6,6);
            }
        }
        painter->restore();
        return;
    }break;
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

/*使悬浮点击等样式生效*/
void CustomStyle::polish(QWidget *widget)
{
    widget->setAttribute(Qt::WA_Hover);
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
