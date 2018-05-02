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

#include "screenspace.h"

#include "x11info.h"

using namespace Wacom;

namespace Wacom
{
class ScreenSpacePrivate
{
public:
    static const QString DESKTOP_STRING;

    QString output = DESKTOP_STRING;
};

const QString ScreenSpacePrivate::DESKTOP_STRING     = QLatin1String("desktop");
}


ScreenSpace::ScreenSpace()
    : d_ptr(new ScreenSpacePrivate)
{
    // nothing to do except for private class initialization
}


ScreenSpace::ScreenSpace(const ScreenSpace& screenSpace)
    : d_ptr(new ScreenSpacePrivate)
{
    operator=(screenSpace);
}


ScreenSpace::ScreenSpace(const QString& screenSpace)
    : d_ptr(new ScreenSpacePrivate)
{
    if (!screenSpace.isEmpty())
        setScreenSpace(screenSpace);
}


ScreenSpace::~ScreenSpace()
{
    delete this->d_ptr;
}



ScreenSpace& ScreenSpace::operator=(const ScreenSpace& screenSpace)
{
    *(this->d_ptr) = *(screenSpace.d_ptr);

    return (*this);
}


bool ScreenSpace::operator==(const ScreenSpace& screenSpace) const
{
    Q_D(const ScreenSpace);

    return d->output == screenSpace.d_ptr->output;
}


bool ScreenSpace::operator!=(const ScreenSpace& screenSpace) const
{
    return (!operator==(screenSpace));
}


const ScreenSpace ScreenSpace::desktop()
{
    return ScreenSpace(ScreenSpacePrivate::DESKTOP_STRING);
}


bool ScreenSpace::isDesktop() const
{
    Q_D(const ScreenSpace);

    return (d->output == ScreenSpacePrivate::DESKTOP_STRING);
}


bool ScreenSpace::isMonitor() const
{
    return !isDesktop();
}



const ScreenSpace ScreenSpace::monitor(QString output)
{
    return ScreenSpace(output);
}


const QString ScreenSpace::toString() const
{
    Q_D(const ScreenSpace);

    return d->output;
}

ScreenSpace ScreenSpace::next() const
{
    ScreenSpace nextScreen = ScreenSpace::desktop();

    if (isDesktop()) {
        nextScreen = ScreenSpace::monitor(X11Info::getPrimaryScreenName());
    } else {
        auto nextScreenName = X11Info::getNextScreenName(toString());
        if (nextScreenName == X11Info::getPrimaryScreenName()) {
            nextScreen = ScreenSpace::desktop();
        } else {
            nextScreen = ScreenSpace::monitor(nextScreenName);
        }
    }

    return nextScreen;
}


void ScreenSpace::setScreenSpace(const QString& screenSpace)
{
    Q_D(ScreenSpace);

    d->output = screenSpace;
}
