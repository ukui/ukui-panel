/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2015 LXQt team
 * Authors:
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

#include "spacer.h"
#include <QApplication>

void SpacerWidget::setType(QString const & type)
{
    if (type != mType)
    {
        mType = type;
        QEvent e{QEvent::ThemeChange};
        QApplication::sendEvent(this, &e);
    }
}

void SpacerWidget::setOrientation(QString const & orientation)
{
    if (orientation != mOrientation)
    {
        mOrientation = orientation;
        QEvent e{QEvent::ThemeChange};
        QApplication::sendEvent(this, &e);
    }
}

/************************************************

 ************************************************/
Spacer::Spacer(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject()
    , IUKUIPanelPlugin(startupInfo)
    , mSize(8)
    , mExpandable(false)
{
}

void Spacer::setSizes()
{
    if (mExpandable)
    {
        mSpacer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mSpacer.setMinimumSize({1, 1});
        mSpacer.setMaximumSize({QWIDGETSIZE_MAX, QWIDGETSIZE_MAX});
        mSpacer.setOrientation(panel()->isHorizontal() ? QStringLiteral("horizontal") : QStringLiteral("vertical"));
    } else
    {
        if (panel()->isHorizontal())
        {
            mSpacer.setOrientation(QStringLiteral("horizontal"));
            mSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            mSpacer.setFixedWidth(mSize);
            mSpacer.setMinimumHeight(0);
            mSpacer.setMaximumHeight(QWIDGETSIZE_MAX);
        }
        else
        {
            mSpacer.setOrientation(QStringLiteral("vertical"));
            mSpacer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            mSpacer.setFixedHeight(mSize);
            mSpacer.setMinimumWidth(0);
            mSpacer.setMaximumWidth(QWIDGETSIZE_MAX);
        }
    }
}

/************************************************

 ************************************************/
void Spacer::realign()
{
    setSizes();
}
