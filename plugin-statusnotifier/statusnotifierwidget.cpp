/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "statusnotifierwidget.h"
#include <QApplication>
#include <QDBusReply>
#include <QDebug>
#include "../panel/iukuipanelplugin.h"
#include "../panel/customstyle.h"


#define UKUI_PANEL_SETTINGS              "org.ukui.panel.settings"
#define SHOW_STATUSNOTIFIER_BUTTON       "statusnotifierbutton"

StatusNotifierWidget::StatusNotifierWidget(IUKUIPanelPlugin *plugin, QWidget *parent) :
    QWidget(parent),
    mPlugin(plugin)
{
    QString dbusName = QString("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
    if (!QDBusConnection::sessionBus().registerService(dbusName))
        qDebug() << QDBusConnection::sessionBus().lastError().message();

    //一些标志位，防止realign()的反复执行占用cpu
    timecount=0;
    mHide=false;
    mShow=false;
    mLock=true;
    mRealign=true;

    mWatcher = new StatusNotifierWatcher;
    mWatcher->RegisterStatusNotifierHost(dbusName);

    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
            this, &StatusNotifierWidget::itemAdded);
    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
            this, &StatusNotifierWidget::itemRemoved);

    //在按键加入容器后进行多次刷新
    time = new QTimer(this);
    connect(time, &QTimer::timeout, this,[=] (){
        if(timecount<50){
            mRealign=true;
            mLock=true;
            realign();
            qDebug()<<timecount;
            timecount++;
        }else
            time->stop();
    });
    time->start(50);

    mBtn = new StatusNotifierPopUpButton();
    mBtn->setText("<");

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    setLayoutDirection(Qt::RightToLeft);
    realign();
    mLayout->addWidget(mBtn);

    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        gsettings = new QGSettings(id);
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==SHOW_STATUSNOTIFIER_BUTTON){
            mRealign=true;
            realign();
        }
    });
    gsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
    qDebug() << mWatcher->RegisteredStatusNotifierItems();

}

StatusNotifierWidget::~StatusNotifierWidget()
{
    delete mWatcher;
}

void StatusNotifierWidget::itemAdded(QString serviceAndPath)
{
    int slash = serviceAndPath.indexOf('/');
    QString serv = serviceAndPath.left(slash);
    QString path = serviceAndPath.mid(slash);
    StatusNotifierButton *button = new StatusNotifierButton(serv, path, mPlugin, this);
    mServices.insert(serviceAndPath, button);
    mStatusNotifierButtons.append(button);
    button->setStyle(new CustomStyle);
    connect(button, SIGNAL(switchButtons(StatusNotifierButton*,StatusNotifierButton*)), this, SLOT(switchButtons(StatusNotifierButton*,StatusNotifierButton*)));
    mRealign=true;
    mLock=true;
    realign();
}

void StatusNotifierWidget::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = mServices.value(serviceAndPath, NULL);
    if (button)
    {
        mStatusNotifierButtons.removeOne(button);
        button->deleteLater();
        mLayout->removeWidget(button);
    }
}

void StatusNotifierWidget::realign()
{
    UKUi::GridLayout *layout = qobject_cast<UKUi::GridLayout*>(mLayout);
    layout->setEnabled(false);
    IUKUIPanel *panel = mPlugin->panel();
    for(int i=0;i<mStatusNotifierButtons.size();i++){
    mStatusNotifierButtons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
    mStatusNotifierButtons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
    }
    if (panel->isHorizontal())
    {
        layout->setRowCount(panel->lineCount());
        layout->setColumnCount(0);
    }
    else
    {
        layout->setColumnCount(panel->lineCount());
        layout->setRowCount(0);
    }
    if(mRealign){
    for(int i=0;i<mStatusNotifierButtons.size();i++){
        qDebug()<<mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton();
    }
    for(int i=0;i<mStatusNotifierButtons.size();i++){
        if(mStatusNotifierButtons.at(i))
        {
            mStatusNotifierButtons.at(i)->setFixedSize(mPlugin->panel()->iconSize(),mPlugin->panel()->panelSize());
            mStatusNotifierButtons.at(i)->setIconSize(QSize(mPlugin->panel()->iconSize()/2,mPlugin->panel()->iconSize()/2));
            QStringList mStatusNotifierButtonList;
            mStatusNotifierButtonList<<"ukui-volume-control-applet-qt"<<"kylin-nm"<<"ukui-sidebar"<<"fcitx"<<"sogouimebs-qimpanel"<<"fcitx-qimpanel";
            if(!mStatusNotifierButtonList.contains(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton())){
                mStatusNotifierButtons.at(i)->setVisible(gsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool());
                hidebutton.insert(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton(),mStatusNotifierButtons.at(i));
                mHide=true;
            }
            else{
                mStatusNotifierButtons.at(i)->setVisible(true);
                //把需要固定位置的按键加入容器
                if(mLock){
                    mStatusNotifierButtons.at(i)->setVisible(true);
                    showbutton.insert(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton(), mStatusNotifierButtons.at(i));
                    mShow=true;
                }
            }

        }
        else{
            qDebug()<<"mStatusNotifierButtons add error   :  "<<mStatusNotifierButtons.at(i);
        }
    }
    //在长显示依次加入布局
    readSettings();
    if(mShow&&mHide&&mLock){
        QList<QString>::Iterator it = readappkey.begin(),itend = readappkey.end();
        int n = 0;
        for (;it != itend; it++,n++){
            if(showbutton.contains(*it)){
                StatusNotifierButton *button=showbutton[*it];
                qDebug()<<button;
                layout->addWidget(button);
                button->show();
            }
        }
        writeappkey=readappkey;
        readappkey.clear();
        //在可隐藏按键加入布局
        QHash<QString, StatusNotifierButton*>::const_iterator m;
        for(m=hidebutton.constBegin();m!=hidebutton.constEnd();++m){
           m.value()->setVisible(gsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool());
            layout->addWidget(*m);
        }
        hidebutton.clear();
        qDebug()<<showbutton;
        if(!showbutton.isEmpty())
        mLock=false;
    }
    mRealign=false;
    mLayout->addWidget(mBtn);
    }
    layout->setEnabled(true);
}

void StatusNotifierWidget::switchButtons(StatusNotifierButton *button1, StatusNotifierButton *button2)
{
    if (button1 == button2)
        return;

    int n1 = mLayout->indexOf(button1);
    int n2 = mLayout->indexOf(button2);

    int l = qMin(n1, n2);
    int m = qMax(n1, n2);

    mLayout->moveItem(l, m);
    mLayout->moveItem(m-1, l);
    //将改变的长显示的按键布局顺序写入配置文件
    writeappkey.move(l,m);
    writeappkey.move(m-1, l);
    saveSettings();
}

void StatusNotifierWidget::on_pushButton_clicked(const QString &service) {
    QDBusInterface *iface= new QDBusInterface(service
                         , "/StatusNotifierItem"
                         , "org.freedesktop.DBus.Properties"
                         , QDBusConnection::sessionBus());
    QDBusReply<QMap<QString, QVariant> > reply = iface->call("GetAll", "org.kde.StatusNotifierItem");
    if (reply.isValid()){
        QMap<QString, QVariant> propertyMap;
        propertyMap = reply.value();
        lockServices = propertyMap.find("Id").value().toString();
        qDebug()<<"rely is: "<<lockServices;

    } else {
        qDebug() << "reply failed";
    }
}

//写配置文件
void StatusNotifierWidget::saveSettings()
{
    PluginSettings *settings = mPlugin->settings();
    settings->beginGroup("statusnotifier");
    settings->endGroup();
    settings->setValue("app1",writeappkey);
}

//读配置文件
void StatusNotifierWidget::readSettings(){
    PluginSettings *settings = mPlugin->settings();
    settings->beginGroup("statusnotifier");
    settings->endGroup();
    readappkey=settings->value("app1").toStringList();
}

StatusNotifierPopUpButton::StatusNotifierPopUpButton()
{
    this->setStyle(new CustomStyle);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        gsettings = new QGSettings(id);
}

StatusNotifierPopUpButton::~StatusNotifierPopUpButton()
{

}

void StatusNotifierPopUpButton::mousePressEvent(QMouseEvent *)
{
    if(gsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool()){
        this->setText("<");
        gsettings->set(SHOW_STATUSNOTIFIER_BUTTON,false);
    }
    else{
        this->setText(">");
        gsettings->set(SHOW_STATUSNOTIFIER_BUTTON,true);
    }
}

