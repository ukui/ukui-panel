/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
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
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef UKUIPAGESELECTWIDGET_H
#define UKUIPAGESELECTWIDGET_H

#include "../ukuiglobals.h"
#include <QListWidget>

namespace UKUi
{

class UKUI_API PageSelectWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PageSelectWidget(QWidget *parent = 0);
    virtual ~PageSelectWidget();
    int maxTextWidth() const;
    bool event(QEvent * event) override;

    int getWrappedTextWidth() const {
        return mWrappedTextWidth;
    }

protected:
    QSize viewportSizeHint() const override;
    QSize minimumSizeHint() const override;

protected slots:
    void updateMaxTextWidth();

private:
    int mMaxTextWidth;
    int mWrappedTextWidth;
};

} // namespace UKUi
#endif // PAGESELECTWIDGET_H
