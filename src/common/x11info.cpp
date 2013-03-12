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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QX11Info>

using namespace Wacom;


const QList< QRect > X11Info::getScreens()
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



const QRect X11Info::getScreensUnited()
{
    QList< QRect > screens = getScreens();
    QRect unitedScreen;

    for (int i = 0 ; i < screens.size() ; ++i) {
        unitedScreen = unitedScreen.united(screens.at(i));
    }

    return unitedScreen;
}


