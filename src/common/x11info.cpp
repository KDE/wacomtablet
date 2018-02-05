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
    auto screens = getScreenGeometries();
    QRect unitedScreen(0, 0, 0, 0);

    for (const auto &screen : screens) {
        unitedScreen = unitedScreen.united(screen);
    }

    return unitedScreen;
}

const QMap<QString, QRect> X11Info::getScreenGeometries()
{
    QMap<QString, QRect> screenGeometries;
    const auto screens = QGuiApplication::screens();

    Q_FOREACH (QScreen *screen, screens) {
        QRect geometry = screen->geometry();
        screenGeometries[screen->name()] = QRect(geometry.topLeft(), geometry.size() * screen->devicePixelRatio());
    }

    return screenGeometries;
}

const ScreenRotation X11Info::getScreenRotation(QString output)
{
    for (const auto &screen : QGuiApplication::screens()) {
        if (screen->name() == output) {
            switch (screen->orientation()) {
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
        }
    }

    return ScreenRotation::NONE;
}

const QString X11Info::getPrimaryScreenName()
{
    return QGuiApplication::primaryScreen()->name();
}

const QString X11Info::getNextScreenName(QString output)
{
    const auto screenNames = getScreenGeometries().keys();
    const auto index = screenNames.indexOf(output);

    if (index >= screenNames.size() - 1) {
        return screenNames.at(0);
    } else {
        return screenNames.at(index + 1);
    }
}
