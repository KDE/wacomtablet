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

#include "screenmap.h"
#include "stringutils.h"

#include <QtCore/QHash>
#include <QtCore/QStringList>

using namespace Wacom;

namespace Wacom
{
    class ScreenMapPrivate
    {
        public:
            static const QString SCREENAREA_SEPERATOR;
            static const QString SCREEN_SEPERATOR;

            QRect             tabletGeometry;
            QHash<int, QRect> mappings;
    };

    const QString ScreenMapPrivate::SCREENAREA_SEPERATOR = QLatin1String(":");
    const QString ScreenMapPrivate::SCREEN_SEPERATOR     = QLatin1String("|");
}

ScreenMap::ScreenMap(const QRect& tabletGeometry)
        : d_ptr(new ScreenMapPrivate)
{
    Q_D(ScreenMap);

    d->tabletGeometry = tabletGeometry;
}


ScreenMap::ScreenMap(const QString& mapping)
        : d_ptr(new ScreenMapPrivate)
{
    Q_D(ScreenMap);

    d->tabletGeometry = QRect(-1, -1, -1, -1);
    fromString(mapping);
}



ScreenMap::ScreenMap(const ScreenMap& screenMap)
        : d_ptr(new ScreenMapPrivate)
{
    operator=(screenMap);
}



ScreenMap::~ScreenMap()
{
    delete this->d_ptr;
}


ScreenMap& ScreenMap::operator=(const ScreenMap& screenMap)
{
    *(this->d_ptr) = *(screenMap.d_ptr);
    return (*this);
}


void ScreenMap::fromString(const QString& mappings)
{
    Q_D(ScreenMap);

    QStringList screenMappings = mappings.split(QLatin1String("|"), QString::SkipEmptyParts);
    QString     separator(QLatin1String(":"));
    QStringList mapping;
    ScreenSpace screen;
    QString     area;
    int         screenNumber;

    d->mappings.clear();

    foreach(QString screenMapping, screenMappings) {

        mapping = screenMapping.split(separator, QString::SkipEmptyParts);

        if (mapping.count() != 2) {
            continue;
        }

        screen        = ScreenSpace(mapping.at(0).trimmed());
        area          = mapping.at(1).trimmed();
        screenNumber  = screen.getScreenNumber();

        QRect areaRect;

        if (!area.contains(QLatin1String("-1 -1 -1 -1"))) {
            areaRect = StringUtils::toQRectByCoordinates(area);
        }

        if (areaRect.isEmpty()) {
            areaRect = d->tabletGeometry;
        }

        d->mappings.insert(screenNumber, areaRect);
    }
}



const QRect ScreenMap::getMapping(const ScreenSpace& screen, const ScreenRotation rotation) const
{
    Q_D(const ScreenMap);

    // try to find selection for the current screen
    QHash<int,QRect>::const_iterator citer = d->mappings.constFind(screen.getScreenNumber());

    QRect area;

    if (citer != d->mappings.constEnd()) {
        area = citer.value();
    } else {
        area = d->tabletGeometry;
    }

    if (rotation != ScreenRotation::NONE && isTabletGeometryValid()) {
        area = toRotation(d->tabletGeometry, area, rotation);
    }

    return area;
}


const QString ScreenMap::getMappingAsString(const ScreenSpace& screen) const
{
    return StringUtils::fromQRect(getMapping(screen), true);
}



void ScreenMap::setMapping(const ScreenSpace& screen, const QRect& mapping, const ScreenRotation& rotation)
{
    Q_D(ScreenMap);

    if (rotation != ScreenRotation::NONE && isTabletGeometryValid()) {
        d->mappings.insert(screen.getScreenNumber(), fromRotation(d->tabletGeometry, mapping, rotation));
    } else {
        d->mappings.insert(screen.getScreenNumber(), mapping);
    }
}



const QString ScreenMap::toString() const
{
    Q_D(const ScreenMap);

    // create mapping string
    QHash<int,QRect>::const_iterator mapping = d->mappings.constBegin();

    QString     separator = QLatin1String("|");
    ScreenSpace screen;
    QString     area;
    QString     mappings;
    QRect       mappingRect;

    for ( ; mapping != d->mappings.constEnd() ; ++mapping) {

        mappingRect = mapping.value();

        if (mappingRect.isEmpty()) {
            mappingRect = d->tabletGeometry;
        }

        area = StringUtils::fromQRect(mappingRect, true);

        if (mapping.key() >= 0) {
            screen = ScreenSpace::monitor(mapping.key());
        } else {
            screen = ScreenSpace::desktop();
        }

        if (!mappings.isEmpty()) {
            mappings.append(ScreenMapPrivate::SCREEN_SEPERATOR);
        }

        mappings.append(QString::fromLatin1("%1%2%3").arg(screen.toString())
                                                     .arg(ScreenMapPrivate::SCREENAREA_SEPERATOR)
                                                     .arg(area));
    }

    return mappings;
}




bool ScreenMap::isTabletGeometryValid() const
{
    Q_D(const ScreenMap);

    return (!d->tabletGeometry.isEmpty() &&
            !(d->tabletGeometry.x() == -1 && d->tabletGeometry.width()  == -1 &&
              d->tabletGeometry.y() == -1 && d->tabletGeometry.height() == -1));
}


const QRect ScreenMap::fromRotation(const QRect& tablet, const QRect& area, const ScreenRotation& rotation) const
{
    QRect result;

    if (rotation == ScreenRotation::CW) {
        result.setX(area.y());
        result.setY(tablet.height() - area.x() - area.width());
        result.setWidth(area.height());
        result.setHeight(area.width());

    } else if (rotation == ScreenRotation::CCW) {
        result.setX(tablet.width() - area.y() - area.height());
        result.setY(area.x());
        result.setWidth(area.height());
        result.setHeight(area.width());

    } else if (rotation == ScreenRotation::HALF) {
        result.setX(tablet.width() - area.width() - area.x());
        result.setY(tablet.height() - area.height() - area.y());
        result.setWidth(area.width());
        result.setHeight(area.height());
    }

    return result;
}


const QRect ScreenMap::toRotation(const QRect& tablet, const QRect& area, const ScreenRotation& rotation) const
{
    QRect result;

    if (rotation == ScreenRotation::CW) {
        result.setX(tablet.height() - area.height() - area.y());
        result.setY(area.x());
        result.setWidth(area.height());
        result.setHeight(area.width());

    } else if (rotation == ScreenRotation::CCW) {
        result.setX(area.y());
        result.setY(tablet.width() - area.width() - area.x());
        result.setWidth(area.height());
        result.setHeight(area.width());

    } else if (rotation == ScreenRotation::HALF) {
        result.setX(tablet.width() - area.width() - area.x());
        result.setY(tablet.height() - area.height() - area.y());
        result.setWidth(area.width());
        result.setHeight(area.height());
    }

    return result;
}
