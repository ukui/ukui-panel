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

    mWatcher = new StatusNotifierWatcher;
    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
            this, &StatusNotifierWidget::itemAdded);
    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
            this, &StatusNotifierWidget::itemRemoved);

    mBtn = new StatusNotifierStorageArrow(this);
    connect(mBtn,SIGNAL(addButton(QString)),this,SLOT(btnAddButton(QString)));

    mLayout = new UKUi::GridLayout(this);
    setLayout(mLayout);
    mLayout->addWidget(mBtn);
    const QByteArray id(UKUI_PANEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(id))
        gsettings = new QGSettings(id);
    connect(gsettings, &QGSettings::changed, this, [=] (const QString &key){
        if(key==SHOW_STATUSNOTIFIER_BUTTON){
            exchangeHideAndShow();
        }
    });

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

    //dbus异步调用，同步执行获取不到托盘名字，通过定时器进行异步刷新，后期可进行优化
    for(int i=0;i<mStatusNotifierButtons.size();i++){
        if(mStatusNotifierButtons.at(i))
        {
            if(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton()==""){
                continue;
            }
        }
        else{
            qDebug()<<"mStatusNotifierButtons add error   :  "<<mStatusNotifierButtons.at(i);
        }
    }

    QTimer::singleShot(200,this,[=](){resetLayout();});

}

void StatusNotifierWidget::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = mServices.value(serviceAndPath, NULL);
    if (button)
    {
        mStatusNotifierButtons.removeOne(button);
        mLayout->removeWidget(button);
        if(m_ShowButtons.keys().contains(button->hideAbleStatusNotifierButton())){
            m_ShowButtons.remove(button->hideAbleStatusNotifierButton());
        }
        if(m_HideButtons.keys().contains(button->hideAbleStatusNotifierButton())){
            m_HideButtons.remove(button->hideAbleStatusNotifierButton());
        }
        m_AllButtons.remove(button->hideAbleStatusNotifierButton());
        resetLayout();
        button->deleteLater();
    }
}

void StatusNotifierWidget::realign()
{
    UKUi::GridLayout *layout = qobject_cast<UKUi::GridLayout*>(mLayout);
    layout->setEnabled(false);
    layout->setDirection(UKUi::GridLayout::LeftToRight);
    IUKUIPanel *panel = mPlugin->panel();

    if (panel->isHorizontal())
    {
        layout->setRowCount(panel->lineCount());
        layout->setColumnCount(0);
        layout->setCellFixedSize(QSize(panel->panelSize()*0.7,panel->panelSize()));
    }
    else
    {
        layout->setColumnCount(panel->lineCount());
        layout->setRowCount(0);
        layout->setCellFixedSize(QSize(panel->panelSize(),panel->panelSize()*0.7));
    }
    Direction=panel->isHorizontal();
    layout->setEnabled(true);
}

void StatusNotifierWidget::resetLayout(){
    QStringList show=readSettings().at(0);
    show.removeAll("");
    QStringList hide=readSettings().at(1);
    hide.removeAll("");
    for(int i=0;i<mStatusNotifierButtons.size();i++){
        if(mStatusNotifierButtons.at(i))
        {
            m_AllButtons.insert(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton().toUtf8(),mStatusNotifierButtons.at(i));
            if((!show.contains(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton()))&&(!hide.contains(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton()))){
                if(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton()==""){
                    continue;
                }
                hide.append(mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton());
                saveSettings("",mStatusNotifierButtons.at(i)->hideAbleStatusNotifierButton());
                continue;
            }
        }
        else{
            qDebug()<<"mStatusNotifierButtons add error   :  "<<mStatusNotifierButtons.at(i);
        }
    }

    for(int i=0;i<hide.size();i++){
        if(!m_AllButtons.value(hide.at(i))==NULL){
            m_AllButtons.value(hide.at(i))->setVisible(gsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool());
            mLayout->addWidget(m_AllButtons.value(hide.at(i)));
            m_HideButtons.insert(hide.at(i),m_AllButtons.value(hide.at(i)));
        }
    }
    mLayout->addWidget(mBtn);
    for(int i=0;i<show.size();i++){
        if(!m_AllButtons.value(show.at(i))==NULL){
            if(m_AllButtons.keys().contains(show.at(i))){
                mLayout->addWidget(m_AllButtons.value(show.at(i)));
                m_ShowButtons.insert(show.at(i),m_AllButtons.value(show.at(i)));
            }
        }
    }
    mLayout->setEnabled(true);
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

    if(!(m_HideButtons.keys().contains(button1->hideAbleStatusNotifierButton())&&m_HideButtons.keys().contains(button2->hideAbleStatusNotifierButton()))){
        m_HideButtons.remove(button1->hideAbleStatusNotifierButton());
    }
    if(!(m_ShowButtons.keys().contains(button1->hideAbleStatusNotifierButton())&&m_ShowButtons.keys().contains(button2->hideAbleStatusNotifierButton()))){
        m_ShowButtons.remove(button1->hideAbleStatusNotifierButton());
    }
    saveSettings(button1->hideAbleStatusNotifierButton(),button2->hideAbleStatusNotifierButton());
    resetLayout();
}

void StatusNotifierWidget::saveSettings(QString button1,QString button2){

    PluginSettings *settings=mPlugin->settings();
    QStringList showApp=settings->value("showApp").toStringList();
    QStringList hideApp=settings->value("hideApp").toStringList();

    if(button2==NULL){
        if(m_HideButtons.keys().contains(button1)){
            m_HideButtons.remove(button1);
        }
        if(m_ShowButtons.keys().contains(button1)){
            m_ShowButtons.remove(button1);
        }
        if(m_HideButtons.keys().isEmpty()){
            hideApp.append(button1);
            if(showApp.contains(button1)){
                showApp.removeAll(button1);
            }
        }
        if(m_ShowButtons.keys().isEmpty()){
            showApp.append(button1);
            if(hideApp.contains(button1)){
                hideApp.removeAll(button1);
            }
        }
        settings->setValue("showApp",showApp);
        settings->setValue("hideApp",hideApp);
        return;
    }

    if(button1==NULL){
        if(!button2.isNull()){
        hideApp.append(button2);
        hideApp.removeAll("");
        settings->setValue("hideApp",hideApp);
        return;
        }
    }

    if(showApp.contains(button1)&&showApp.contains(button2)){
        int tep=showApp.indexOf(button1);
        showApp.replace(showApp.indexOf(button2),button1);
        showApp.replace(tep,button2);
        settings->setValue("showApp",showApp);
    }

    if(showApp.contains(button1)&&hideApp.contains(button2)){
        hideApp.insert(hideApp.indexOf(button2),button1);
        showApp.removeAll(button1);
        settings->setValue("showApp",showApp);
        settings->setValue("hideApp",hideApp);
    }


    if(hideApp.contains(button1)&&showApp.contains(button2)){
        showApp.insert(showApp.indexOf(button2),button1);
        hideApp.removeAll(button1);
        settings->setValue("showApp",showApp);
        settings->setValue("hideApp",hideApp);
    }
    if(hideApp.contains(button1)&&hideApp.contains(button2)){
        int tep=hideApp.indexOf(button1);
        hideApp.replace(hideApp.indexOf(button2),button1);
        hideApp.replace(tep,button2);
        settings->setValue("hideApp",hideApp);
    }
}

QList<QStringList> StatusNotifierWidget::readSettings(){
    PluginSettings *settings=mPlugin->settings();
    QStringList showApp=settings->value("showApp").toStringList();
    QStringList hideApp=settings->value("hideApp").toStringList();
    QList<QStringList> list;
    list.append(showApp);
    list.append(hideApp);
    return list;
}

void StatusNotifierWidget::exchangeHideAndShow(){
    QMap<QString, StatusNotifierButton*>::const_iterator i;
    for(i=m_HideButtons.constBegin();i!=m_HideButtons.constEnd();++i){
        i.value()->setVisible(gsettings->get(SHOW_STATUSNOTIFIER_BUTTON).toBool());
    }
}


void StatusNotifierWidget::btnAddButton(QString button){
    saveSettings(button,"");
    resetLayout();
}
