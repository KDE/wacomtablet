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

#include "debug.h"
#include "x11tabletfinder.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>
#include <X11/Xutil.h>

using namespace Wacom;

/**
 * Class for private members.
 */
namespace Wacom {
    class X11TabletFinderPrivate
    {
        public:
            X11TabletFinderPrivate()
                : serialIdPropertyName(QLatin1String("Wacom Serial IDs")),
                  toolTypePropertyName(QLatin1String("Wacom Tool Type"))
            { }

            const QString     serialIdPropertyName;
            const QString     toolTypePropertyName;
            TabletInformation tabletinformation;
    };
}


X11TabletFinder::X11TabletFinder() : d_ptr(new X11TabletFinderPrivate)
{
    Q_D(X11TabletFinder);
    d->tabletinformation.setAvailable(false);
}


X11TabletFinder::~X11TabletFinder()
{
    delete d_ptr;
}



const TabletInformation& X11TabletFinder::getInformation() const
{
    Q_D(const X11TabletFinder);
    return d->tabletinformation;
}



bool X11TabletFinder::isAvailable() const
{
    Q_D(const X11TabletFinder);
    return d->tabletinformation.isAvailable();
}



bool X11TabletFinder::visit (X11InputDevice& device)
{
    Q_D(X11TabletFinder);

    if (!device.isTabletDevice()) {
        return false;
    }

    // parse tool type
    QString toolType = getToolType(device);

    if (toolType.isEmpty()) {
        kError() << QString::fromLatin1("Empty tool type property detected for device '%1'!").arg(device.getName());
        return false;
    }

    if (parseToolType(toolType, device.getName())) {
        d->tabletinformation.setAvailable(true);

    } else {
        kError() << QString::fromLatin1("Unsupported tool type property '%1'!").arg(toolType);
        return false;
    }


    // get tablet id
    QString tabletId = getTabletId(device);

    if (!tabletId.isEmpty()) {
        d->tabletinformation.set (TabletInfo::TabletId, tabletId);
    }

    return false;
}



const QString X11TabletFinder::getTabletId(X11InputDevice& device)
{
    Q_D(const X11TabletFinder);

    QString     tabletHexIdStr;
    QList<long> serialIdValues;

    if (!device.getLongProperty(d->serialIdPropertyName, serialIdValues, 1000)) {
        return tabletHexIdStr;
    }

    // the offset for the tablet id is 0 see wacom-properties.h in the xf86-input-wacom driver for more information on this
    long tabletId;

    if (serialIdValues.size() > 0) {
        tabletId = serialIdValues.at(0);

        if (tabletId > 0) {
            tabletHexIdStr = QString::fromLatin1("%1").arg(tabletId, 4, 16, QLatin1Char('0')).toUpper();
        }
    }

    return tabletHexIdStr;
}



const QString X11TabletFinder::getToolType(X11InputDevice& device)
{
    Q_D(const X11TabletFinder);

    QList<long> toolTypeAtoms;

    if (!device.getAtomProperty(d->toolTypePropertyName, toolTypeAtoms)) {
        return QString();
    }

    QString toolTypeName;

    if (toolTypeAtoms.size() == 1) {
        char *type_name = XGetAtomName (device.getDisplay(), (Atom)toolTypeAtoms.at(0));

        if (type_name != NULL) {
            toolTypeName = QLatin1String(type_name);
        }

        XFree( type_name );
    }

    return toolTypeName;
}


bool X11TabletFinder::parseToolType(const QString& toolType, const QString& deviceName)
{
    Q_D(X11TabletFinder);

    if( toolType.contains( QLatin1String( "pad" ), Qt::CaseInsensitive ) ) {
        d->tabletinformation.setDeviceName (DeviceType::Pad, deviceName);
        return true;

    } else if( toolType.contains( QLatin1String( "eraser" ), Qt::CaseInsensitive ) ) {
        d->tabletinformation.setDeviceName (DeviceType::Eraser, deviceName);
        return true;

    } else if( toolType.contains( QLatin1String( "cursor" ), Qt::CaseInsensitive ) ) {
        d->tabletinformation.setDeviceName (DeviceType::Cursor, deviceName);
        return true;

    } else if( toolType.contains( QLatin1String( "touch" ),  Qt::CaseInsensitive ) ) {
        d->tabletinformation.setDeviceName (DeviceType::Touch, deviceName);
        return true;

    } else if( toolType.contains( QLatin1String( "stylus" ), Qt::CaseInsensitive ) ) {
        d->tabletinformation.setDeviceName (DeviceType::Stylus, deviceName);
        return true;
    }

    return false;
}
