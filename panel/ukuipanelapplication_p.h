/*
 * Copyright (C) 2016  Luís Pereira <luis.artur.pereira@gmail.com>
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef UKUIPanelAPPLICATION_P_H
#define UKUIPanelAPPLICATION_P_H

#include "ukuipanelapplication.h"

namespace UKUi {
class Settings;
}

class UKUIPanelApplicationPrivate {
    Q_DECLARE_PUBLIC(UKUIPanelApplication)
public:

    UKUIPanelApplicationPrivate(UKUIPanelApplication *q);
    ~UKUIPanelApplicationPrivate() {};

    UKUi::Settings *mSettings;

    IUKUIPanel::Position computeNewPanelPosition(const UKUIPanel *p, const int screenNum);

private:
    UKUIPanelApplication *const q_ptr;
};

#endif // UKUIPanelAPPLICATION_P_H
