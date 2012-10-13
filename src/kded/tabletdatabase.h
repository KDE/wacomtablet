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

#ifndef TABLETDATABASE_H
#define TABLETDATABASE_H

#include "tabletinformation.h"

#include <QtCore/QString>
#include <QtCore/QMap>

// forward declarations
class KConfigGroup;

namespace Wacom
{

// Wacom forward declarations
class TabletDatabasePrivate;

class TabletDatabase
{
public:
    //! Default Constructor
    TabletDatabase ();
    TabletDatabase (const QString& dataDirectory, const QString& companyFile);
    ~TabletDatabase ();

    QString lookupBackend(const QString& companyId);

    bool lookupButtonMapping(QMap<QString,QString> &map, const QString &companyId, const QString &deviceId);

    bool lookupDevice(Wacom::TabletInformation& devinfo, const QString& deviceId);


private:

    bool lookupCompanyGroup (KConfigGroup& companyGroup, const QString& companyId);

    QString lookupCompanyId (const QString& deviceId);

    bool lookupDeviceGroup (KConfigGroup& deviceGroup, const QString& companyId, const QString& deviceId);

    bool lookupDeviceGroup (KConfigGroup& deviceGroup, KConfigGroup& companyGroup, const QString& deviceId);

    Q_DECLARE_PRIVATE(TabletDatabase)
    TabletDatabasePrivate *const d_ptr;  /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
