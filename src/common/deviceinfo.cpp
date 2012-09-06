/*
 * Copyright 2012 Alexander Maret-Huskinson <alex@maret.de>
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

#include<QString>

#include "deviceinfo.h"

using namespace Wacom;

/*
 * Instanciate static instances-container of the DeviceInfo template specialization.
 * This has to be done here obviously before any instance is created.
 */
template<>
DeviceInfoTemplateSpecialization::Container DeviceInfoTemplateSpecialization::instances = DeviceInfoTemplateSpecialization::Container();


/*
 * Instanciate Properties.
 */
const DeviceInfo DeviceInfo::CompanyId   ( QLatin1String("CompanyId") );
const DeviceInfo DeviceInfo::CompanyName ( QLatin1String("CompanyName") );
const DeviceInfo DeviceInfo::CursorName  ( QLatin1String("CursorName") );
const DeviceInfo DeviceInfo::EraserName  ( QLatin1String("EraserName") );
const DeviceInfo DeviceInfo::PadName     ( QLatin1String("PadName") );
const DeviceInfo DeviceInfo::StylusName  ( QLatin1String("StylusName") );
const DeviceInfo DeviceInfo::TabletId    ( QLatin1String("TabletId") );
const DeviceInfo DeviceInfo::TabletModel ( QLatin1String("TabletModel") );
const DeviceInfo DeviceInfo::TabletName  ( QLatin1String("TabletName") );
const DeviceInfo DeviceInfo::TouchName   ( QLatin1String("TouchName") );
