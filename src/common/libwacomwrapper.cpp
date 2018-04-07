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

#include "libwacomwrapper.h"

#include "debug.h"

#include <memory>

extern "C" {
#include <libwacom/libwacom.h>
}

namespace Wacom {

libWacomWrapper::libWacomWrapper()
{
    db = libwacom_database_new();
}

libWacomWrapper &libWacomWrapper::instance()
{
    static libWacomWrapper instance;
    return instance;
}

libWacomWrapper::~libWacomWrapper()
{
    libwacom_database_destroy(db);
}

bool libWacomWrapper::lookupTabletInfo(int tabletId, int vendorId, TabletInformation &tabletInfo)
{
    dbgWacom << "LibWacom lookup for" << tabletId << vendorId;
    auto errorDeleter = [](WacomError *&e){libwacom_error_free(&e);};
    std::unique_ptr<WacomError, decltype(errorDeleter)>
            error(libwacom_error_new(), errorDeleter);
    std::unique_ptr<WacomDevice, decltype(&libwacom_destroy)>
            device(libwacom_new_from_usbid(db, vendorId, tabletId, error.get()), &libwacom_destroy);

    if (!device) {
        dbgWacom << "LibWacom lookup failed:" << libwacom_error_get_message(error.get());
        return false;
    }

    // TODO: libWacom returned button layouts don't make much sense (yet)
    // because they use letters instead of numbers
    tabletInfo.set(TabletInfo::ButtonLayout,  QString());
    // Seems like there is no model or vendor names in libwacom
    // TODO: are these properties even used anywhere?
    tabletInfo.set(TabletInfo::CompanyName,   QString());
    tabletInfo.set(TabletInfo::TabletModel,   QString());
    // TODO: Seems like there's no info about the pad wheel in libwacom
    // Only a couple of tablets have the wheel anyway, so it probably can be hacked around
    tabletInfo.set(TabletInfo::HasWheel,      false);
    // TODO: Returns more detailed information than we expect,
    // current LED code is broken anyway so this should be untangled later
    tabletInfo.set(TabletInfo::StatusLEDs,    QString::number(0));

    tabletInfo.set(TabletInfo::TabletName,    QString::fromLatin1(libwacom_get_name(device.get())));
    tabletInfo.set(TabletInfo::NumPadButtons, QString::number(libwacom_get_num_buttons(device.get())));

    int numStrips = libwacom_get_num_strips(device.get());
    bool hasLeftStrip = numStrips > 0;
    bool hasRightStrip = numStrips > 1;
    bool hasRing = libwacom_has_ring(device.get()) != 0;

    tabletInfo.set(TabletInfo::HasLeftTouchStrip,  hasLeftStrip);
    tabletInfo.set(TabletInfo::HasRightTouchStrip, hasRightStrip);
    tabletInfo.set(TabletInfo::HasTouchRing,       hasRing);
    return true;
}

}
