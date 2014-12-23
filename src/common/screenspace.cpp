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

#include "debug.h" // always needs to be first include

#include "screenspace.h"

#include <QtCore/QRegExp>

using namespace Wacom;

namespace Wacom
{
    class ScreenSpacePrivate
    {
        public:
            ScreenSpacePrivate() {
                monitor = -1;
            }

            static const QString DESKTOP_STRING_OLD; // old config file
            static const QString DESKTOP_STRING;
            static const QString MONITOR_PREFIX;

            int monitor;
    };

    const QString ScreenSpacePrivate::DESKTOP_STRING_OLD = QLatin1String("full");
    const QString ScreenSpacePrivate::DESKTOP_STRING     = QLatin1String("desktop");
    const QString ScreenSpacePrivate::MONITOR_PREFIX     = QLatin1String("map");
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
    setScreenSpace(screenSpace);
}



ScreenSpace::ScreenSpace(int monitorNumber)
        : d_ptr(new ScreenSpacePrivate)
{
    Q_D(ScreenSpace);

    if (monitorNumber <= -1) {
        d->monitor = -1;
    } else {
        d->monitor = monitorNumber;
    }
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

    return (d->monitor == screenSpace.d_ptr->monitor);
}


bool ScreenSpace::operator!=(const ScreenSpace& screenSpace) const
{
    return (!operator==(screenSpace));
}


const ScreenSpace ScreenSpace::desktop()
{
    return ScreenSpace(-1);
}


int ScreenSpace::getScreenNumber() const
{
    Q_D(const ScreenSpace);

    return d->monitor;
}


bool ScreenSpace::isDesktop() const
{
    Q_D(const ScreenSpace);

    return (d->monitor == -1);
}


bool ScreenSpace::isMonitor() const
{
    Q_D(const ScreenSpace);

    return (d->monitor >= 0);
}



bool ScreenSpace::isMonitor(int screenNumber) const
{
    Q_D(const ScreenSpace);

    return (d->monitor == screenNumber);
}


const ScreenSpace ScreenSpace::monitor(int screenNumber)
{
    if (screenNumber < 0) {
        screenNumber = 0;
    }

    return ScreenSpace(screenNumber);
}


const QString ScreenSpace::toString() const
{
    Q_D(const ScreenSpace);

    if (d->monitor >= 0) {
        return QString::fromLatin1("%1%2").arg(ScreenSpacePrivate::MONITOR_PREFIX).arg(d->monitor);
    }

    return ScreenSpacePrivate::DESKTOP_STRING;
}


void ScreenSpace::setScreenSpace(const QString& screenSpace)
{
    Q_D(ScreenSpace);

    QRegExp monitorRegExp(QLatin1String("map(\\d+)"), Qt::CaseInsensitive);
    QRegExp desktopRegExp(QString::fromLatin1("(?:%1|%2)").arg(ScreenSpacePrivate::DESKTOP_STRING)
                                                          .arg(ScreenSpacePrivate::DESKTOP_STRING_OLD), Qt::CaseInsensitive);

    if (monitorRegExp.indexIn(screenSpace, 0) != -1) {

        int screenNumber = monitorRegExp.cap(1).toInt();

        d->monitor = (screenNumber < 0) ? 0 : screenNumber;

    } else {
        if (desktopRegExp.indexIn(screenSpace, 0) == -1) {
            qDebug() << QString::fromLatin1("Failed to parse screen space '%1'!").arg(screenSpace);
        }

        d->monitor = -1;
    }
}
