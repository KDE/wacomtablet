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

#include<QString>

#include "tabletinfo.h"

using namespace Wacom;

/*
 * Instanciate static instances-container of the TabletInfo template specialization.
 * This has to be done here obviously before any instance is created.
 */
template<>
TabletInfoTemplateSpecialization::Container TabletInfoTemplateSpecialization::instances = TabletInfoTemplateSpecialization::Container();


/*
 * Instanciate Properties.
 */
const TabletInfo TabletInfo::CompanyId   ( QLatin1String("CompanyId") );
const TabletInfo TabletInfo::CompanyName ( QLatin1String("CompanyName") );
const TabletInfo TabletInfo::CursorName  ( QLatin1String("CursorName") );
const TabletInfo TabletInfo::DeviceId    ( QLatin1String("DeviceId"));
const TabletInfo TabletInfo::DeviceNode  ( QLatin1String("DeviceNode"));
const TabletInfo TabletInfo::EraserName  ( QLatin1String("EraserName") );
const TabletInfo TabletInfo::PadName     ( QLatin1String("PadName") );
const TabletInfo TabletInfo::ProductId   ( QLatin1String("ProductId"));
const TabletInfo TabletInfo::StylusName  ( QLatin1String("StylusName") );
const TabletInfo TabletInfo::TabletId    ( QLatin1String("TabletId") );
const TabletInfo TabletInfo::TabletModel ( QLatin1String("TabletModel") );
const TabletInfo TabletInfo::TabletName  ( QLatin1String("TabletName") );
const TabletInfo TabletInfo::TouchName   ( QLatin1String("TouchName") );
const TabletInfo TabletInfo::VendorId    ( QLatin1String("VendorId") );

