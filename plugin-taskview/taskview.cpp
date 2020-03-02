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


#include "taskview.h"
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QScreen>
#include <QDebug>
#include <QToolBar>
#include <QStyle>
TaskView::TaskView(const IUKUIPanelPluginStartupInfo &startupInfo) :
    QObject(),
    IUKUIPanelPlugin(startupInfo)
{
    realign();

}


TaskView::~TaskView()
{
}


void TaskView::realign()
{
    mWidget.setFixedSize(panel()->panelSize(),panel()->panelSize());
}

TaskViewWidget::TaskViewWidget(QWidget *parent):
    QFrame(parent)
{
    taskviewstatus=NORMAL;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (1);
    setLayout(layout);
    layout->addWidget(&mButton);
    mCapturing = false;
    connect(&mButton, SIGNAL(clicked()), this, SLOT(captureMouse()));

    QSize mBtnSize(92,92);
//    mButton.setIcon(QIcon("/usr/share/ukui-panel/plugin-taskview/img/taskview.svg"));
    mButton.setIconSize(mBtnSize);
    mButton.setStyleSheet(
                //正常状态样式
                "QToolButton{"
                "background-color:rgba(190,216,239,0%);"
                "qproperty-icon:url(/usr/share/ukui-panel/plugin-taskview/img/taskview.svg);"
                //"qproperty-iconSize:28px 28px;"
                "border-style:outset;"                  //边框样式（inset/outset）
                "border-width:2px;"                     //边框宽度像素
                "border-radius:6px;"                   //边框圆角半径像素
                "border-color:rgba(190,216,239,0%);"    //边框颜色
                "padding:7px;"

                "}"
                //鼠标悬停样式
                "QToolButton:hover{"
                "background-color:rgba(190,216,239,20%);"
                "}"
                //鼠标按下样式
                "QToolButton:pressed{"
                "background-color:rgba(190,216,239,12%);"
                "}"

                );

}



TaskViewWidget::~TaskViewWidget()
{
}


void TaskViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
}


void TaskViewWidget::captureMouse()
{
    system("ukui-window-switch --show-workspace");
}

void TaskViewWidget::contextMenuEvent(QContextMenuEvent *event) {
}


//void TaskViewWidget::enterEvent(QEvent *)
//{
//    taskviewstatus=HOVER;
//    repaint();
//}

//void TaskViewWidget::leaveEvent(QEvent *)
//{
//    taskviewstatus=NORMAL;
//    repaint();
//}

void TaskViewWidget::paintEvent(QPaintEvent *)
{
//        CustomStyle opt;
////        opt.initFrom(this);
//        QPainter p(this);

//        switch(taskviewstatus)
//          {
//          case NORMAL:
//              {
//                  p.setBrush(QBrush(QColor(0xBE,0xD8,0xEF,0x00)));
//                  p.setPen(Qt::NoPen);
//                  break;
//              }
//          case HOVER:
//              {
//                  p.setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x19)));
//                  p.setPen(Qt::NoPen);
//                  break;
//              }
//          case PRESS:
//              {
//                  p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
//                  p.setPen(Qt::NoPen);
//                  break;
//              }
//          }
//        p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//        p.drawRoundedRect(opt.rect,6,6);
//        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
