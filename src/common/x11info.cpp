/*
 * This file is part of the KDE wacomtablet project. For copyright
 * information and license terms see the AUTHORS and COPYING files
 * in the top-level directory of this distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "x11info.h"

#include <QApplication>
#include <QDesktopWidget>

#include <X11/extensions/Xrandr.h>

using namespace Wacom;


int X11Info::getDefaultScreen()
{
    return QX11Info::appScreen();
}


Display* X11Info::getDisplay()
{
    return QX11Info::display();
}


const QRect X11Info::getDisplayGeometry()
{
    QList< QRect > screens = getScreenGeometries();
    QRect unitedScreen(0, 0, 0, 0);

    for (int i = 0 ; i < screens.size() ; ++i) {
        unitedScreen = unitedScreen.united(screens.at(i));
    }

    return unitedScreen;
}


int X11Info::getNumberOfScreens()
{
    if (QApplication::desktop()->isVirtualDesktop()) {
        return QApplication::desktop()->numScreens();
    }

    return 1;
}


const QList< QRect > X11Info::getScreenGeometries()
{
    QList< QRect > screens;

    if( QApplication::desktop()->isVirtualDesktop() ) {

        int num = QApplication::desktop()->numScreens();

        for( int i = 0; i < num; i++ ) {
            screens.append(QApplication::desktop()->screenGeometry(i));
        }

    } else {
        screens.append(QApplication::desktop()->screenGeometry(-1));
    }

    return screens;
}

const ScreenRotation X11Info::getScreenRotation()
{
    Rotation       xrandrRotation;
    ScreenRotation currentRotation = ScreenRotation::NONE;

    XRRRotations(getDisplay(), getDefaultScreen(), &xrandrRotation);

    switch (xrandrRotation) {
        case RR_Rotate_0:
            currentRotation = ScreenRotation::NONE;
            break;
        case RR_Rotate_90:
            currentRotation = ScreenRotation::CCW;
            break;
        case RR_Rotate_180:
            currentRotation = ScreenRotation::HALF;
            break;
        case RR_Rotate_270:
            currentRotation = ScreenRotation::CW;
            break;
        default:
            // defaults to NONE
            break;
    }

    return currentRotation;
}
