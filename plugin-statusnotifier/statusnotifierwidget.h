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

#ifndef STATUSNOTIFIERWIDGET_H
#define STATUSNOTIFIERWIDGET_H

#include <QDir>
#include <QGSettings/QGSettings>
#include <QTimer>
#include <QEvent>

#include "../panel/common/ukuigridlayout.h"
#include "../panel/iukuipanelplugin.h"
#include "../panel/common/ukuisettings.h"
#include "../panel/pluginsettings.h"

#include "statusnotifierbutton.h"
#include "statusnotifierwatcher.h"
class StatusNotifierPopUpButton;
class StatusNotifierWidget : public QWidget
{
    Q_OBJECT

public:
    StatusNotifierWidget(IUKUIPanelPlugin *plugin, QWidget *parent = 0);
    ~StatusNotifierWidget();

signals:

public slots:
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);

    void realign();

private:
    UKUi::GridLayout *mLayout;
    IUKUIPanelPlugin *mPlugin;
    StatusNotifierWatcher *mWatcher;

    QHash<QString, StatusNotifierButton*> mServices;
    QMap<QString,StatusNotifierButton*> m_ShowButtons;
    QMap<QString,StatusNotifierButton*> m_HideButtons;
    QMap<QString,StatusNotifierButton*> m_AllButtons;

    QList<StatusNotifierButton*> mStatusNotifierButtons;
    QToolButton *mBtn;
    QGSettings *gsettings;
    QTimer *time;
    int timecount;
    bool mRealign;

private:
    void saveSettings(QString button1,QString button2);
    QList<QStringList> readSettings();


private slots:
    void switchButtons(StatusNotifierButton *button1, StatusNotifierButton *button2);
    void btnAddButton(QString button);
};

class StatusNotifierPopUpButton : public QToolButton
{
    Q_OBJECT
public:
    StatusNotifierPopUpButton(StatusNotifierWidget *parent = nullptr);
    ~StatusNotifierPopUpButton();
protected:
    void mousePressEvent(QMouseEvent *);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private:
    QGSettings *gsettings;

signals:
    void addButton(QString button);

};

#endif // STATUSNOTIFIERWIDGET_H
