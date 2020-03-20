/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
#include "qclickwidget.h"

QClickWidget::QClickWidget(QWidget *parent, bool is_eject, QString name, qlonglong capacity,QString path)
    : QWidget(parent),m_is_eject(is_eject),m_name(name),m_capacity(capacity),m_path(path)
{
    connect(this, SIGNAL(clicked()), this, SLOT(mouseClicked()));
}

QClickWidget::~QClickWidget()
{

}

void QClickWidget::mouseClicked()
{
    //处理代码
//        std::string str = m_path.toStdString();
//        const char* ch = str.c_str();
    qDebug() << "打开" << m_path;
    QProcess::startDetached("peony "+m_path);

    this->topLevelWidget()->hide();
}

void QClickWidget::mousePressEvent(QMouseEvent *ev)
{
    mousePos = QPoint(ev->x(), ev->y());
}

void QClickWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mousePos == QPoint(ev->x(), ev->y())) Q_EMIT clicked();
}

void QClickWidget::paintEvent(QPaintEvent *)
 {
     //解决QClickWidget类设置样式不生效的问题
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     p.setBrush(QColor(0x00,0x00,0x00));
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }
