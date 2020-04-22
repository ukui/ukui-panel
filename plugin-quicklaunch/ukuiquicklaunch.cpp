/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include "ukuiquicklaunch.h"
#include "quicklaunchbutton.h"
#include "quicklaunchaction.h"
#include "../panel/iukuipanelplugin.h"
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QToolButton>
#include <QUrl>
#include <QDebug>
#include <XdgDesktopFile>
#include <XdgIcon>
#include "../panel/common/ukuigridlayout.h"
#include "../panel/pluginsettings.h"
#include <QTableWidget>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
using namespace  std;

UKUIQuickLaunch::UKUIQuickLaunch(IUKUIPanelPlugin *plugin, QWidget* parent) :
    QFrame(parent),
    mPlugin(plugin),
    mPlaceHolder(0)
{
    setAcceptDrops(true);
    mVBtn.clear();

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;

    const auto apps = mPlugin->settings()->readArray("apps");
    for (const QMap<QString, QVariant> &app : apps)
    {
        desktop = app.value("desktop", "").toString();
        file = app.value("file", "").toString();
        if (!desktop.isEmpty())
        {
            XdgDesktopFile xdg;
            if (!xdg.load(desktop))
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not valid";
                continue;
            }
            if (!xdg.isSuitable())
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not applicable";
                continue;
            }

            addButton(new QuickLaunchAction(&xdg, this));
        }
        else if (! file.isEmpty())
        {
            addButton(new QuickLaunchAction(file, this));
        }
        else
        {
            execname = app.value("name", "").toString();
            exec = app.value("exec", "").toString();
            icon = app.value("icon", "").toString();
            if (icon.isNull())
            {
                qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
                continue;
            }
            addButton(new QuickLaunchAction(execname, exec, icon, this));
        }
    } // for

    if (mLayout->isEmpty())
        showPlaceHolder();

    realign();
}


UKUIQuickLaunch::~UKUIQuickLaunch()
{
    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        (*it)->deleteLater();
        mVBtn.erase(it);
    }
    mVBtn.clear();
}


int UKUIQuickLaunch::indexOfButton(QuickLaunchButton* button) const
{
    return mLayout->indexOf(button);
}


int UKUIQuickLaunch::countOfButtons() const
{
    return mLayout->count();
}


void UKUIQuickLaunch::realign()
{
    mLayout->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();

    if (mPlaceHolder)
    {
        mLayout->setColumnCount(1);
        mLayout->setRowCount(1);
    }
    else
    {
        /*这里可能存在cpu占用过高的情况*/
        if (panel->isHorizontal())
        {
            if(mVBtn.size()<10)
            {
            mLayout->setRowCount(panel->lineCount());
            mLayout->setColumnCount(0);
            for(auto it = mVBtn.begin(); it != mVBtn.end(); it++)
            {
                (*it)->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize());
                (*it)->setIconSize(QSize(mPlugin->panel()->iconSize(),mPlugin->panel()->iconSize()));
            }
            }
            else
            {
                mLayout->setRowCount(2);
                mLayout->setColumnCount(0);
                for(auto it = mVBtn.begin(); it != mVBtn.end(); it++)
                {
                    (*it)->setFixedSize(mPlugin->panel()->panelSize()/2,mPlugin->panel()->panelSize()/2);
                    (*it)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
            }
        }
        else
        {
//            mLayout->setColumnCount(panel->lineCount());
//            mLayout->setRowCount(0);
            if(mVBtn.size()<6)
            {
            mLayout->setColumnCount(panel->lineCount());
            mLayout->setRowCount(0);
            for(auto it = mVBtn.begin(); it != mVBtn.end(); it++)
            {
                (*it)->setFixedSize(mPlugin->panel()->panelSize(),mPlugin->panel()->panelSize());
                (*it)->setIconSize(QSize(mPlugin->panel()->iconSize(),mPlugin->panel()->iconSize()));
            }
            }
            else
            {
                mLayout->setColumnCount(2);
                mLayout->setRowCount(0);
                for(auto it = mVBtn.begin(); it != mVBtn.end(); it++)
                {
                    (*it)->setFixedSize(mPlugin->panel()->panelSize()/2,mPlugin->panel()->panelSize()/2);
                    (*it)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
                }
            }



        }
    }
    mLayout->setEnabled(true);
}

void UKUIQuickLaunch::addButton(QuickLaunchAction* action)
{
    mLayout->setEnabled(false);
    QuickLaunchButton *btn = new QuickLaunchButton(action, mPlugin, this);
    btn->setArrowType(Qt::NoArrow);
    /*＠bug
     * 快速启动栏右键菜单原本的样式有对于不可选项有置灰效果，
     * 后跟随主题框架之后置灰效果消失，可能与此属性相关
     */
    //        btn->setMenu(Qt::InstantPopup);
    mVBtn.push_back(btn);
    mLayout->addWidget(btn);
    connect(btn, SIGNAL(switchButtons(QuickLaunchButton*,QuickLaunchButton*)), this, SLOT(switchButtons(QuickLaunchButton*,QuickLaunchButton*)));
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(movedLeft()), this, SLOT(buttonMoveLeft()));
    connect(btn, SIGNAL(movedRight()), this, SLOT(buttonMoveRight()));

    mLayout->removeWidget(mPlaceHolder);
    mPlaceHolder->deleteLater();
    mPlaceHolder = NULL;
    mLayout->setEnabled(true);
    realign();
}


bool UKUIQuickLaunch::checkButton(QuickLaunchAction* action)
{
    bool checkresult;
    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    int i = 0;
    QLayoutItem *child;

    if(mLayout->layout()->count()>0)
    {
        while ((child = mLayout->layout()->itemAt(i))) {
            QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
            if (b->file_name == btn->file_name)
            {
                qDebug()<<" already  insert  ";
                checkresult=true;
                break;
            }
            else {
                ++i;
                qDebug()<<"don't insert ";
                checkresult=false;
            }

        }
        return checkresult;
    }
    else
    {
        return true;
    }

}

//check by filename
void UKUIQuickLaunch::checkButton(QString *filename)
{
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        qDebug()<<i;
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if(b)
        {
            if (b->file_name == filename) {
                qDebug()<<" already  insert  "<<child;                //mLayout->removeItem(child);
            } else {
                ++i;
                qDebug()<<"not insert  ";
            }
        }
    }
}

void UKUIQuickLaunch::removeButton(QuickLaunchAction* action)
{
    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        qDebug()<<" mLayout->layout()->itemAt(i)";
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if (b->file_name == btn->file_name) {
            //mLayout->removeItem(child);
            if(mLayout->layout()->count()>1)
            {
                mLayout->removeWidget(b);
                qDebug()<<"removeButton   ****";
            }
            //b->deleteLater();
        } else {
            ++i;
        }
    }
    saveSettings();
}

//remove by filename
void UKUIQuickLaunch::removeButton(QString *filename)
{
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        qDebug()<<i;
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        qDebug()<<"b->file_name   ---"<<b->file_name;
        if (b->file_name == filename) {
            qDebug()<<"child   >>>"<<child;
            //mLayout->removeItem(child);
            mLayout->removeWidget(b);
            b->deleteLater();
        } else {
            ++i;
        }
    }
    saveSettings();
}

void UKUIQuickLaunch::dragEnterEvent(QDragEnterEvent *e)
{
    // Getting URL from mainmenu...
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
        return;
    }

    if (e->source() && e->source()->parent() == this)
    {
        e->acceptProposedAction();
    }
}


void UKUIQuickLaunch::dropEvent(QDropEvent *e)
{
    qDebug()<<"UKUIQuickLaunch::dropEvent";
    const auto urls = e->mimeData()->urls().toSet();
    for (const QUrl &url : urls)
    {
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        QFileInfo fi(fileName);
        XdgDesktopFile xdg;
        if (xdg.load(fileName))
        {
            if (xdg.isSuitable())
                addButton(new QuickLaunchAction(&xdg, this));
        }
        else if (fi.exists() && fi.isExecutable() && !fi.isDir())
        {
            addButton(new QuickLaunchAction(fileName, fileName, "", this));
        }
        else if (fi.exists())
        {
            addButton(new QuickLaunchAction(fileName, this));
        }
        else
        {
            qWarning() << "XdgDesktopFile" << fileName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                                     tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                                     );
        }
    }
    saveSettings();
}

void UKUIQuickLaunch::paintEvent(QPaintEvent *)
{
    //设置quicklaunch 区域底色
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool UKUIQuickLaunch::AddToTaskbar(QString arg)
{
    qDebug()<<"AddToTaskbar                  *************8";
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    if (xdg.load(fileName))
    {
        /*This fuction returns true if the desktop file is applicable to the
          current environment.
          but I don't need this attributes now
        */
        //        if (xdg.isSuitable())
        addButton(new QuickLaunchAction(&xdg, this));
    }
    else if (fi.exists() && fi.isExecutable() && !fi.isDir())
    {
        addButton(new QuickLaunchAction(fileName, fileName, "", this));
    }
    else if (fi.exists())
    {
        addButton(new QuickLaunchAction(fileName, this));
    }
    else
    {
        qWarning() << "XdgDesktopFile" << fileName << "is not valid";
        QMessageBox::information(this, tr("Drop Error"),
                                 tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                                 );
    }
    saveSettings();
    return true;
}

/*
 * @need resolved bug
 * 为开始菜单提供检测应用是否在任务栏上面的接口
 * 由于在删除最后一个应用的时候会出现任务栏崩溃的问题，
 * 所以暂定　mLayout->count()>0的情况下才进行checkButton操作
 * 在mLayout->count()>0的情况下才进行removeButton操作
 */
bool UKUIQuickLaunch::CheckIfExist(QString arg)
{
    if (mLayout->count()>0)
    {
        const auto url=QUrl(arg);
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        XdgDesktopFile xdg;
        xdg.load(fileName);
        bool state;
        state=checkButton(new QuickLaunchAction(&xdg, this));
        return state;
    }
    else
        return false;
}

bool UKUIQuickLaunch::RemoveFromTaskbar(QString arg)
{
    if(mLayout->count()>1)
    {
        qDebug()<<"RemoveFromTaskbar  ****";
        const auto url=QUrl(arg);
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        XdgDesktopFile xdg;
        xdg.load(fileName);
        removeButton(new QuickLaunchAction(&xdg, this));
        return true;
    }
}

int UKUIQuickLaunch::GetPanelPosition(QString arg)
{
    return mPlugin->panel()->position();
}

int UKUIQuickLaunch::GetPanelSize(QString arg)
{
    return mPlugin->panel()->panelSize();
}

void UKUIQuickLaunch::switchButtons(QuickLaunchButton *button1, QuickLaunchButton *button2)
{
    if (button1 == button2)
        return;

    int n1 = mLayout->indexOf(button1);
    int n2 = mLayout->indexOf(button2);

    int l = qMin(n1, n2);
    int m = qMax(n1, n2);

    mLayout->moveItem(l, m);
    mLayout->moveItem(m-1, l);
    saveSettings();
}


void UKUIQuickLaunch::buttonDeleted()
{
    if(mLayout->count()>1)
    {
        QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
        if (!btn)
            return;
        for(auto it = mVBtn.begin();it != mVBtn.end();it++)
        {
            if(mLayout->layout()->count()>0)
            {
                if(*it == btn)
                {
                    mVBtn.erase(it);
                    break;
                }
            }
            else
                qDebug()<<"can 't buttonDeleted   ";
        }

        btn->deleteLater();
        mLayout->removeWidget(btn);
        saveSettings();

        if (mLayout->isEmpty())
            showPlaceHolder();
        realign();
    }

}


void UKUIQuickLaunch::buttonMoveLeft()
{
    QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn)
        return;

    int index = indexOfButton(btn);
    if (index > 0)
    {
        mLayout->moveItem(index, index - 1);
        saveSettings();
    }
}


void UKUIQuickLaunch::buttonMoveRight()
{
    QuickLaunchButton *btn1 = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn1)
        return;

    int index = indexOfButton(btn1);
    if (index < countOfButtons() - 1)
    {
        mLayout->moveItem(index, index + 1);
        saveSettings();
    }
}


void UKUIQuickLaunch::saveSettings()
{
    PluginSettings *settings = mPlugin->settings();
    settings->remove("apps");

    QList<QMap<QString, QVariant> > hashList;
    int size = mLayout->count();
    for (int j = 0; j < size; ++j)
    {
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(j)->widget());
        if (!b)
            continue;

        // convert QHash<QString, QString> to QMap<QString, QVariant>
        QMap<QString, QVariant> map;
        QHashIterator<QString, QString> it(b->settingsMap());
        while (it.hasNext())
        {
            it.next();
            map[it.key()] = it.value();
        }
        hashList << map;
    }

    settings->setArray("apps", hashList);
}


void UKUIQuickLaunch::showPlaceHolder()
{
    if (!mPlaceHolder)
    {
        mPlaceHolder = new QLabel(this);
        mPlaceHolder->setAlignment(Qt::AlignCenter);
        mPlaceHolder->setObjectName("QuickLaunchPlaceHolder");
        mPlaceHolder->setText(tr("Drop application\nicons here"));
    }

    mLayout->addWidget(mPlaceHolder);
}





/*
 * Implementation of adaptor class FilectrlAdaptor
 */

FilectrlAdaptor::FilectrlAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

FilectrlAdaptor::~FilectrlAdaptor()
{
    // destructor
}

bool FilectrlAdaptor::AddToTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddToTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

bool FilectrlAdaptor::CheckIfExist(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "CheckIfExist", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

bool FilectrlAdaptor::RemoveFromTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

int FilectrlAdaptor::GetPanelPosition(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelPosition", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}

int FilectrlAdaptor::GetPanelSize(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelSize", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}
