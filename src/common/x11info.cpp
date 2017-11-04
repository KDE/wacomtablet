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
#include <QScreen>

using namespace Wacom;

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
    if (QGuiApplication::primaryScreen()->virtualSiblings().size() > 1) {
        return QGuiApplication::screens().size();
    }

    return 1;
}


const QList< QRect > X11Info::getScreenGeometries()
{
    QList< QRect > screenGeometries;

    auto primaryScreen = QGuiApplication::primaryScreen();
    if(primaryScreen->virtualSiblings().size() > 1) {
        auto screens = QGuiApplication::screens();

        Q_FOREACH (QScreen *screen, screens) {
            QRect geometry = screen->geometry();
            screenGeometries.append(QRect(geometry.topLeft(), geometry.size() * screen->devicePixelRatio()));
        }
    } else {
        QRect geometry = primaryScreen->geometry();
        screenGeometries.append(QRect(geometry.topLeft(), geometry.size() * primaryScreen->devicePixelRatio()));
    }

    return screenGeometries;
}

const ScreenRotation X11Info::getScreenRotation()
{
    switch (QGuiApplication::primaryScreen()->orientation()) {
    case Qt::PrimaryOrientation:
    case Qt::LandscapeOrientation:
        return ScreenRotation::NONE;
    case Qt::PortraitOrientation:
        return ScreenRotation::CW;
    case Qt::InvertedLandscapeOrientation:
        return ScreenRotation::HALF;
    case Qt::InvertedPortraitOrientation:
        return ScreenRotation::CCW;
    }

    // this should never happen, but we probably should log an error here
    return ScreenRotation::NONE;
}
