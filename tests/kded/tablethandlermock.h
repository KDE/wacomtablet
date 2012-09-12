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

#ifndef TABLETHANDLERMOCK_H
#define TABLETHANDLERMOCK_H

#include "tablethandlerinterface.h"
#include "tabletinformation.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{
class TabletHandlerMock : public TabletHandlerInterface
{
    Q_OBJECT

public:
    TabletHandlerMock();
    virtual ~TabletHandlerMock();


    //! Emmits a profileChanged signal with the given parameter.
    void emitProfileChanged(const QString& profile);

    //! Emmits a tabletAdded signal with the given parameter.
    void emitTabletAdded(const TabletInformation& info);

    //! Emmits a tabletRemoved signal.
    void emitTabletRemoved();


    //! Gets the current mock property value no matter which property or device is requested.
    QString getProperty(const QString& device, const Property& property) const;

    //! Gets the current mock property value no matter which property or device is requested.
    QString getProperty(const QString& device, const QString& property) const;

    //! Returns the mock's profile list.
    QStringList listProfiles() const;

    //! Sets the given profile on the mock and emits a profileChanged signal.
    void setProfile(const QString& profile);

    //! Sets the given property value on the mock no matter which device or property is set.
    void setProperty(const QString & device, const QString& property, const QString & value);

    //! Sets the given property value on the mock no matter which device or property is set.
    void setProperty(const QString& device, const Property & property, const QString& value);

    //! Toggles the mock's pen mode flag.
    void togglePenMode();

    //! Toggle the mock's touch flag.
    void toggleTouch();


Q_SIGNALS:

    void profileChanged(const QString& profile);

    void tabletAdded(const TabletInformation& info);

    void tabletRemoved();


public:
    QString     m_device;          //!< The device a property was set on and the device a property is returned from.
    QString     m_property;        //!< The property which was set and the property which can be get.
    QString     m_propertyValue;   //!< The property value returned by this mock, no matter which property is requested.
    QStringList m_profiles;        //!< The list of profiles returned by this mock.
    QString     m_profile;         //!< The profile name returned by this mock.
    bool        m_flagPenMode;     //!< The flag which resembles the pen mode state.
    bool        m_flagTouch;       //!< The flag which resembles the touch state.


}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
