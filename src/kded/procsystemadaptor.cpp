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

#include "debug.h" // always needs to be first include

#include "procsystemadaptor.h"
#include "procsystemproperty.h"

#include <QProcess>

using namespace Wacom;

namespace Wacom {
class ProcSystemAdaptorPrivate
{
    public:
        QString        deviceName;
}; // CLASS
} // NAMESPACE


ProcSystemAdaptor::ProcSystemAdaptor(const QString& deviceName)
    : PropertyAdaptor(NULL), d_ptr(new ProcSystemAdaptorPrivate)
{
    Q_D( ProcSystemAdaptor );
    d->deviceName = deviceName;
}


ProcSystemAdaptor::~ProcSystemAdaptor()
{
    delete this->d_ptr;
}


const QList< Property > ProcSystemAdaptor::getProperties() const
{
    return ProcSystemProperty::ids();
}



const QString ProcSystemAdaptor::getProperty(const Property& property) const
{
    Q_D(const ProcSystemAdaptor);

    errWacom << QString::fromLatin1("Can not get unsupported property '%1' from device '%2' using proc system!").arg(property.key()).arg(d->deviceName);

    return QString();
}


bool ProcSystemAdaptor::setProperty(const Property& property, const QString& value)
{
    Q_D(const ProcSystemAdaptor);

    dbgWacom << QString::fromLatin1("Setting property '%1' to '%2'.").arg(property.key()).arg(value);

    // https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-driver-wacom
    /* /sys/bus/usb/devices/<busnum>-<devnum>:<cfg>.<intf>/wacom_led/status_led0_select
       Writing to this file sets which one of the four (for Intuos 4
       and Intuos 5) or of the right four (for Cintiq 21UX2 and Cintiq
       24HD) status LEDs is active (0..3). The other three LEDs on the
       same side are always inactive.
    */
    /* /sys/bus/usb/devices/<busnum>-<devnum>:<cfg>.<intf>/wacom_led/status_led1_select
       Writing to this file sets which one of the left four (for Cintiq 21UX2
       and Cintiq 24HD) status LEDs is active (0..3). The other three LEDs on
       the left are always inactive.
    */
    int statusLed = value.toInt();
    QString cmd;
    if(statusLed < 4 && statusLed >= 0) {
        cmd = QString::fromLatin1("bash -c \"echo %1 > /sys/bus/usb/devices/*/wacom_led/status_led0_select\"").arg(statusLed);
    }
    else if(statusLed < 8 && statusLed >= 4) {
        statusLed -= 4;
        cmd = QString::fromLatin1("bash -c \"echo %1 > /sys/bus/usb/devices/*/wacom_led/status_led1_select\"").arg(statusLed);
    }
    else {
        return false;
    }

    int ret = QProcess::execute(cmd);
    return ret == 0;
}


bool ProcSystemAdaptor::supportsProperty(const Property& property) const
{
    return (ProcSystemProperty::map(property) != NULL);
}

