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

#include "tabletrotation.h"

using namespace Wacom;

/*
 * Instanciate static instances-container of the TabletRotation template specialization.
 * This has to be done here obviously before any instance is created.
 */
template<>
TabletRotationTemplateSpecialization::Container TabletRotationTemplateSpecialization::instances = TabletRotationTemplateSpecialization::Container();


/*
 * Instanciate Device Types.
 */
const TabletRotation TabletRotation::NONE ( QLatin1String("none") );
const TabletRotation TabletRotation::CCW ( QLatin1String("ccw") );
const TabletRotation TabletRotation::HALF    ( QLatin1String("half") );
const TabletRotation TabletRotation::CW ( QLatin1String("cw") );
