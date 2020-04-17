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


#ifndef UKUIQUICKLAUNCH_H
#define UKUIQUICKLAUNCH_H

#include "../panel/ukuipanel.h"
#include <QHash>
#include <QString>
#include <QMap>
#include <QVector>
#include "qlayout.h"
#include "qlayoutitem.h"
#include "qlayoutitem.h"
#include "qgridlayout.h"
#include <QPainter>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

class XdgDesktopFile;
class QuickLaunchAction;
class QDragEnterEvent;
class QuickLaunchButton;
class QSettings;
class QLabel;

namespace UKUi {
class GridLayout;
}


/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class UKUIQuickLaunch : public QFrame
{
    Q_OBJECT


public:
   UKUIQuickLaunch(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~UKUIQuickLaunch();

    int indexOfButton(QuickLaunchButton* button) const;
    int countOfButtons() const;

    void realign();
    //virtual QLayoutItem *takeAt(int index) = 0;
    void saveSettings();
    void showPlaceHolder();

    friend class FilectrlAdaptor;

private:
    UKUi::GridLayout *mLayout;
    IUKUIPanelPlugin *mPlugin;
    QLabel *mPlaceHolder;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void paintEvent(QPaintEvent *);
    QVector<QuickLaunchButton*> mVBtn;






private slots:
    void addButton(QuickLaunchAction* action);
    bool checkButton(QuickLaunchAction* action);
    void checkButton(QString *filename);
    void removeButton(QString *filename);
    void removeButton(QuickLaunchAction* action);
    void switchButtons(QuickLaunchButton *button1, QuickLaunchButton *button2);
    void buttonDeleted();
    void buttonMoveLeft();
    void buttonMoveRight();

public slots:
    bool AddToTaskbar(QString arg);
    bool RemoveFromTaskbar(QString arg);
    bool CheckIfExist(QString arg);
    int GetPanelPosition(QString arg);
    int GetPanelSize(QString arg);

};



class FilectrlAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.ukui.panel.desktop")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.ukui.panel.desktop\">\n"
"    <method name=\"AddToTaskbar\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n"
"    </method>\n"
"    <method name=\"RemoveFromTaskbar\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n"
"    </method>\n"
"    <method name=\"CheckIfExist\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n"
"    </method>\n"
"    <method name=\"GetPanelPosition\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n"
"    </method>\n"
"    <method name=\"GetPanelSize\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    FilectrlAdaptor(QObject *parent);
    virtual ~FilectrlAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    bool AddToTaskbar(const QString &arg);
    bool CheckIfExist(const QString &arg);
    bool RemoveFromTaskbar(const QString &arg);
    int GetPanelPosition(const QString &arg);
    int GetPanelSize(const QString &arg);

Q_SIGNALS: // SIGNALS

signals:
    void addtak(int);
};

#endif
