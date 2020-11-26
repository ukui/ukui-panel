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
#include <QDir>
#include <QFileInfo>
#include <QTableWidget>
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QPushButton>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include "json.h"
using namespace  std;
using QtJson::JsonObject;
using QtJson::JsonArray;

#define PAGEBUTTON_SMALL_SIZE  20
#define PAGEBUTTON_MEDIUM_SIZE 30
#define PAGEBUTTON_LARGE_SIZE  40
#define PANEL_SMALL_SIZE  0
#define PANEL_MEDIUM_SIZE 1
#define PANEL_LARGE_SIZE  2
#
#define PANEL_SETTINGS "org.ukui.panel.settings"
#define PANEL_LINES    "panellines"

UKUIQuickLaunch::UKUIQuickLaunch(IUKUIPanelPlugin *plugin, QWidget* parent) :
    QFrame(parent),
    mPlugin(plugin),
    mPlaceHolder(0)
{

    struct passwd *pwd;
    pwd=getpwuid(getuid());
    pwd->pw_name;
    SecurityConfigPath=QString("/home/")+pwd->pw_name+QString("/.config/ukui-panel-security-config.json");

    setAcceptDrops(true);
    mVBtn.clear();

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);


    const QByteArray id(PANEL_SETTINGS);
    tmpwidget = new QWidget(this);
    QVBoxLayout *_style = new QVBoxLayout(tmpwidget);

    pageup = new QToolButton(this);
    pagedown = new QToolButton(this);
    QStyle *style = new CustomStyle;
    pageup->setStyle(style);
    pagedown->setStyle(style);
    pageup->setText("∧");
    pagedown->setText("∨");
    style->deleteLater();


    _style->addWidget(pageup, 0,Qt::AlignTop|Qt::AlignHCenter);
    _style->addWidget(pagedown,0,Qt::AlignHCenter);
    _style->setContentsMargins(0,1,0,10);
    //tmpwidget->setFixedSize(24,mPlugin->panel()->panelSize());
    if(QGSettings::isSchemaInstalled(id)){
        settings=new QGSettings(id);
    }
    apps_number = settings->get("quicklaunchappsnumber").toInt();
    GetMaxPage();
    old_page = page_num;

    connect(pageup,SIGNAL(clicked()),this,SLOT(PageUp()));
    connect(pagedown,SIGNAL(clicked()),this,SLOT(PageDown()));
    connect(settings, &QGSettings::changed, this, [=] (const QString &key){
        if (key == "quicklaunchappsnumber") {
            apps_number = settings->get("quicklaunchappsnumber").toInt();
            realign();
        }
        if(key==PANEL_LINES)
        {
            realign();
            mLayout->removeWidget(tmpwidget);
            mLayout->addWidget(tmpwidget);
            mLayout->removeWidget(mPlaceHolder);
        }
    });
    refreshQuickLaunch("init");
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), "org.freedesktop.kylinssoclient.interface", "keyChanged", this, SLOT(refreshQuickLaunch(QString)));

    /*监听系统应用的目录以及安卓兼容应用的目录*/
    fsWatcher=new QFileSystemWatcher(this);
    fsWatcher->addPath(desktopFilePath);
    fsWatcher->addPath(androidDesktopFilePath);
    connect(fsWatcher,&QFileSystemWatcher::directoryChanged,[this](){
               directoryUpdated(desktopFilePath);
               directoryUpdated(androidDesktopFilePath);
            });

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

void UKUIQuickLaunch::ReloadSecurityConfig(){
    this->loadJsonfile();
    this->refreshQuickLaunch("ukui-panel2");
}

QString UKUIQuickLaunch::GetSecurityConfigPath(){
    return SecurityConfigPath;
}
QString UKUIQuickLaunch::readFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

void UKUIQuickLaunch::loadJsonfile() {
    QString json = readFile(SecurityConfigPath);
    if (json.isEmpty()) {
        qFatal("Could not read JSON file!");
        return ;
    }
    bool ok;
    JsonObject result = QtJson::parse(json, ok).toMap();
    QVariant fristLayer;
    fristLayer=result.value("ukui-panel");
    mModel=fristLayer.toMap().value("mode").toString();

    QVariant blacklistLayer;
    blacklistLayer=fristLayer.toMap().value("blacklist");
    QVariant whitelistLayer;
    whitelistLayer=fristLayer.toMap().value("whitelist");

    if(!blacklistLayer.isNull()){
        QList<QVariant> thirdLayer;
        thirdLayer=blacklistLayer.toList();
        QMap<QString,QVariant> fourthLayer;
        fourthLayer=thirdLayer.at(0).toMap();
        QList<QVariant> fifthLayer;
        fifthLayer=fourthLayer.value("entries").toList();
        QMap<QString,QVariant> attribute;
        QList<QString> blackNames;
        for(int i=0;i<fifthLayer.size();i++){
            attribute=fifthLayer.at(i).toMap();
            blackNames.append(attribute.value("path").toString());
        }
        blacklist=blackNames;
    }

    if(!whitelistLayer.isNull()){
        QList<QVariant> thirdLayer;
        thirdLayer=whitelistLayer.toList();
        QMap<QString,QVariant> fourthLayer;
        fourthLayer=thirdLayer.at(0).toMap();
        QList<QVariant> fifthLayer;
        fifthLayer=fourthLayer.value("entries").toList();
        QMap<QString,QVariant> attribute;
        QList<QString> whiteNames;
        for(int i=0;i<fifthLayer.size();i++){
            attribute=fifthLayer.at(i).toMap();
            whiteNames.append(attribute.value("path").toString());
        }
        whitelist=whiteNames;
    }
}

/*任务栏刷新  在快读启动栏初始化和云账户同步的时候调用*/
void UKUIQuickLaunch::refreshQuickLaunch(QString ssoclient){
    if(ssoclient != "ukui-panel2" && ssoclient != "init")
        return;
    QStringList mblacklist;
    QStringList mwhitelist;
    if(mModel=="blacklist")
    mblacklist=blacklist;
    if(mModel=="whitelist")
    mwhitelist=whitelist;
    for(auto it = mVBtn.begin(); it != mVBtn.end();)
    {
        (*it)->deleteLater();
        mVBtn.erase(it);
    }

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;
    //qsetting的方式读取写入 apps
    const auto apps = mPlugin->settings()->readArray("apps");
    for (const QMap<QString, QVariant> &app : apps)
    {
        desktop = app.value("desktop", "").toString();

        if(mblacklist.contains(desktop)){
            desktop.clear();
        }
        if(mModel=="whitelist"){
            if(!mwhitelist.contains(desktop)){
                desktop.clear();
            }
        }

        file = app.value("file", "").toString();
        if (!desktop.isEmpty())
        {
            XdgDesktopFile xdg;
            if (!xdg.load(desktop))
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not valid";
                continue;
            }
            /* 检测desktop文件的属性，目前UKUI桌面环境不需要此进行isSuitable检测
            if (!xdg.isSuitable())
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not applicable";
                continue;
            }
            */
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
            //addButton(new QuickLaunchAction(execname, exec, icon, this));
        }
    }
    int i = 0;
    int counts = countOfButtons();
    int shows = (counts < apps_number ? counts : apps_number);
    while (i != counts && shows && counts >0) {
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if (shows) {
            b->setHidden(0);
            --shows;
        } else {
            b->setHidden(1);
        }
        ++i;
    }
    realign();
    mLayout->addWidget(tmpwidget);
}

void UKUIQuickLaunch::PageUp() {
    --page_num;
    if (page_num < 1) page_num = max_page;
    old_page = page_num;
    realign();
}

void UKUIQuickLaunch::PageDown() {
    ++page_num;
    if (page_num > max_page) page_num = 1;
    old_page = page_num;
    realign();
}

void UKUIQuickLaunch::GetMaxPage() {
    if (mPlugin->panel()->isHorizontal()) {
        int btn_cnt = countOfButtons();
        max_page = (int)(btn_cnt / apps_number);
        if (btn_cnt % apps_number != 0) max_page += 1;
    } else if (mPlugin->panel()->isMaxSize()){
        int btn_cnt = countOfButtons();
        max_page = (int)(btn_cnt / 2);
        if (btn_cnt % 2 != 0) max_page += 1;
    } else {
        int btn_cnt = countOfButtons();
        max_page = (int)(btn_cnt / 3);
        if (btn_cnt % 3 != 0) max_page += 1;
    }
    if (page_num > max_page && max_page) page_num = max_page;
}

int UKUIQuickLaunch::indexOfButton(QuickLaunchButton* button) const
{
    return mLayout->indexOf(button);
}

/*快速启动栏上应用的数量*/
int UKUIQuickLaunch::countOfButtons() const
{
    return mLayout->count() - 1;
}

/* 快速启动栏的实时调整函数，
*/
void UKUIQuickLaunch::realign()
{
    GetMaxPage();
    int counts = countOfButtons();
    int i = 0;
    int loop_times = 0;
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
            i = (page_num - 1) * apps_number;
            loop_times = apps_number;
            if (counts < apps_number) loop_times = counts - i;
            setMaximumWidth(mPlugin->panel()->panelSize() * apps_number + 27);
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
            if (countOfButtons() <= apps_number) {
                tmpwidget->setHidden(1);
            } else {
                tmpwidget->setHidden(0);
            }

        }
        else
        {
            if (mPlugin->panel()->isMaxSize()) {
                i = (page_num - 1) * 2;
                loop_times = 2;
                if (counts < 2) loop_times = counts - i;
                setMaximumHeight(mPlugin->panel()->panelSize() * 2 + 40);
            } else {
                i = (page_num - 1) * 3;
                loop_times = 3;
                if (counts < 3) loop_times = counts - i;
                setMaximumHeight(mPlugin->panel()->panelSize() * 3 + 27);
            }
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

            if (countOfButtons() <= 3) {
                tmpwidget->setHidden(1);
            } else {
                tmpwidget->setHidden(0);
            }
            if (countOfButtons() > 2 && panel->isMaxSize())
                tmpwidget->setHidden(0);
        }
    }
    mLayout->setEnabled(true);
    switch(mPlugin->panel()->sizeModel()) {
        case PANEL_SMALL_SIZE :
            pageup->setFixedSize(PAGEBUTTON_SMALL_SIZE, PAGEBUTTON_SMALL_SIZE);
            pagedown->setFixedSize(PAGEBUTTON_SMALL_SIZE, PAGEBUTTON_SMALL_SIZE);
            break;
        case PANEL_MEDIUM_SIZE :
            pageup->setFixedSize(PAGEBUTTON_MEDIUM_SIZE, PAGEBUTTON_MEDIUM_SIZE);
            pagedown->setFixedSize(PAGEBUTTON_MEDIUM_SIZE, PAGEBUTTON_MEDIUM_SIZE);
            break;
        case PANEL_LARGE_SIZE :
            pageup->setFixedSize(PAGEBUTTON_LARGE_SIZE, PAGEBUTTON_LARGE_SIZE);
            pagedown->setFixedSize(PAGEBUTTON_LARGE_SIZE, PAGEBUTTON_LARGE_SIZE);
            break;
    }
    for(auto it = mVBtn.begin();it != mVBtn.end();it++) {
        if (!i && loop_times) {
            (*it)->setHidden(0);
            --loop_times;
        } else {
            (*it)->setHidden(1);
            --i;
        }
    }
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
    if (countOfButtons() > apps_number) btn->setHidden(1);
    connect(btn, SIGNAL(switchButtons(QuickLaunchButton*,QuickLaunchButton*)), this, SLOT(switchButtons(QuickLaunchButton*,QuickLaunchButton*)));
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(movedLeft()), this, SLOT(buttonMoveLeft()));
    connect(btn, SIGNAL(movedRight()), this, SLOT(buttonMoveRight()));

    mLayout->removeWidget(mPlaceHolder);
    mPlaceHolder->deleteLater();
    mPlaceHolder = NULL;
    mLayout->setEnabled(true);
   // GetMaxPage();
    realign();
    mLayout->removeWidget(tmpwidget);
    mLayout->addWidget(tmpwidget);
}

bool UKUIQuickLaunch::checkButton(QuickLaunchAction* action)
{
    bool checkresult;
    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    int i = 0;
    int counts = countOfButtons();

    /* 仅仅在快速启动栏上的应用数量大于０的时候才进行判断
     * 若在快速启动栏　应用数量为０的时候b->file_name为空
     * 会造成任务栏的崩溃
      */
    qDebug()<<"检测到目前已经固定到任务栏的应用数量 "<<countOfButtons();
    if(countOfButtons()>0){
        while (i != counts) {
            QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
            qDebug()<<"mLayout->itemAt("<<i<<") ";
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

/* 删除　button*/
void UKUIQuickLaunch::removeButton(QuickLaunchAction* action)
{
    QuickLaunchButton* btn = new QuickLaunchButton(action, mPlugin, this);
    int i = 0;
    int flag = 1;
    int counts = countOfButtons();
     while (i != counts && flag)
     {
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if (b->file_name == btn->file_name) {
            for(auto it = mVBtn.begin();it != mVBtn.end();it++)
            {
                if(*it == b)
                {
                    mVBtn.erase(it);
                    flag = 0;
                    break;
                }
            }
            mLayout->removeWidget(b);
        } else {
            ++i;
        }
     }
    // GetMaxPage();
    //    btn->deleteLater();
    realign();

    if (old_page != page_num) {
        old_page = page_num;
        PageUp();
    }
    saveSettings();
    btn->deleteLater();
    mLayout->removeWidget(tmpwidget);
    mLayout->addWidget(tmpwidget);
    mLayout->removeWidget(mPlaceHolder);
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
        QuickLaunchAction *_action = NULL;
        if (!fileName.compare("computer:///"))
            fileName = QString("/usr/share/applications/peony-computer.desktop");
        if (!fileName.compare("trash:///"))
            fileName = QString("/usr/share/applications/peony-trash.desktop");
        if (CheckIfExist(fileName)) return;
        if (xdg.load(fileName))
        {
            if (xdg.isSuitable())
                _action = new QuickLaunchAction(&xdg, this);
        }
        else if (fi.exists() && fi.isExecutable() && !fi.isDir())
        {
            _action = new QuickLaunchAction(fileName, fileName, "", this);
        }
        else if (fi.exists())
        {
            _action = new QuickLaunchAction(fileName, this);
        }
        else
        {
            qWarning() << "XdgDesktopFile" << fileName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                                     tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                                     );
        }
        if (_action)
            addButton(_action);
    }
    saveSettings();
}

// 只要任何监控的目录更新（添加、删除、重命名），就会调用。
void UKUIQuickLaunch::directoryUpdated(const QString &path)
{
    // 比较最新的内容和保存的内容找出区别(变化)
    QStringList currEntryList = m_currentContentsMap[path];
    const QDir dir(path);
    QStringList newEntryList = dir.entryList(QDir::NoDotAndDotDot  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    QSet<QString> newDirSet = QSet<QString>::fromList(newEntryList);
    QSet<QString> currentDirSet = QSet<QString>::fromList(currEntryList);

    // 添加了文件
    QSet<QString> newFiles = newDirSet - currentDirSet;
    QStringList newFile = newFiles.toList();

    // 文件已被移除
    QSet<QString> deletedFiles = currentDirSet - newDirSet;
    QStringList deleteFile = deletedFiles.toList();

    // 更新当前设置
    m_currentContentsMap[path] = newEntryList;

    if (!newFile.isEmpty() && !deleteFile.isEmpty())
    {
        // 文件/目录重命名
        if ((newFile.count() == 1) && (deleteFile.count() == 1))
        {
//            qDebug() << QString("File Renamed from %1 to %2").arg(deleteFile.first()).arg(newFile.first());
        }
    }
    else
    {
        // 添加新文件/目录至Dir
        if (!newFile.isEmpty())
        {
            foreach (QString file, newFile)
            {
                // 处理操作每个新文件....
            }
        }
        // 从Dir中删除文件/目录
        if (!deleteFile.isEmpty())
        {
            foreach(QString file, deleteFile)
            {
                // 处理操作每个被删除的文件....
                FileDeleteFromTaskbar(path+file);
            }
        }
    }
}

bool UKUIQuickLaunch::AddToTaskbar(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    QFileInfo fi(fileName);
    XdgDesktopFile xdg;
    if (CheckIfExist(fileName)) return false;
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
        QuickLaunchAction *_action = new QuickLaunchAction(&xdg, this);
        state=checkButton(_action);
        _action->deleteLater();
        return state;
    }
    return false;
}

bool UKUIQuickLaunch::pubCheckIfExist(QString name) {
    return CheckIfExist(name);
}

/*为开始菜单提供从任务栏上移除的接口*/
bool UKUIQuickLaunch::RemoveFromTaskbar(QString arg)
{
    const auto url=QUrl(arg);
    QString fileName(url.isLocalFile() ? url.toLocalFile() : url.url());
    XdgDesktopFile xdg;
    xdg.load(fileName);
    QuickLaunchAction *_action = new QuickLaunchAction(&xdg, this);
    removeButton(_action);
    return true;
}

/*从任务栏上移除文件的接口*/
void UKUIQuickLaunch::FileDeleteFromTaskbar(QString file)
{
    int i=0;
    bool flag = true;
    while (i < countOfButtons() && flag)
    {
        qDebug()<<i;
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(i)->widget());
        if(NULL == b){
            qDebug()<<"Quicklaunch  Error : find a QuickLaunchButton is empty ";
            break;
        }
        else{
            if(QString::compare(file,b->file_name)==0)
            {
                for(auto it = mVBtn.begin();it != mVBtn.end();it++){
                    if(*it == b){
                        mVBtn.erase(it);
                        flag = false;
                        break;
                    }
                }
                mLayout->removeWidget(b);
            }
        }
        i++;
    }
    saveSettings();
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
    if (countOfButtons() == apps_number) PageUp();
    //GetMaxPage();
    realign();
    if (old_page != page_num) {
        old_page = page_num;
        PageUp();
    }
    saveSettings();
    mLayout->removeWidget(tmpwidget);
    mLayout->addWidget(tmpwidget);
    mLayout->removeWidget(mPlaceHolder);

    /*//注释showPlaceHolder的原因是在开始菜单检测快速启动栏上面固定的应用数量的时候
      //countOfButtons无法获取快速启动栏上的应用为０的情况
    if (mLayout->isEmpty()){
        qDebug()<<"mLayout->isEmpty()"<<countOfButtons();
        showPlaceHolder();
    }
    */
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
        mVBtn.move(index, index - 1);
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
        mVBtn.move(index, index + 1);
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

/*从快速启动栏删除应用*/
bool FilectrlAdaptor::FileDeleteFromTaskbar(const QString &arg)
{
    bool out0;
    QMetaObject::invokeMethod(parent(), "FileDeleteFromTaskbar", Q_RETURN_ARG(bool, out0), Q_ARG(QString, arg));
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

void FilectrlAdaptor::ReloadSecurityConfig()
{
    QMetaObject::invokeMethod(parent(), "ReloadSecurityConfig");
}

QString FilectrlAdaptor::GetSecurityConfigPath()
{
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetSecurityConfigPath", Q_RETURN_ARG(QString, out0));
    return out0;
}
