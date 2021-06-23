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
#include <QMenu>
#include "../panel/iukuipanelplugin.h"
#include "ukuiwebviewdialog.h"
#include "../panel/common/ukuigridlayout.h"
#include "../panel/common_fun/listengsettings.h"
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
    bool isSeparate() const { return true; }
    void realign()override;
    void initializeCalendar();
    void setTimeShowStyle();
    /**
     * @brief modifyCalendarWidget 修改日历显示位置
     */
    void modifyCalendarWidget();
Q_SIGNALS:
    void deactivated();

private Q_SLOTS:
    void checkUpdateTime();
    void updateTimeText();
    void hidewebview();
    void CalendarWidgetShow();

private:
    QWidget *mMainWidget;
    frmLunarCalendarWidget *w;
    UkuiWebviewDialog   *mWebViewDiag;
    bool mbActived;
    bool mbHasCreatedWebView;
    int font_size;
    CalendarActiveLabel *mContent;
    UKUi::GridLayout *mLayout;
    QString timeState;
    QTimer *mTimer;
    QTimer *mCheckTimer;
    int mUpdateInterval;

    int16_t mViewWidht;
    int16_t mViewHeight;

    QString mActiveTimeZone;

    QGSettings *gsettings;
    QGSettings *fgsettings;
    QString hourSystemMode;

    QString hourSystem_24_horzontal;
    QString hourSystem_24_vartical;
    QString hourSystem_12_horzontal;
    QString hourSystem_12_vartical;
    QString current_date;
    IUKUIPanelPlugin * mPlugin;

};


class CalendarActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit CalendarActiveLabel(IUKUIPanelPlugin *plugin,QWidget * = NULL);

    IUKUIPanelPlugin * mPlugin;
    int16_t mViewWidht = 440;
    int16_t mViewHeight = 600 ;

    int changeHight = 0;
    void changeWidowpos();

protected:
    /**
     * @brief contextMenuEvent 右键菜单设置项
     * @param event
     */
    virtual void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    frmLunarCalendarWidget *w;


Q_SIGNALS:
    void pressTimeText();

private Q_SLOTS:
    /**
     * @brief setControlTime 右键菜单选项，在控制面板设置时间
     */
    void setControlTime();
    /**
     * @brief setUpPanel 设置任务栏选项
     */
    void setUpPanel();


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
