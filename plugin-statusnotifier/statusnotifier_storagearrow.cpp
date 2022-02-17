/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  Lesser General Public License as published by
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

#include "statusnotifier_storagearrow.h"
#include "statusnotifierbutton.h"
#include "../panel/customstyle.h"

StatusNotifierStorageArrow::StatusNotifierStorageArrow(StatusNotifierWidget *parent)
{
    mParent=parent;
    this->setParent(parent);
    this->setAcceptDrops(true);
    this->setStyle(new CustomStyle);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id)){
        mGsettings = new QGSettings(id);
        connect(mGsettings, &QGSettings::changed, this, [=] (const QString &key){
            if(key == PANEL_POSITION_KEY){
                mPanelPosition=mGsettings->get(PANEL_POSITION_KEY).toInt();
                setArrowIcon();
            }
        });
    }
    QTimer::singleShot(10,this,[=](){
       setArrowIcon();
    });
    this->setProperty("useIconHighlightEffect", 0x2);
    this->setContextMenuPolicy(Qt::PreventContextMenu); //不显示右键菜单并且不将事件往基类传递
}

StatusNotifierStorageArrow::~StatusNotifierStorageArrow()
{

}

void StatusNotifierStorageArrow::dropEvent(QDropEvent *event){
}

void StatusNotifierStorageArrow::dragEnterEvent(QDragEnterEvent *event){
    const StatusNotifierButtonMimeData *mimeData = qobject_cast<const StatusNotifierButtonMimeData*>(event->mimeData());
    if (mimeData && mimeData->button()){
        emit addButton(mimeData->button()->mId);
    }

    if(mGsettings){
        mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
        setArrowIcon();
    }
    event->accept();
    QToolButton::dragEnterEvent(event);
}

void StatusNotifierStorageArrow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        if(mParent->Direction){
            if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
                setIcon(QIcon::fromTheme("ukui-start-symbolic"));
                mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
            }
            else{
                setIcon(QIcon::fromTheme("ukui-end-symbolic"));
                mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
            }
        }
        else{
            if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
                setIcon(QIcon::fromTheme("ukui-up-symbolic"));
                mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
            }
            else{
                setIcon(QIcon::fromTheme("ukui-down-symbolic"));
                mGsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
            }
        }
    }
}

void StatusNotifierStorageArrow::setArrowIcon()
{
    if(mParent->Direction){
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("ukui-end-symbolic"));
        }
        else{
            setIcon(QIcon::fromTheme("ukui-start-symbolic"));
        }
    }else{
        if(mGsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
            setIcon(QIcon::fromTheme("ukui-down-symbolic"));
        }
        else{
            setIcon(QIcon::fromTheme("ukui-up-symbolic"));
        }
    }
}

void StatusNotifierStorageArrow::resizeEvent(QResizeEvent *event){
    if (mParent->Direction)
    {
        this->setIconSize(QSize(this->width()*0.5,this->width()*0.5));
    }
    else
    {
        this->setIconSize(QSize(this->height()*0.5,this->height()*0.5));
    }

     QToolButton::resizeEvent(event);
}

void StatusNotifierStorageArrow::mouseMoveEvent(QMouseEvent *e)
{
    QDrag *drag = new QDrag(this);
    drag->exec();
    drag->deleteLater();
}
