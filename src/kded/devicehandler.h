/*
 * Copyright 2009, 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include "tabletprofile.h"
#include "deviceinformation.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMetaType>

namespace Wacom
{

class DeviceHandlerPrivate;
/**
  * This is the handler for all tablet devices
  * Allows the tablet detection and builds the interface between the KConfig related module and the
  * tablet settings via xsetwacom and in the future some other driver backends
  */
class DeviceHandler: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.WacomDevice")

    Q_PROPERTY(QString companyID      READ companyId)         /**< Unique ID of the tablet company (as hex)*/
    Q_PROPERTY(QString deviceID       READ deviceId)          /**< Unique device ID (as hex)*/
    Q_PROPERTY(QString companyName    READ companyName)       /**< Name of the tablet company */
    Q_PROPERTY(QString deviceName     READ deviceName)        /**< Name of the tablet */
    Q_PROPERTY(QString deviceModel    READ deviceModel)       /**< Model name of the tablet */
    Q_PROPERTY(QStringList deviceList READ deviceList)        /**< List of all internal input device names found via xinput --list */
    Q_PROPERTY(QString padName        READ padName)           /**< Internal name of the pad */
    Q_PROPERTY(QString stylusName     READ stylusName)        /**< Internal name of the stylus */
    Q_PROPERTY(QString eraserName     READ eraserName)        /**< Internal name of the eraser */
    Q_PROPERTY(QString cursorName     READ cursorName)        /**< Internal name of the cursor */
    Q_PROPERTY(QString touchName      READ touchName)         /**< Internal name of the touch */
    Q_PROPERTY(bool isDeviceAvailable READ isDeviceAvailable) /**< Is a tabled device connected or not? */
    Q_PROPERTY(bool hasPadButtons     READ hasPadButtons)     /**< Does the tablet device has buttons that can be configured? */

public:

    /**
      * default constructor
      */
    DeviceHandler();

    /**
      * default destructor
      */
    ~DeviceHandler();

    /**
      * resets all device information
      */
    void clearDeviceInformation();

    /**
      * Tablet Device detection
      * Finds a connected tablet via X11 search, and collects all the information from the wacom-driver (via X11 calls) and
      * additional information from the data files
      *
      * @return @c true if detection worked
      *         @c false if a failure happened
      */
    bool detectTablet();

    /**
      * Sets all new parameter of @p device according to the config @p gtprofile
      *
      * @param gtprofile Pointer to the KConfigGroup profile
      */
    void applyProfile(const TabletProfile &gtprofile);

public Q_SLOTS:

    Q_SCRIPTABLE DeviceInformation getAllInformation() const;

    /**
      * returns if a device in general was found or not
      *
      * @return @c true if something was detected
      *         @c false if nothing was found
      */
    Q_SCRIPTABLE bool isDeviceAvailable() const;

    /**
      * Tells you if the detected tablet has configurable pushbuttons or not
      *
      * @return @c true if pushbuttons are available and thus the conf dialogue can be shown
      *         @c false if nothing is available
      */
    Q_SCRIPTABLE bool hasPadButtons() const;

    /**
      * Internal device ID of the tablet
      *
      * @return Device ID
      */
    Q_SCRIPTABLE QString deviceId() const;

    /**
      * Unique ID of the tablet company (as hex)
      *
      * @return company ID
      */
    Q_SCRIPTABLE QString companyId() const;

    /**
      * Company Name of the Device
      *
      * @return Company Name
      */
    Q_SCRIPTABLE QString companyName() const;

    /**
      * Name of the connected graphic tablet
      *
      * @return Tablet name
      */
    Q_SCRIPTABLE QString deviceName() const;

    /**
      * Model name of the device
      *
      * @return Model name
      */
    Q_SCRIPTABLE QString deviceModel() const;

    /**
      * List of the internal device names (pad/stylus/eraser) as used by xsetwacom command
      *
      * @return StringList of the connected input devices
      */
    Q_SCRIPTABLE QStringList deviceList() const;

    /**
      * returns the internal name of the device @p name
      *
      * @param name the device we are looking for
      * @return the internal name of the device to be sued by the configuration
      */
    Q_SCRIPTABLE QString name(const QString & name) const;

    /**
      * Internal name of the pad as used by xsetwacom
      *
      * @return Pad name as used by xsetwacom
      */
    Q_SCRIPTABLE QString padName() const;

    /**
      * Internal name of the stylus as used by xsetwacom
      *
      * @return Stylus name as used by xsetwacom
      */
    Q_SCRIPTABLE QString stylusName() const;

    /**
      * Internal name of the eraser as used by xsetwacom
      *
      * @return Eraser name as used by xsetwacom
      */
    Q_SCRIPTABLE QString eraserName() const;

    /**
      * Internal name of the cursor as used by xsetwacom.
      *
      * @return Cursor name as used by xsetwacom
      */
    Q_SCRIPTABLE QString cursorName() const;

    /**
      * Internal name of the touch device as used by xsetwacom.
      *
      * @return Touch name as used by xsetwacom
      */
    Q_SCRIPTABLE QString touchName() const;

    /**
      * Sets the configuration of @p param from @p device with @p value
      * This is forwarded to the right backend specified by m_curDevice
      *
      * @param device name of the tablet device we set. Internal name of the pad/stylus/eraser/cursor
      * @param param Name of the parameter
      * @param value New value of the parameter
      */
    Q_SCRIPTABLE void setConfiguration(const QString & device, const QString & param, const QString & value);

    /**
      * returns the current value for a specific tablet setting
      * This is forwarded to the right backend specified by m_curDevice
      *
      * @param device name of the tablet device we set. Internal name of the pad/stylus/eraser/cursor
      * @param param the parameter we are looking for
      * @return the value as string
      */
    Q_SCRIPTABLE QString getConfiguration(const QString & device, const QString & param) const;

    /**
      * returns the factory default value for a specific tablet setting
      * This is forwarded to the right backend specified by m_curDevice
      *
      * @param device name of the tablet device we set. Internal name of the pad/stylus/eraser/cursor
      * @param param the parameter we are looking for
      * @return the factory default value as string
      *
      * @bug "xsetwacom getDefault device param" is not supported by the xf86-input-wacom driver right now
      */
    Q_SCRIPTABLE QString getDefaultConfiguration(const QString & device, const QString & param) const;

    /**
      * Toggles the touch tool on/off
      */
    Q_SCRIPTABLE void toggleTouch();

    /**
      * Toggles the stylus/eraser to absolute/relative mode
      */
    Q_SCRIPTABLE void togglePenMode();


private:

    /**
      * Sets the backend for the settings based on the name in the company list data file
      * Support is only available for xsetwacom at the moment, will hopefully change in the future
      *
      * @param backendName name of the used backend as specified in the device overview list
      */
    void selectDeviceBackend(const QString & backendName);

    Q_DECLARE_PRIVATE(DeviceHandler)
    DeviceHandlerPrivate *const d_ptr;  /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE

Q_DECLARE_METATYPE(Wacom::DeviceInformation)
Q_DECLARE_METATYPE(QList<Wacom::DeviceInformation>)

#endif // DEVICEHANDLER_H
