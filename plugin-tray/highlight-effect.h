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

#ifndef HIGHLIGHTEFFECT_H
#define HIGHLIGHTEFFECT_H

#include <QObject>
#include <QStyleOption>

class QAbstractItemView;
class QAbstractButton;
class QMenu;

class HighLightEffect : public QObject
{
    Q_OBJECT
public:
    enum EffectMode {
        HighlightOnly,
        BothDefaultAndHighlit
    };
    Q_ENUM(EffectMode)

    /*!
     * \brief setSkipEffect
     * \param w
     * \param skip
     * \details
     * in ukui-style, some widget such as menu will be set use highlight
     * icon effect automaticlly,
     * but we might not want to compose a special pure color image.
     * This function is use to skip the effect.
     */
    static void setSkipEffect(QWidget *w, bool skip = true);
    static bool isPixmapPureColor(const QPixmap &pixmap);
    static bool setMenuIconHighlightEffect(QMenu *menu, bool set = true, EffectMode mode = HighlightOnly);
    static bool setViewItemIconHighlightEffect(QAbstractItemView *view, bool set = true, EffectMode mode = HighlightOnly);
    static bool setButtonIconHighlightEffect(QAbstractButton *button, bool set = true, EffectMode mode = HighlightOnly);
    static bool isWidgetIconUseHighlightEffect(const QWidget *w);

    static void setSymoblicColor(const QColor &color);
    static void setWidgetIconFillSymbolicColor(QWidget *widget, bool fill);

    static const QColor getCurrentSymbolicColor();

    static QPixmap generatePixmap(const QPixmap &pixmap,
                                  const QStyleOption *option,
                                  const QWidget *widget = nullptr,
                                  bool force = false,
                                  EffectMode mode = HighlightOnly);
    static QPixmap filledSymbolicColoredPixmap(const QPixmap &source, const QColor &baseColor);
    static QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
private:
    explicit HighLightEffect(QObject *parent = nullptr);


};

#endif // HIGHLIGHTEFFECT_H
