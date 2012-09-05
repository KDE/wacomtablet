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

#ifndef XINPUTADAPTOR_H
#define XINPUTADAPTOR_H

#include <QtCore/QString>

#include "propertyadaptor.h"

namespace Wacom {

// Forward Declarations
class XinputProperty;
class XinputAdaptorPrivate;

class XinputAdaptor : public PropertyAdaptor
{

public:
    //! Default constructor.
    explicit XinputAdaptor(const QString& deviceName);

    //! Destructor
    virtual ~XinputAdaptor();

    /**
     * @sa PropertyAdaptor::getProperties()
     */
    const QList<Property> getProperties() const;

    /**
     * @sa PropertyAdaptor::getProperty(const Property&)
     */
    const QString getProperty(const Property& property) const;

    /**
     * @sa PropertyAdaptor::setProperty(const Property&, const QString&)
     */
    bool setProperty(const Wacom::Property& property, const QString& value) const;

    /**
     * @sa PropertyAdaptor::supportsProperty(const Property&)
     */
    bool supportsProperty(const Property& property) const;


private:

    bool mapTabletToScreen(const QString& screenArea) const;
    
    bool setProperty(const XinputProperty& property, const QString& value) const;

    Q_DECLARE_PRIVATE( XinputAdaptor )
    XinputAdaptorPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
