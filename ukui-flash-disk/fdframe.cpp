/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
 * 
 * Authors:
 *  Yang Min yangmin@kylinos.cn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QPainterPath>
#include <KWindowEffects>

#include "fdframe.h"

#define THEME_UKFD_TRANS "org.ukui.control-center.personalise"

FDFrame::FDFrame(QWidget* parent) : QFrame(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);
    //setWindowOpacity(0.8);
    initOpacityGSettings();
    KWindowEffects::enableBlurBehind(winId(), true);
}

FDFrame::~FDFrame()
{
    if (m_gsTransOpacity) {
        delete m_gsTransOpacity;
        m_gsTransOpacity = nullptr;
    }
}

void FDFrame::initOpacityGSettings()
{
    const QByteArray idtrans(THEME_UKFD_TRANS);
    if(QGSettings::isSchemaInstalled(idtrans)) {
        m_gsTransOpacity = new QGSettings(idtrans);
    }

    if (!m_gsTransOpacity) {
        m_curTransOpacity = 1;
        return;
    }

    connect(m_gsTransOpacity, &QGSettings::changed, this, [=](const QString &key) {
        if (key == "transparency") {
            QStringList keys = m_gsTransOpacity->keys();
            if (keys.contains("transparency")) {
                m_curTransOpacity = m_gsTransOpacity->get("transparency").toString().toDouble();
                repaint();
            }
        }
    });

    QStringList keys = m_gsTransOpacity->keys();
    if(keys.contains("transparency")) {
        m_curTransOpacity = m_gsTransOpacity->get("transparency").toString().toDouble();
    }
}

void FDFrame::paintEvent(QPaintEvent * event)
{
    QPainterPath path;

    QPainter painter(this);
    painter.setOpacity(m_curTransOpacity);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setClipping(true);
    painter.setPen(Qt::transparent);

    path.addRoundedRect(this->rect(), 6, 6);
    path.setFillRule(Qt::WindingFill);
    painter.setBrush(this->palette().base());
    painter.setPen(Qt::transparent);

    painter.drawPath(path);
    QFrame::paintEvent(event);
}