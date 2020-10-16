/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */


#ifndef NIGHTMODE_H
#define NIGHTMODE_H

#include "../panel/iukuipanelplugin.h"
#include <QApplication>
#include <QToolButton>
#include <QProcess>
#include <QGSettings>

#include "../panel/ukuipanel.h"
#include "../panel/ukuicontrolstyle.h"

class NightModeButton:public QToolButton
{
    Q_OBJECT
public:
    NightModeButton(IUKUIPanelPlugin *plugin, QWidget* parent = 0);
    ~NightModeButton();

    void setupSettings();

protected:
    void mousePressEvent(QMouseEvent* event);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void setNightMode(const bool nightMode);
    void setUkuiStyle(QString );

    IUKUIPanelPlugin * mPlugin;
    QMenu *nightModeMenu;
    QGSettings *gsettings;
    QGSettings *mqtstyleGsettings;
    QGSettings *mgtkstyleGsettings;
    QSettings *mqsettings;
    QSettings  * kwinSettings;
    bool mode;

private slots:
    void turnNightMode();
    void setUpNightMode();
};

class NightMode : public QObject, public IUKUIPanelPlugin
{
    Q_OBJECT
public:
    NightMode(const IUKUIPanelPluginStartupInfo &startupInfo);
    ~NightMode();

    virtual QWidget *widget() { return mButton; }
    virtual QString themeId() const { return QStringLiteral("nightmode"); }
    void realign();

private:
    NightModeButton *mButton;
    QGSettings *gsettings;
};

class NightModeLibrary: public QObject, public IUKUIPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(IUKUIPanelPluginLibrary)
public:
    IUKUIPanelPlugin *instance(const IUKUIPanelPluginStartupInfo &startupInfo) const
    {
        return new NightMode(startupInfo);
    }
};
#endif
