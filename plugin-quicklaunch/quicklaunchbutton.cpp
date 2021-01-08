/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "quicklaunchbutton.h"
#include "ukuiquicklaunch.h"
#include "../panel/iukuipanelplugin.h"
#include <QAction>
#include <QDrag>
#include <QMenu>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QApplication>
#include <XdgIcon>
#include <string>
#include <XdgDesktopFile>
#include <QMessageBox>

#define MIMETYPE            "x-ukui/quicklaunch-button"
#define UKUI_PANEL_SETTINGS "org.ukui.panel.settings"
#define PANELPOSITION       "panelposition"


QuickLaunchButton::QuickLaunchButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget * parent)
    : QToolButton(parent),
      mAct(act),
      mPlugin(plugin)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyle(new CustomStyle());
    setAcceptDrops(true);
    /*设置快速启动栏的按键不接受焦点*/
    //setFocusPolicy(Qt::NoFocus);
    setAutoRaise(true);
    quicklanuchstatus = NORMAL;

    setDefaultAction(mAct);
    mAct->setParent(this);

    /*设置快速启动栏的菜单项*/
    const QByteArray id(UKUI_PANEL_SETTINGS);
    mgsettings = new QGSettings(id);
    QTimer::singleShot(5000,[this] {modifyQuicklaunchMenuAction(true);});
    connect(mgsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANELPOSITION){
            modifyQuicklaunchMenuAction(true);
        }
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(this_customContextMenuRequested(const QPoint&)));
    file_name=act->m_settingsMap["desktop"];
    file = act->m_settingsMap["file"];
    exec = act->m_settingsMap["exec"];
    name = act->m_settingsMap["name"];
    repaint();

}


QuickLaunchButton::~QuickLaunchButton()
{
    //m_act->deleteLater();
}


QHash<QString,QString> QuickLaunchButton::settingsMap()
{
    Q_ASSERT(mAct);
    return mAct->settingsMap();
}

/*与鼠标右键的选项有关*/
void QuickLaunchButton::this_customContextMenuRequested(const QPoint & pos)
{
    UKUIQuickLaunch *panel = qobject_cast<UKUIQuickLaunch*>(parent());
    mMoveLeftAct->setEnabled( panel && panel->indexOfButton(this) > 0);
    mMoveRightAct->setEnabled(panel && panel->indexOfButton(this) < panel->countOfButtons() - 1);

//    mMoveLeftAct->setEnabled( panel &&
//                              panel->indexOfButton(this) > 0 &&
//                              (mPlugin->panel()->isHorizontal() ? (panel->indexOfButton(this) % 5 != 0) :
//                               (mPlugin->panel()->isMaxSize() ? (panel->indexOfButton(this) % 2 != 0) :
//                                (panel->indexOfButton(this) % 3 != 0))));
//    mMoveRightAct->setEnabled(panel &&
//                              panel->indexOfButton(this) < panel->countOfButtons() - 1 &&
//                              (mPlugin->panel()->isHorizontal() ? (panel->indexOfButton(this) % 5 != 4) :
//                               (mPlugin->panel()->isMaxSize() ? (panel->indexOfButton(this) % 2 != 1) :
//                                (panel->indexOfButton(this) % 3 != 2))));
    mPlugin->willShowWindow(mMenu);
    mMenu->popup(mPlugin->panel()->calculatePopupWindowPos(mapToGlobal({0, 0}), mMenu->sizeHint()).topLeft());
}

/*调整快速启动栏的菜单项*/
void QuickLaunchButton::modifyQuicklaunchMenuAction(bool direction)
{
    if(mPlugin->panel()->isHorizontal()){
            mMoveLeftAct = new QAction(XdgIcon::fromTheme("go-previous"), tr("move to left"), this);
            mMoveRightAct = new QAction(XdgIcon::fromTheme("go-next"), tr("move to right"), this);
    }
    else{
            mMoveLeftAct = new QAction(XdgIcon::fromTheme("go-previous"), tr("move to up"), this);
            mMoveRightAct = new QAction(XdgIcon::fromTheme("go-next"), tr("move to down"), this);
    }

    mDeleteAct = new QAction(XdgIcon::fromTheme("dialog-close"), tr("delete from quicklaunch"), this);
    connect(mMoveLeftAct, SIGNAL(triggered()), this, SIGNAL(movedLeft()));
    connect(mMoveRightAct, SIGNAL(triggered()), this, SIGNAL(movedRight()));
    connect(mDeleteAct, SIGNAL(triggered()), this, SLOT(selfRemove()));
    //addAction(mDeleteAct);
    mMenu = new QuicklaunchMenu();
    mMenu->addAction(mAct);
    mMenu->addActions(mAct->addtitionalActions());
    mMenu->addSeparator();
    mMenu->addAction(mMoveLeftAct);
    mMenu->addAction(mMoveRightAct);
    mMenu->addSeparator();
    mMenu->addAction(mDeleteAct);
}

void QuickLaunchButton::selfRemove()
{
    emit buttonDeleted();
}

/***************************************************/

/*quicklanuchstatus的状态*/
void QuickLaunchButton::enterEvent(QEvent *)
{
    //quicklanuchstatus =HOVER;
    repaint();
}

/***************************************************/

void QuickLaunchButton::leaveEvent(QEvent *)
{
    //quicklanuchstatus=NORMAL;
    repaint();
}


/***************************************************/

void QuickLaunchButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier)
    {
        mDragStart = e->pos();
        return;
    }

    QToolButton::mousePressEvent(e);
}

/***************************************************/

QMimeData * QuickLaunchButton::mimeData()
{
    ButtonMimeData *mimeData = new ButtonMimeData();
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::WriteOnly);
    stream<<this;
    mimeData->setData(mimeDataFormat(), ba);
    mimeData->setButton(this);
    return mimeData;
}

/***************************************************/

void QuickLaunchButton::mouseMoveEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        return;
    if (!(e->buttons() & Qt::LeftButton))
        return;
    if ((e->pos() - mDragStart).manhattanLength() < QApplication::startDragDistance())
        return;

    if (e->modifiers() == Qt::ControlModifier)
    {
        return;
    }
    QDrag *drag = new QDrag(this);
    QIcon ico = icon();
    int size = mPlugin->panel()->iconSize();
    QPixmap img = ico.pixmap(ico.actualSize({size, size}));

    drag->setMimeData(mimeData());
    drag->setPixmap(img);

    switch (mPlugin->panel()->position())
    {
        case IUKUIPanel::PositionLeft:
        case IUKUIPanel::PositionTop:
            drag->setHotSpot({0, 0});
            break;
        case IUKUIPanel::PositionRight:
        case IUKUIPanel::PositionBottom:
            drag->setHotSpot(img.rect().bottomRight());
            break;
    }

    drag->exec();
    drag->deleteLater();

    //QAbstractButton::mouseMoveEvent(e);
}

/***************************************************/

void QuickLaunchButton::dragMoveEvent(QDragMoveEvent * e)
{
    if (e->mimeData()->hasFormat(mimeDataFormat()))
        e->acceptProposedAction();
//    else
//        e->ignore();
}

void QuickLaunchButton::dragLeaveEvent(QDragLeaveEvent * e)
{
    repaint();
    QToolButton::dragLeaveEvent(e);
}

/***************************************************/

void QuickLaunchButton::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
    const ButtonMimeData *mimeData = qobject_cast<const ButtonMimeData*>(e->mimeData());
    if (mimeData && mimeData->button())
          emit switchButtons(mimeData->button(), this);
    repaint();
    QToolButton::dragEnterEvent(e);
}

/***************************************************/

void QuickLaunchButton::mouseReleaseEvent(QMouseEvent* e)
{
    repaint();
    QToolButton::mouseReleaseEvent(e);
}

void QuickLaunchButton::dropEvent(QDropEvent *e)
{
    UKUIQuickLaunch *uqk = qobject_cast<UKUIQuickLaunch*>(parent());
    qDebug()<<"UKUIQuickLaunch::dropEvent";
    const auto urls = e->mimeData()->urls().toSet();
    for (const QUrl &url : urls)
    {
        XdgDesktopFile xdg;
        QString urlName(url.isLocalFile() ? url.toLocalFile() : url.url());
        QFileInfo ur(urlName);
        QString fileName("/usr/share/applications/");

        fileName.append(urlName.section('/', -1, -1));
        fileName = uqk->isComputerOrTrash(urlName);
        urlName = uqk->isComputerOrTrash(urlName);

        if (uqk->pubCheckIfExist(urlName)) return;
        if (uqk->pubCheckIfExist(fileName)) return;
        if (uqk->isDesktopFile(urlName)) {
            if (ur.isSymLink()){
                if (xdg.load(urlName) && xdg.isSuitable()) {
                   if (uqk->pubCheckIfExist(xdg.fileName())) return;
                   uqk->pubAddButton(new QuickLaunchAction(&xdg, this));
                }
            } else {
                if (xdg.load(fileName) && xdg.isSuitable()) {
                   if (uqk->pubCheckIfExist(urlName)) return;
                   uqk->pubAddButton(new QuickLaunchAction(&xdg, this));
                }
            }
        } else if (ur.exists() && ur.isExecutable() && !ur.isDir() || ur.isSymLink()) {
            if (ur.size() <= 153600)
                xdg.load(urlName);
            uqk->pubAddButton(new QuickLaunchAction(urlName, this));
        } else if (ur.exists()) {
            if (ur.size() <= 153600) {
                xdg.load(urlName);
            }
            uqk->pubAddButton(new QuickLaunchAction(urlName, this));
            //taskbar->pubAddButton(new QuickLaunchAction(urlName, urlName, "", this));
        } else {
            qWarning() << "XdgDesktopFile" << urlName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                                     tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(urlName)
                                     );
        }
    }
    uqk->saveSettings();

}
/***************************************************/

void QuickLaunchButton::contextMenuEvent(QContextMenuEvent *) { }

QuicklaunchMenu::QuicklaunchMenu() { }

QuicklaunchMenu::~QuicklaunchMenu() { }

void QuicklaunchMenu::contextMenuEvent(QContextMenuEvent *) { }
