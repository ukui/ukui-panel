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
    qDebug()<<"GetPanelPosition panel.position"<<mPlugin->panel()->position();


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
    for(auto it = mVBtn.begin(); it != mVBtn.end(); it++)
    {
        (*it)->setFixedSize(46,mPlugin->panel()->panelSize());
    }
    mLayout->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();

    if (mPlaceHolder)
    {
        mLayout->setColumnCount(1);
        mLayout->setRowCount(1);
    }
    else
    {
        if (panel->isHorizontal())
        {
            mLayout->setRowCount(panel->lineCount());
            mLayout->setColumnCount(0);
        }
        else
        {
            mLayout->setColumnCount(panel->lineCount());
            mLayout->setRowCount(0);
        }
    }
    mLayout->setEnabled(true);
}

void UKUIQuickLaunch::addButton(QuickLaunchAction* action)
{
    mLayout->setEnabled(false);

    QuickLaunchButton *btn = new QuickLaunchButton(action, mPlugin, this);
    btn->setFixedSize(46,mPlugin->panel()->panelSize());
    //save btn
    mVBtn.push_back(btn);
    mLayout->addWidget(btn);
    //set button style
    btn->setIconSize(QSize(28,28));
    btn->setStyleSheet(
                //正常状态样式
                "QToolButton{"
                "background-color:rgba(190,216,239,0%);"
                                "border-style:outset;"                  //边框样式（inset/outset）
                                "border-color:rgba(190,216,239,0%);"    //边框颜色
                                "qproperty-iconSize: 28px 28px;"
                                "border-width:4px;"                     //边框宽度像素
                                "border-radius:6px;"                   //边框圆角半径像素
                                "padding:0px;"
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
    saveSettings();
    return checkresult;
}

//check by filename
void UKUIQuickLaunch::checkButton(QString *filename)
{
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        qDebug()<<i;
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
            if (b->file_name == filename) {
                qDebug()<<" already  insert  "<<child;                //mLayout->removeItem(child);
            } else {
                ++i;
                qDebug()<<"not insert  ";
            }
        }
     saveSettings();
}

void UKUIQuickLaunch::removeButton(QuickLaunchAction* action)
{

    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    qDebug()<<"btn->file_name  *** >>>"<<btn->file_name;
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        qDebug()<<i;
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        qDebug()<<"b->file_name   ---"<<b;
            if (b->file_name == btn->file_name) {
                qDebug()<<"child   >>>"<<child;
                //mLayout->removeItem(child);
                mLayout->removeWidget(b);
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

bool UKUIQuickLaunch::AddToTaskbar(QString arg)
{
        const auto url=QUrl(arg);
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        QFileInfo fi(fileName);
        XdgDesktopFile xdg;
        //add by QuickLaunchAction(&xdg, this)
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
        saveSettings();
        return true;
}

bool UKUIQuickLaunch::CheckIfExist(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    xdg.load(fileName);
    return  checkButton(new QuickLaunchAction(&xdg, this));
}

bool UKUIQuickLaunch::RemoveFromTaskbar(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    xdg.load(fileName);
    removeButton(new QuickLaunchAction(&xdg, this));
    return true;
}

int UKUIQuickLaunch::GetPanelPosition(QString arg)
{
    qDebug()<<"GetPanelPosition panel.position"<<mPlugin->panel()->position();
    return mPlugin->panel()->position();
}

int UKUIQuickLaunch::GetPanelSize(QString arg)
{
    qDebug()<<"GetPanelPosition panel.position"<<mPlugin->panel()->position();
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
    QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn)
        return;
    mLayout->removeWidget(btn);
    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        if(*it == btn)
        {
            mVBtn.erase(it);
            break;
        }
        else
        {
            it++;
        }
    }
    btn->deleteLater();
    saveSettings();

    if (mLayout->isEmpty())
        showPlaceHolder();
    realign();
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
    hashList.removeLast();

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
    // handle method call com.kylin.security.controller.filectrl.AddToTaskbar
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddToTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    emit addtak(3);
    return out0;
}

bool FilectrlAdaptor::CheckIfExist(const QString &arg)
{
    // handle method call com.kylin.security.controller.filectrl.CheckIfExist
    bool out0;
    QMetaObject::invokeMethod(parent(), "CheckIfExist", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

bool FilectrlAdaptor::RemoveFromTaskbar(const QString &arg)
{
    // handle method call com.kylin.security.controller.filectrl.RemoveFromTaskbar
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    qDebug()<<"revove set is here"<<arg;
    return out0;
}

int FilectrlAdaptor::GetPanelPosition(const QString &arg)
{
    // handle method call com.kylin.security.controller.filectrl.RemoveFromTaskbar
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelPosition", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}

int FilectrlAdaptor::GetPanelSize(const QString &arg)
{
    // handle method call com.kylin.security.controller.filectrl.RemoveFromTaskbar
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelSize", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}
