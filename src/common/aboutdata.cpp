/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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
#include "aboutdata.h"

#include <KDE/KLocalizedString>

using namespace Wacom;

AboutData::AboutData(const QByteArray& appName, const KLocalizedString& programName, const QByteArray& version,
                     const KLocalizedString& shortDescription, const KLocalizedString& otherText)
    : KAboutData(appName,
                 "wacomtablet",
                 programName,
                 version,
                 shortDescription,
                 KAboutData::License_GPL,
                 ki18n("(c), 2009,2010 Jörg Ehrichs"),
                 otherText,
                 "http://www.etricceline.de")
{
    addAuthor( ki18n( "Jörg Ehrichs" ), ki18n( "Maintainer" ) , "joerg.ehrichs@gmx.de" );
    addAuthor( ki18n( "Alexander Maret-Huskinson" ), ki18n( "Contributor" ) , "alex@maret.de" );
}
