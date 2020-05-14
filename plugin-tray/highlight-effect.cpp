/*
 * Qt5-UKUI's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "highlight-effect.h"

#include <QAbstractItemView>
#include <QAbstractButton>
#include <QMenu>
#include <QStyleOption>

#include <QPixmap>
#include <QPainter>

#include <QImage>
#include <QtMath>

#include <QApplication>

#include <QDebug>

#define TORLERANCE 36

static QColor symbolic_color = Qt::gray;

void HighLightEffect::setSkipEffect(QWidget *w, bool skip)
{
    w->setProperty("skipHighlightIconEffect", skip);
}

bool HighLightEffect::isPixmapPureColor(const QPixmap &pixmap)
{
    QImage img = pixmap.toImage();
    bool init = false;
    int red = 0;
    int green = 0;
    int blue = 0;
    qreal variance = 0;
    qreal mean = 0;
    qreal standardDeviation = 0;
    QVector<int> pixels;
    bool isPure = true;
    bool isFullyPure = true;
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() != 0) {
                int hue = color.hue();
                pixels<<hue;
                mean += hue;
                if (!init) {
                    red = color.red();
                    green = color.green();
                    blue = color.blue();
                    init = true;
                } else {
                    color.setAlpha(255);
                    int r = color.red();
                    int g = color.green();
                    int b = color.blue();
                    int dr = qAbs(r - red);
                    int dg = qAbs(g - green);
                    int db = qAbs(b - blue);
                    bool same = dr < TORLERANCE && dg < TORLERANCE && db < TORLERANCE;
                    if (isFullyPure) {
                        if (dr > 0 || dg > 0 || db > 0) {
                            isFullyPure = false;
                        }
                    }
                    if (!same) {
                        if (isPure || isFullyPure) {
                            isPure = false;
                            isFullyPure = false;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (isPure)
        return true;

    mean = mean/pixels.count();
    for (auto hue : pixels) {
        variance += (hue - mean)*(hue - mean);
    }

    standardDeviation = qSqrt(variance/pixels.count());

    isFullyPure = standardDeviation == 0 || variance == 0;
    isPure = standardDeviation < 1 || variance == 0;

    return isPure;
}

bool HighLightEffect::setMenuIconHighlightEffect(QMenu *menu, bool set, HighLightEffect::EffectMode mode)
{
    if (!menu)
        return false;

    menu->setProperty("useIconHighlightEffect", set);
    menu->setProperty("iconHighlightEffectMode", mode);
    return true;
}

bool HighLightEffect::setViewItemIconHighlightEffect(QAbstractItemView *view, bool set, HighLightEffect::EffectMode mode)
{
    if (!view)
        return false;

    view->viewport()->setProperty("useIconHighlightEffect", set);
    view->viewport()->setProperty("iconHighlightEffectMode", mode);
    return true;
}

bool HighLightEffect::setButtonIconHighlightEffect(QAbstractButton *button, bool set, EffectMode mode)
{
    if (!button)
        return false;

    button->setProperty("useIconHighlightEffect", set);
    button->setProperty("iconHighlightEffectMode", mode);
    return true;
}

bool HighLightEffect::isWidgetIconUseHighlightEffect(const QWidget *w)
{
    if (w) {
        return w->property("useIconHighlightEffect").toBool();
    }
    return false;
}

void HighLightEffect::setSymoblicColor(const QColor &color)
{
    qApp->setProperty("symbolicColor", color);
    symbolic_color = color;
}

void HighLightEffect::setWidgetIconFillSymbolicColor(QWidget *widget, bool fill)
{
    widget->setProperty("fillIconSymbolicColor", fill);
}

const QColor HighLightEffect::getCurrentSymbolicColor()
{
    QIcon symbolic = QIcon::fromTheme("nm-device-wired");
    QPixmap pix = symbolic.pixmap(QSize(16, 16));
    QImage img = pix.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            QColor color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                symbolic_color = color;
                return color;
            }
        }
    }
    return symbolic_color;
}

QPixmap HighLightEffect::generatePixmap(const QPixmap &pixmap, const QStyleOption *option, const QWidget *widget, bool force, EffectMode mode)
{
    if (pixmap.isNull())
        return pixmap;

    if (widget) {
        if (widget->property("skipHighlightIconEffect").isValid()) {
            bool skipEffect = widget->property("skipHighlightIconEffect").toBool();
            if (skipEffect)
                return pixmap;
        }
    }

    bool isPurePixmap = isPixmapPureColor(pixmap);

    if (force) {
        if (!isPurePixmap)
            return pixmap;

        QPixmap target = pixmap;
        QPainter p(&target);

        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        if (option->state & QStyle::State_MouseOver ||
                option->state & QStyle::State_Selected ||
                option->state & QStyle::State_On ||
                option->state & QStyle::State_Sunken) {
            p.fillRect(target.rect(), option->palette.highlightedText());
        } else {
            if (mode == BothDefaultAndHighlit)
                p.fillRect(target.rect(), option->palette.dark());
        }
        //p.end();
        return target;
    }

    if (widget) {
        if (isWidgetIconUseHighlightEffect(widget)) {
            bool fillIconSymbolicColor = false;
            if (widget->property("fillIconSymbolicColor").isValid()) {
                fillIconSymbolicColor = widget->property("fillIconSymbolicColor").toBool();
            }

            if (widget->property("iconHighlightEffectMode").isValid()) {
                mode = qvariant_cast<EffectMode>(widget->property("iconHighlightEffectMode"));
            }

            bool isEnable = option->state.testFlag(QStyle::State_Enabled);
            bool overOrDown =  option->state.testFlag(QStyle::State_MouseOver) ||
                    option->state.testFlag(QStyle::State_Sunken) ||
                    option->state.testFlag(QStyle::State_On) ||
                    option->state.testFlag(QStyle::State_Selected);
            if (auto button = qobject_cast<const QAbstractButton *>(widget)) {
                if (button->isDown() || button->isChecked()) {
                    overOrDown = true;
                }
            }

            if (qobject_cast<const QAbstractItemView *>(widget)) {
                if (!option->state.testFlag(QStyle::State_Selected))
                    overOrDown = false;
            }

            if (isEnable && overOrDown) {
                QPixmap target = pixmap;
                if (fillIconSymbolicColor) {
                    target = filledSymbolicColoredPixmap(pixmap, option->palette.highlightedText().color());
                }

                if (!isPurePixmap)
                    return target;

                QPainter p(&target);
                p.setRenderHint(QPainter::Antialiasing);
                p.setRenderHint(QPainter::SmoothPixmapTransform);
                p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                p.fillRect(target.rect(), option->palette.highlightedText());
                //p.end();
                return target;
            } else {
                if (mode == BothDefaultAndHighlit) {
                    QPixmap target = pixmap;

                    if (fillIconSymbolicColor) {
                        target = filledSymbolicColoredPixmap(pixmap, option->palette.highlightedText().color());
                    }

                    if (!isPurePixmap)
                        return target;

                    QPainter p(&target);

                    p.setRenderHint(QPainter::Antialiasing);
                    p.setRenderHint(QPainter::SmoothPixmapTransform);
                    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                    p.fillRect(target.rect(), option->palette.dark());
                    //p.end();
                    return target;
                }
            }
        }
    } else {
        if (!isPurePixmap)
            return pixmap;

        bool isEnable = option->state.testFlag(QStyle::State_Enabled);
        bool overOrDown = option->state.testFlag(QStyle::State_MouseOver) ||
                option->state.testFlag(QStyle::State_Sunken) ||
                option->state.testFlag(QStyle::State_Selected) ||
                option->state.testFlag(QStyle::State_On);
        if (isEnable && overOrDown) {
            QPixmap target = pixmap;
            QPainter p(&target);

            p.setRenderHint(QPainter::Antialiasing);
            p.setRenderHint(QPainter::SmoothPixmapTransform);
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(target.rect(), option->palette.highlightedText());
            //p.end();
            return target;
        } else {
            if (mode == BothDefaultAndHighlit) {
                QPixmap target = pixmap;
                QPainter p(&target);

                p.setRenderHint(QPainter::Antialiasing);
                p.setRenderHint(QPainter::SmoothPixmapTransform);
                p.setCompositionMode(QPainter::CompositionMode_SourceIn);
                p.fillRect(target.rect(), option->palette.dark());
                //p.end();
                return target;
            }
        }
    }

    return pixmap;
}

HighLightEffect::HighLightEffect(QObject *parent) : QObject(parent)
{

}

QPixmap HighLightEffect::filledSymbolicColoredPixmap(const QPixmap &source, const QColor &baseColor)
{
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                int hue = color.hue();
                if (!qAbs(hue - symbolic_color.hue()) < 10) {
                    color.setRed(baseColor.red());
                    color.setGreen(baseColor.green());
                    color.setBlue(baseColor.blue());
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap HighLightEffect::drawSymbolicColoredPixmap(const QPixmap &source)
{
    QColor currentcolor=HighLightEffect::getCurrentSymbolicColor();
    QColor gray(128,128,128);
    QColor standard (31,32,34);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else if(qAbs(color.red()-standard.red())<20 && qAbs(color.green()-standard.green())<20 && qAbs(color.blue()-standard.blue())<20)
                {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else
                {
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}
