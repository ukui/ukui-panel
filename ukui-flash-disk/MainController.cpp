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
#include "MainController.h"
#include <KWindowEffects>
//#include "stdlib.h"

MainController* MainController::mSelf = 0;  //static variable
MainController* MainController::self()      //static function    //complete the singleton object
{
    if (!mSelf)
    {
        mSelf = new MainController;
    }
    return mSelf;
}

MainController::MainController()
{
}

MainController::~MainController()
{
}

int MainController::init()                   //init select
{
    m_DiskWindow = new MainWindow;         //main process singleton object
    connect(this, &MainController::notifyWnd, m_DiskWindow, &MainWindow::onNotifyWnd);
    return 0;
}
