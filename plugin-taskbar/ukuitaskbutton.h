/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef UKUITASKBUTTON_H
#define UKUITASKBUTTON_H

#include <QGSettings/QGSettings>
#include <QToolButton>
#include <QProxyStyle>
#include <QLabel>
#include "../panel/iukuipanel.h"
//#include <QWinThumbnailToolBar>
#include <QtX11Extras/qtx11extrasversion.h>
#include "../panel/ukuicontrolstyle.h"

#include "quicklaunchaction.h"
#include <QMimeData>
#include "../panel/customstyle.h"
#include <QStyleOption>
#include <QGSettings>
#include <QPainter>
#include "popupmenu.h"

class QPainter;
class QPalette;
class QMimeData;
class UKUITaskGroup;
class UKUITaskBar;
class IUKUIPanelPlugin;
class QuicklaunchMenu:public QMenu
{
public:
    QuicklaunchMenu();
    ~QuicklaunchMenu();
protected:
    void contextMenuEvent(QContextMenuEvent*);

};
class LeftAlignedTextStyle : public QProxyStyle
{
    using QProxyStyle::QProxyStyle;
public:

    virtual void drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole = QPalette::NoRole) const override;
};


class UKUITaskButton : public QToolButton
{
    Q_OBJECT

    Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

public:
    explicit UKUITaskButton(QString appName,const WId window, UKUITaskBar * taskBar, QWidget *parent = 0);
    UKUITaskButton(QuickLaunchAction * act, IUKUIPanelPlugin * plugin, QWidget* parent = 0);
    virtual ~UKUITaskButton();

    bool isApplicationHidden() const;
    bool isApplicationActive() const;
    WId windowId() const { return mWindow; }

    bool hasUrgencyHint() const { return mUrgencyHint; }
    void setUrgencyHint(bool set);

    bool isOnDesktop(int desktop) const;
    bool isOnCurrentScreen() const;
    bool isMinimized() const;
    void updateText();

    Qt::Corner origin() const;
    virtual void setAutoRotation(bool value, IUKUIPanel::Position position);

    UKUITaskBar * parentTaskBar() const {return mParentTaskBar;}

    void refreshIconGeometry(QRect const & geom);
    static QString mimeDataFormat() { return QLatin1String("ukui/UKUITaskButton"); }
    /*! \return true if this buttom received DragEnter event (and no DragLeave event yet)
     * */
    bool hasDragAndDropHover() const;

    /////////////////////////////////
    QHash<QString,QString> settingsMap();
    QString file_name;
    QString file;
    QString name;
    QString exec;

    void toDomodifyQuicklaunchMenuAction(bool direction) { modifyQuicklaunchMenuAction(direction);}

public slots:
    void raiseApplication();
    void minimizeApplication();
    void maximizeApplication();
    void deMaximizeApplication();
    void shadeApplication();
    void unShadeApplication();
    void closeApplication();
    void moveApplicationToDesktop();
    void moveApplication();
    void resizeApplication();
    void setApplicationLayer();
    void setOrigin(Qt::Corner);

    void updateIcon();

    //////

    void selfRemove();
    void this_customContextMenuRequested(const QPoint & pos);


protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void paintEvent(QPaintEvent *);

    void setWindowId(WId wid) {mWindow = wid;}
    virtual QMimeData * mimeData();
    static bool sDraggging;

    inline IUKUIPanelPlugin * plugin() const { return mPlugin; }

    /////////////////////////////////////
    //virtual QMimeData * mimeData();

private:
    bool statFlag = true;

    WId mWindow;
    QString mAppName;
    bool mUrgencyHint;
    QPoint mDragStartPosition;
    Qt::Corner mOrigin;
    QPixmap mPixmap;
    bool mDrawPixmap;
    UKUITaskBar * mParentTaskBar;
    IUKUIPanelPlugin * mPlugin;
    enum TaskButtonStatus{NORMAL, HOVER, PRESS};
    TaskButtonStatus taskbuttonstatus;
    QIcon mIcon;

    // Timer for when draggind something into a button (the button's window
    // must be activated so that the use can continue dragging to the window
    QTimer * mDNDTimer;
    QGSettings *gsettings;


    ///////////////////////////////////
    QuickLaunchAction *mAct;
    QAction *mDeleteAct;
    QuicklaunchMenu *mMenu;
    QPoint mDragStart;
    TaskButtonStatus quicklanuchstatus;
    CustomStyle toolbuttonstyle;
    QGSettings *mgsettings;

    void modifyQuicklaunchMenuAction(bool direction);
private slots:
    void activateWithDraggable();


signals:
    void dropped(QObject * dragSource, QPoint const & pos);
    void dragging(QObject * dragSource, QPoint const & pos);
    //////////////////////////////////
    void buttonDeleted();
    void switchButtons(UKUITaskButton *from, UKUITaskButton *to);
    void t_saveSettings();
};


class ButtonMimeData: public QMimeData
{
    Q_OBJECT
public:
    ButtonMimeData():
        QMimeData(),
        mButton(0)
    {
    }

    UKUITaskButton *button() const { return mButton; }
    void setButton(UKUITaskButton *button) { mButton = button; }

private:
    UKUITaskButton *mButton;
};
//typedef QHash<WId,UKUITaskButton*> UKUITaskButtonHash;
//typedef QHash<WId,QWidget*> UKUITaskButtonHash;

#endif // UKUITASKBUTTON_H
