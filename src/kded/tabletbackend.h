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

#ifndef TABLETBACKEND_H
#define TABLETBACKEND_H

#include "tabletbackendinterface.h"

#include <QMap>
#include <QList>

namespace Wacom
{

// forward declaration
class TabletBackendPrivate;

/**
 * The tablet backend which is used to set properties on a tablet.
 * The properties which are available depend on the property adapters
 * which were added to the backend.
 */
class TabletBackend : public TabletBackendInterface
{
public:

    explicit TabletBackend(const TabletInformation& tabletInformation);
    virtual ~TabletBackend();

    /**
     * @see TabletBackendInterface::addAdaptor(const DeviceType&, PropertyAdaptor*)
     */
    void addAdaptor(const DeviceType& deviceType, PropertyAdaptor* adaptor);

    /**
     * @see TabletBackendInterface::getInformation() const;
     */
    const TabletInformation& getInformation() const;

    /**
     * @see TabletBackendInterface::getProperty(const DeviceType&, const Property&) const
     */
    const QString getProperty(const DeviceType& type, const Property& property) const;

    /**
     * @see TabletBackendInterface::setProfile(const TabletProfile&)
     */
    void setProfile(const TabletProfile& profile);

    /**
     * @see TabletBackendInterface::setProfile(const DeviceType&, const DeviceProfile&)
     */
    void setProfile(const DeviceType& deviceType, const DeviceProfile& profile);

    /**
     * @see TabletBackendInterface::setStatusLED(int led)
     */
    void setStatusLED(int led);

    /**
     * @see TabletBackendInterface::setStatusLEDBrightness(int brightness)
     */
    void setStatusLEDBrightness(int brightness);

    /**
     * @see TabletBackendInterface::setProperty(const DeviceType&, const Property&, const QString&)
     */
    bool setProperty(const DeviceType& type, const Property& property, const QString& value);


private:
    typedef QList<PropertyAdaptor*>       AdaptorList;
    typedef QMap<DeviceType, AdaptorList> DeviceMap;

    Q_DECLARE_PRIVATE(TabletBackend);
    TabletBackendPrivate *const d_ptr; //!< D-Pointer which gives access to private members.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
