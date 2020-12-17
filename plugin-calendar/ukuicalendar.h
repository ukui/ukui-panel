/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012-2013 Razor team
 * Authors:
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


#include <QTimeZone>

#include <QDialog>
#include <QLabel>
#include <QtWebKitWidgets/QWebView>
#include <QGSettings>

#include "../panel/common/ukuirotatedwidget.h"
#include "../panel/iukuipanelplugin.h"
#include "../panel/popupmenu.h"
#include "lunarcalendarwidget/frmlunarcalendarwidget.h"

class QTimer;
class CalendarActiveLabel;
class UkuiCalendarWebView;

class IndicatorCalendar : public QWidget, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    IndicatorCalendar(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~IndicatorCalendar();

    virtual QWidget *widget() { return mMainWidget; }
    virtual QString themeId() const { return QLatin1String("Calendar"); }
//    virtual IUKUIPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    bool isSeparate() const { return true; }

    void settingsChanged()override;
    void realign()override;
    void initializeCalendar();
    void setTimeShowStyle();
    void setToolTip();
//signals:
Q_SIGNALS:
    void deactivated();

private Q_SLOTS:
    void timeout();
    void wheelScrolled(int);
    void deletePopup();
    void updateTimeText();

private:
    QWidget *mMainWidget;
    frmLunarCalendarWidget *w;
    bool mbActived;
    bool mbHasCreatedWebView;
    CalendarActiveLabel *mContent;

    QTimer *mTimer;
    int mUpdateInterval;

    int16_t mViewWidht;
    int16_t mViewHeight;

    QStringList mTimeZones;
    QMap<QString, QString> mTimeZoneCustomNames;
    QString mDefaultTimeZone;
    QString mActiveTimeZone;
    QString mFormat;

    bool mAutoRotate;
    QLabel *mPopupContent;

    QDateTime mShownTime;

    void restartTimer();

    void setTimeText();
    QString formatDateTime(const QDateTime &datetime, const QString &timeZoneName);
    void updatePopupContent();
    bool formatHasTimeZone(QString format);
    QString preformat(const QDateTime &dateTime,const QString &format, const QTimeZone &timeZone);
    bool mbIsNeedUpdate;
    QGSettings *gsettings;
    QString hourSystemMode;

    QString hourSystem_24_horzontal;
    QString hourSystem_24_vartical;
    QString hourSystem_12_horzontal;
    QString hourSystem_12_vartical;
    QString current_date;

};


class CalendarActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit CalendarActiveLabel(IUKUIPanelPlugin *plugin,QWidget * = NULL);
    ~CalendarActiveLabel();

Q_SIGNALS:
    void wheelScrolled(int);
    void leftMouseButtonClicked();
    void middleMouseButtonClicked();

protected:
    void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private Q_SLOTS:
    void setControlTime();
    void setUpPanel();

private:
    IUKUIPanelPlugin * mPlugin;
    frmLunarCalendarWidget *w;
    enum LunarCalendarState {ST_HIDE,ST_SHOW};
    LunarCalendarState state;
    int16_t mWidht;
    int16_t mHeight;

};

class UKUICalendarPluginLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ukui.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new IndicatorCalendar(startupInfo);
    }
};
