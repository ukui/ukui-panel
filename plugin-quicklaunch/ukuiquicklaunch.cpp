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

#define PANEL_SETTINGS "org.ukui.panel.settings"
#define PANEL_LINES    "panellines"

UKUIQuickLaunch::UKUIQuickLaunch(IUKUIPanelPlugin *plugin, QWidget* parent) :
    QFrame(parent),
    mPlugin(plugin),
    mPlaceHolder(0)
{
    setAcceptDrops(true);
    mVBtn.clear();

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);

    const QByteArray id(PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id)){
        settings=new QGSettings(id);
    }
    connect(settings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==PANEL_LINES)
            realign();
    });

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;

    //gsetting的方式读取写入 apps
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

//    if (mLayout->isEmpty())
//        showPlaceHolder();

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

/*快速启动栏上应用的数量*/
int UKUIQuickLaunch::countOfButtons() const
{
    return mLayout->count();
}

/* 快速启动栏的实时调整函数，
*/
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
            if(settings->get(PANEL_LINES).toInt()==1)
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
            if(settings->get(PANEL_LINES).toInt()==1)
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

    /* 仅仅在快速启动栏上的应用数量大于０的时候才进行判断
     * 若在快速启动栏　应用数量为０的时候b->file_name为空
     * 会造成任务栏的崩溃
      */
    qDebug()<<"检测到目前已经固定到任务栏的应用数量 "<<countOfButtons();
    if(countOfButtons()>0){
        while ((child = mLayout->layout()->itemAt(i))) {
            QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
            qDebug()<<"mLayout->itemAt(i) ";
            if (b->file_name == btn->file_name){
                checkresult=true;
                break;
            }
            else {
                checkresult=false;
                ++i;
            }
        }
                return checkresult;
    }
    else{
        qDebug()<<"countOfButtons =0  "<<countOfButtons();
        return false;
    }
    delete btn;
}

/* 以xdg的方式删除　button*/
void UKUIQuickLaunch::removeButton(QuickLaunchAction* action)
{
    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    int i = 0;
    QLayoutItem *child;

    while ((child = mLayout->layout()->itemAt(i))) {
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if (b->file_name == btn->file_name) {
            for(auto it = mVBtn.begin();it != mVBtn.end();it++)
            {
                if(*it == b)
                {
                    mVBtn.erase(it);
                    break;
                }
            }

            mLayout->removeItem(child);
            mLayout->removeWidget(b);
            b->deleteLater();
            btn->deleteLater();
        } else {
            ++i;
        }
    }

    //    btn->deleteLater();
    realign();
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

bool UKUIQuickLaunch::AddToTaskbar(QString arg)
{
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
 */
bool UKUIQuickLaunch::CheckIfExist(QString arg)
{
    if(countOfButtons()>0)
    {
        const auto url=QUrl(arg);
        QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
        XdgDesktopFile xdg;
        xdg.load(fileName);
        bool state;
        state=checkButton(new QuickLaunchAction(&xdg, this));
        return state;
    }
}

/*为开始菜单提供从任务栏上移除的接口*/
bool UKUIQuickLaunch::RemoveFromTaskbar(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    XdgDesktopFile xdg;
    xdg.load(fileName);
    removeButton(new QuickLaunchAction(&xdg, this));
    return true;
}

/*获取任务栏位置的接口*/
int UKUIQuickLaunch::GetPanelPosition(QString arg)
{
    return mPlugin->panel()->position();
}

/*获取任务栏高度的接口*/
int UKUIQuickLaunch::GetPanelSize(QString arg)
{
    return mPlugin->panel()->panelSize();
}

/**/
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

/*快速启动栏的按钮删除 右键删除*/
void UKUIQuickLaunch::buttonDeleted()
{
    QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn)
        return;
    for(auto it = mVBtn.begin();it != mVBtn.end();it++)
    {

        if(*it == btn)
        {
            mVBtn.erase(it);
            break;
        }
    }

    btn->deleteLater();
    mLayout->removeWidget(btn);
    saveSettings();

    /*//注释showPlaceHolder的原因是在开始菜单检测快速启动栏上面固定的应用数量的时候
      //countOfButtons无法获取快速启动栏上的应用为０的情况
    if (mLayout->isEmpty()){
        qDebug()<<"mLayout->isEmpty()"<<countOfButtons();
        showPlaceHolder();
    }
    */
    realign();
}

/*快速启动栏应用右键左移函数*/
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

/*快速启动栏右键　右移函数*/
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

/*保持设置*/
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

/*在快速启动栏区域没有应用的时候显示一块空白的区域用以实现拖拽等操作
*/
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
 * 为开始菜单提供D_Bus接口
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

/*添加到快速启动栏*/
bool FilectrlAdaptor::AddToTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddToTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*检测是否已经存在于快速启动栏*/
bool FilectrlAdaptor::CheckIfExist(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "CheckIfExist", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*从快速启动栏删除应用*/
bool FilectrlAdaptor::RemoveFromTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
    return out0;
}

/*获取任务栏位置*/
int FilectrlAdaptor::GetPanelPosition(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelPosition", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}

/*获取任务栏高度*/
int FilectrlAdaptor::GetPanelSize(const QString &arg)
{
    int out0;
    QMetaObject::invokeMethod(parent(), "GetPanelSize", Q_RETURN_ARG(int, out0), Q_ARG(QString, arg));
    return out0;
}
