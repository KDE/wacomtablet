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

#include "devicehandler.h"
#include "wacomdeviceadaptor.h"
#include "deviceinterface.h"
#include "wacominterface.h"

//KDE includes
#include <KDE/KConfigGroup>
#include <KDE/KSharedConfig>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

//Qt includes
#include <QtCore/QProcess>
#include <QtCore/QRegExp>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QtDBus>
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XInput.h>

namespace Wacom
{
    /**
      * Private class for the DeviceHandler d-pointer.
      */
    class DeviceHandlerPrivate
    {
    public:
        KSharedConfig::Ptr companyConfig;        /**< Ref Pointer for the data device list with all known tablet company information */
        DeviceInterface   *curDevice;            /**< Handler for the current device to get/set its configuration */
        QString            companyId;            /**< Unique ID of the tablet company (as hex)*/
        QString            deviceId;             /**< Unique device ID (as hex)*/
        QString            companyName;          /**< Name of the tablet company */
        QString            deviceName;           /**< Name of the tablet */
        QString            deviceModel;          /**< Model name of the tablet */
        QStringList        deviceList;           /**< List of all internal input device names found via xinput --list */
        QString            internalPadName;      /**< Internal name of the pad */
        QString            internalStylusName;   /**< Internal name of the stylus */
        QString            internalEraserName;   /**< Internal name of the eraser */
        QString            internalCursorName;   /**< Internal name of the cursor */
        QString            internalTouchName;    /**< Internal name of the touch device */
        bool               isDeviceAvailable;    /**< Is a tabled device connected or not? */
        bool               hasPadButtons;        /**< Does the tablet device has buttons that can be configured? */
    };
}

QDBusArgument& operator<<(QDBusArgument& argument, const Wacom::DeviceInformation& mystruct)
{
        argument.beginStructure();
        argument << mystruct.companyID << mystruct.deviceID << mystruct.companyName << mystruct.deviceName << mystruct.deviceModel << mystruct.deviceList << mystruct.padName << mystruct.stylusName << mystruct.eraserName << mystruct.cursorName << mystruct.touchName << mystruct.isDeviceAvailable << mystruct.hasPadButtons;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument& argument, Wacom::DeviceInformation& mystruct)
{
        argument.beginStructure();
        argument >> mystruct.companyID >> mystruct.deviceID >> mystruct.companyName
                 >> mystruct.deviceName >> mystruct.deviceModel >> mystruct.deviceList
                 >> mystruct.padName >> mystruct.stylusName >> mystruct.eraserName
                 >> mystruct.cursorName >> mystruct.touchName >> mystruct.isDeviceAvailable >> mystruct.hasPadButtons;
        argument.endStructure();
        return argument;
}

using namespace Wacom;

DeviceHandler::DeviceHandler()
    : d_ptr(new DeviceHandlerPrivate)
{
    Q_D(DeviceHandler);
    d->curDevice = 0;
    d->isDeviceAvailable = false;
    d->companyConfig = KSharedConfig::openConfig(KStandardDirs::locate("data", QLatin1String( "wacomtablet/data/companylist" )), KConfig::SimpleConfig, "data");

    if (d->companyConfig->groupList().isEmpty()) {
        kError() << "company list missing";
    }

    qDBusRegisterMetaType<Wacom::DeviceInformation>();
    qDBusRegisterMetaType< QList<Wacom::DeviceInformation> >();

}

DeviceHandler::~DeviceHandler()
{
    delete this->d_ptr->curDevice;
    delete this->d_ptr;
}

bool DeviceHandler::detectTablet()
{
    // ask xinput for the tablet information
    if (!findXInputDevice()) {
        kDebug() << "no input devices (pad/stylus/eraser/cursor/touch) found via xinput";

        return false;
    }

    Q_D(DeviceHandler);

    kDebug() << "XInput found a device! ::" << d->companyId << d->deviceId;

    // ok up to this point we have a tablet found somehow lets set it up to work with it
    if (!setDeviceInformation(d->companyId, d->deviceId)) {
        kError() << "could not set up the tablet information";
        return false;
    }

    // \0/
    d->isDeviceAvailable = true;
    return true;
}

void DeviceHandler::clearDeviceInformation()
{
    Q_D(DeviceHandler);

    //reset all values
    d->companyId.clear();
    d->deviceId.clear();
    d->companyName.clear();
    d->deviceName.clear();
    d->deviceModel.clear();
    d->deviceList.clear();
    d->internalPadName.clear();
    d->internalStylusName.clear();
    d->internalEraserName.clear();
    d->internalCursorName.clear();
    d->internalTouchName.clear();
    delete d->curDevice;
    d->curDevice = 0;
    d->isDeviceAvailable = false;
    d->hasPadButtons = false;
}

bool DeviceHandler::findXInputDevice()
{
    Q_D(DeviceHandler);
    bool deviceFound = false;

    XDeviceInfo	*info;
    int	ndevices;

    info = XListInputDevices(QX11Info::display(), &ndevices);

    for (int i = 0; i < ndevices; i++)
    {
        //if (info[i].use == IsXPointer || info[i].use == IsXKeyboard || info[i].use == IsXExtensionPointer)
        //    continue;

        uint wacom_prop = XInternAtom(QX11Info::display(), "Wacom Tool Type", True);

        XDevice *dev = XOpenDevice(QX11Info::display(), info[i].id);
        if (!dev) {
            continue;
        }

        int natoms;
        Atom *atoms = XListDeviceProperties(QX11Info::display(), dev, &natoms);
        if (natoms) {
            int j = 0;
            for (j = 0; j < natoms; j++) {
                if (atoms[j] == wacom_prop)
                    break;
            }

            if (j <= natoms) {
                unsigned char *data;
                Atom type;
                int format;
                unsigned long nitems, bytes_after;

                XGetDeviceProperty(QX11Info::display(), dev, wacom_prop, 0, 1, False, AnyPropertyType, &type,
                                   &format, &nitems, &bytes_after, &data);

                // if nitems is not 0 than atleast one wacom tool device exist
                if (nitems) {
                    char* type_name = XGetAtomName(QX11Info::display(), *(Atom*)data);

                    //add device name to the list of known devices
                    d->deviceList.append(QString::fromLatin1(info[i].name));

                    QString deviceType(QString::fromLatin1(type_name));
                    //check what type we found (with Type is Wacom xxx) and save internal reference)
                    if (deviceType.contains( QLatin1String( "pad" ), Qt::CaseInsensitive)) {
                        d->internalPadName = QString::fromLatin1(info[i].name);
                    } else if (deviceType.contains( QLatin1String( "eraser" ), Qt::CaseInsensitive)) {
                        d->internalEraserName = QString::fromLatin1(info[i].name);
                    } else if (deviceType.contains( QLatin1String( "cursor" ), Qt::CaseInsensitive)) {
                        d->internalCursorName = QString::fromLatin1(info[i].name);
                    } else if (deviceType.contains( QLatin1String( "touch" ),  Qt::CaseInsensitive)) {
                        d->internalTouchName = QString::fromLatin1(info[i].name);
                    } else if (deviceType.contains( QLatin1String( "stylus" ), Qt::CaseInsensitive)) {
                        d->internalStylusName = QString::fromLatin1(info[i].name);
                    }

                    deviceFound = true;
                    XFree(type_name);
                }

                XFree(data);
            }

            XFree(atoms);
        }

        if(deviceFound) {

            //get device id
            Atom prop = XInternAtom(QX11Info::display(), "Wacom Serial IDs", True);

            if (!prop)
            {
                kDebug() << "Property: Wacom Serial IDs not available";
            }
            else {
                Atom type;
                int format;
                unsigned char* data;
                unsigned long nitems, bytes_after;

                XGetDeviceProperty(QX11Info::display(), dev, prop, 0, 1000, False, AnyPropertyType, &type, &format, &nitems, &bytes_after, &data);

                // the offset for the tablet id is 0 see wacom-properties.h in the xf86-input-wacom driver for more information on this
                long *ldata = (long*)data;

                if(ldata) {
                    //transform tablet number into hex number (somehow .toInt(&ok,16) does not work
                    QString tabletHexID = QString::number(ldata[0], 16);
                    for (int i = tabletHexID.count(); i < 4; i++) {
                        tabletHexID.prepend(QLatin1String( "0" ));
                    }
                     d->deviceId = tabletHexID;
                 }

                XFree(data);
            }
        }
        XCloseDevice(QX11Info::display(), dev);
    }

    XFreeDeviceList(info);

    // if we just have no pad name only a name for a stylus...copy that, this is how the touchpc devices seem to work
    if (d->internalPadName.isEmpty()) {
        d->internalPadName = d->internalStylusName;
    }

    // this has moved here, as previously the company id could be parsed from the lsusb output, as this is not needed anymor, we
    // need to get the company id otherwise. the company id is responsible for the backend used to set things up
    // go through all known company's in the company list and see what we can find
    //@BUG this is definitely a problem if different company's use the same hexnumber to identify the tablet
    foreach(const QString &tempCompID, d->companyConfig->groupList()) {
        if (detectDeviceInformation(tempCompID, d->deviceId )) {
            break; // we found the connected company id lets end the cycle
        }
    }

    return deviceFound;
}

bool DeviceHandler::detectDeviceInformation(const QString & companyId, const QString & deviceId)
{
    Q_D(DeviceHandler);
    KConfigGroup companyGroup = KConfigGroup(d->companyConfig, companyId);

    if (companyGroup.keyList().isEmpty()) {
        //kDebug() << "no company information found for ID: " << companyId;
        return false;
    }

    // read the company name and the datafile for the device information
    KSharedConfig::Ptr deviceConfig = KSharedConfig::openConfig(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/data/%1").arg(companyGroup.readEntry("listfile"))), KConfig::SimpleConfig, "data");

    if (deviceConfig->groupList().isEmpty()) {
        kError() << "device list missing for company ID: " << companyId;
        return false;
    }

    KConfigGroup deviceGroup = KConfigGroup(deviceConfig, deviceId.toUpper());

    if (deviceGroup.keyList().isEmpty()) {
        kDebug() << "device info not found for device ID: " << deviceId << " :: company" << companyGroup.readEntry("name");
        return false;
    }

    // ok we found a list to the corresponding company id and a device that fits
    d->deviceId = deviceId.toUpper();
    d->companyId = companyId;

    return true;
}

bool DeviceHandler::setDeviceInformation(const QString & companyId, const QString & deviceId)
{
    Q_D(DeviceHandler);
    KConfigGroup companyGroup = KConfigGroup(d->companyConfig, companyId);

    // read the company name and the datafile for the device information
    KSharedConfig::Ptr deviceConfig = KSharedConfig::openConfig(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/data/%1").arg(companyGroup.readEntry("listfile"))), KConfig::SimpleConfig, "data");

    KConfigGroup deviceGroup = KConfigGroup(deviceConfig, deviceId.toUpper());

    d->companyName = companyGroup.readEntry("name");
    d->deviceModel = deviceGroup.readEntry("model");
    d->deviceName = deviceGroup.readEntry("name");

    if (deviceGroup.readEntry("padbuttons")  != QLatin1String("0") ||
        deviceGroup.readEntry("wheel")       != QLatin1String("no") ||
        deviceGroup.readEntry("touchring")   != QLatin1String("no") ||
        deviceGroup.readEntry("touchstripl") != QLatin1String("no") ||
        deviceGroup.readEntry("touchstripr") != QLatin1String("no")) {
        d->hasPadButtons = true;
    } else {
        d->hasPadButtons = false;
    }

    selectDeviceBackend(companyGroup.readEntry("driver"));
    return true;
}

void DeviceHandler::selectDeviceBackend(const QString & backendName)
{
    Q_D(DeviceHandler);
    //@TODO add switch statement to handle other backends too
    if (backendName == QLatin1String("wacom-tools")) {
        d->curDevice = new WacomInterface();
    }

    if (!d->curDevice) {
        kError() << "unknown device backend!" << backendName;
    }
}

DeviceInformation DeviceHandler::getAllInformation() const
{
    Q_D(const DeviceHandler);

    Wacom::DeviceInformation di;
    di.companyID = d->companyId;
    di.deviceID = d->deviceId;
    di.companyName = d->companyName;
    di.deviceName = d->deviceName;
    di.deviceModel = d->deviceModel;
    di.deviceList = d->deviceList;
    di.padName = d->internalPadName;
    di.stylusName = d->internalStylusName;
    di.eraserName = d->internalEraserName;
    di.cursorName = d->internalCursorName;
    di.touchName = d->internalTouchName;
    di.isDeviceAvailable = d->isDeviceAvailable;
    di.hasPadButtons = d->hasPadButtons;

    return di;
}

bool DeviceHandler::isDeviceAvailable() const
{
    Q_D(const DeviceHandler);
    return d->isDeviceAvailable;
}

bool DeviceHandler::hasPadButtons() const
{
    Q_D(const DeviceHandler);
    return d->hasPadButtons;
}

QString DeviceHandler::deviceId() const
{
    Q_D(const DeviceHandler);
    return d->deviceId;
}

QString DeviceHandler::companyId() const
{
    Q_D(const DeviceHandler);
    return d->companyId;
}

QString DeviceHandler::companyName() const
{
    Q_D(const DeviceHandler);
    return d->companyName;
}

QString DeviceHandler::deviceName() const
{
    Q_D(const DeviceHandler);
    return d->deviceName;
}

QString DeviceHandler::deviceModel() const
{
    Q_D(const DeviceHandler);
    return d->deviceModel;
}

QStringList DeviceHandler::deviceList() const
{
    Q_D(const DeviceHandler);
    return d->deviceList;
}

QString DeviceHandler::padName() const
{
    Q_D(const DeviceHandler);
    // if no pad is present, use stylus name as alternative way
    // fixes some problems with serial TabletPC that do not have a pad as such but still
    // can handle pad rotations and such when applied to the stylus settings
    if (d->internalPadName.isEmpty()) {
        return d->internalCursorName;
    } else {
        return d->internalPadName;
    }
}

QString DeviceHandler::stylusName() const
{
    Q_D(const DeviceHandler);
    return d->internalStylusName;
}

QString DeviceHandler::eraserName() const
{
    Q_D(const DeviceHandler);
    return d->internalEraserName;
}

QString DeviceHandler::cursorName() const
{
    Q_D(const DeviceHandler);
    return d->internalCursorName;
}

QString DeviceHandler::touchName() const
{
    Q_D(const DeviceHandler);
    return d->internalTouchName;
}

QString DeviceHandler::name(const QString & name) const
{
    Q_D(const DeviceHandler);
    if (name.contains( QLatin1String( "pad" ))) {
        return d->internalPadName;
    }
    if (name.contains( QLatin1String( "stylus" ))) {
        return d->internalStylusName;
    }
    if (name.contains( QLatin1String( "erser" ))) {
        return d->internalEraserName;
    }
    if (name.contains( QLatin1String( "cursor" ))) {
        return d->internalCursorName;
    }
    if (name.contains( QLatin1String( "touch" ))) {
        return d->internalTouchName;
    }

    return QString();
}

void DeviceHandler::applyProfile(KConfigGroup *gtprofile)
{
    Q_D(DeviceHandler);

    if(!d->curDevice)
        return;

    d->curDevice->applyProfile(d->internalPadName, QLatin1String( "pad" ), gtprofile);
    d->curDevice->applyProfile(d->internalStylusName, QLatin1String( "stylus" ), gtprofile);
    d->curDevice->applyProfile(d->internalEraserName, QLatin1String( "eraser" ), gtprofile);
    d->curDevice->applyProfile(d->internalTouchName, QLatin1String( "touch" ), gtprofile);
    //d->curDevice->applyProfile(d->internalCursorName, "cursor", gtprofile);
}

void DeviceHandler::setConfiguration(const QString & device, const QString & param, const QString & value)
{
    Q_D(DeviceHandler);

    if(!d->curDevice)
        return;

    d->curDevice->setConfiguration(device, param, value);
}

QString DeviceHandler::getConfiguration(const QString & device, const QString & param) const
{
    Q_D(const DeviceHandler);

    if(!d->curDevice)
        return QString();

    return d->curDevice->getConfiguration(device, param);
}

QString DeviceHandler::getDefaultConfiguration(const QString & device, const QString & param) const
{
    Q_D(const DeviceHandler);

    if(!d->curDevice)
        return QString();

    return d->curDevice->getDefaultConfiguration(device, param);
}
