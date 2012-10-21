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

#include "debug.h"
#include "xsetwacomadaptor.h"
#include "xsetwacomproperty.h"
#include "stringutils.h"

#include <QtCore/QProcess>
#include <QtCore/QRegExp>

using namespace Wacom;

namespace Wacom {
class XsetwacomAdaptorPrivate
{
    public:
        QMap<QString, QString> buttonMap;
        QString                device;
}; // CLASS
} // NAMESPACE


XsetwacomAdaptor::XsetwacomAdaptor(const QString& deviceName)
    : PropertyAdaptor(NULL), d_ptr(new XsetwacomAdaptorPrivate)
{
    Q_D( XsetwacomAdaptor );
    d->device = deviceName;
}


XsetwacomAdaptor::XsetwacomAdaptor(const QString& deviceName, const QMap< QString, QString >& buttonMap)
    : PropertyAdaptor(NULL), d_ptr(new XsetwacomAdaptorPrivate)
{
    Q_D( XsetwacomAdaptor );

    d->buttonMap = buttonMap;
    d->device    = deviceName;
}

XsetwacomAdaptor::~XsetwacomAdaptor()
{
    delete this->d_ptr;
}


const QList< Property > XsetwacomAdaptor::getProperties() const
{
    return XsetwacomProperty::ids();
}


const QString XsetwacomAdaptor::getProperty(const Property& property) const
{
    Q_D( const XsetwacomAdaptor );

    const XsetwacomProperty* xsetproperty = XsetwacomProperty::map(property);

    if (xsetproperty == NULL) {
        kError() << QString::fromLatin1("Can not get unsupported property '%1' using xsetwacom!").arg(property.key());
        return QString();
    }

    // TODO: get invert scroll parameter

    QString convertedParam = convertParameter (*xsetproperty);
    QString xsetwacomValue = getParameter (d->device, convertedParam);

    return convertFromXsetwacomValue (*xsetproperty, xsetwacomValue);
}


bool XsetwacomAdaptor::setProperty(const Property& property, const QString& value)
{
    Q_D( const XsetwacomAdaptor );

    kDebug() << QString::fromLatin1("Setting property '%1' to '%2' on device '%3'.").arg(property.key()).arg(value).arg(d->device);

    const XsetwacomProperty* xsetproperty = XsetwacomProperty::map(property);

    if (xsetproperty == NULL) {
        kError() << QString::fromLatin1("Can not set unsupported property '%1' to '%2' on device '%3' using xsetwacom!").arg(property.key()).arg(value).arg(d->device);
        return false;
    }

    // check for special property
    if (property == Property::InvertScroll) {
        return setInvertScroll(value);

    } else {
        // normal property
        QString convertedParam = convertParameter(*xsetproperty);
        QString convertedValue = convertToXsetwacomValue(*xsetproperty, value);

        return setParameter(d->device, convertedParam, convertedValue);
    }

    return false;
}


bool XsetwacomAdaptor::supportsProperty(const Property& property) const
{
    return (XsetwacomProperty::map(property) != NULL);
}



const QString XsetwacomAdaptor::convertParameter(const XsetwacomProperty& param) const
{
    Q_D( const XsetwacomAdaptor );

    QString modifiedParam = param.key();

    // convert button values
    QRegExp rx(QLatin1String("^Button\\s*([0-9]+)$"), Qt::CaseInsensitive);
    int pos = 0;

    if ((pos = rx.indexIn(modifiedParam, pos)) != -1) {
        QString hwButtonNumber = rx.cap(1);
        QString kernelButtonNumber;

        if (!d->buttonMap.isEmpty()) {
            kernelButtonNumber = d->buttonMap.value(hwButtonNumber);
        }

        if (kernelButtonNumber.isEmpty()) {
            kernelButtonNumber = hwButtonNumber;
        }

        modifiedParam = QString(QLatin1String("Button %1")).arg(kernelButtonNumber);
    }

    return modifiedParam;
}



const QString XsetwacomAdaptor::convertFromXsetwacomValue(const XsetwacomProperty& property, const QString& value) const
{
    // TODO convert the value to internal format
    return value;
}



const QString XsetwacomAdaptor::convertToXsetwacomValue(const XsetwacomProperty& property, const QString& value) const
{
    // TODO convert the value to xsetwacom format
    return value;
}



const QString XsetwacomAdaptor::getParameter(const QString& device, const QString& param) const
{
    QString cmd = QString::fromLatin1( "xsetwacom get \"%1\" %2" ).arg( device ).arg( param );

    QProcess getConf;
    getConf.start( cmd );

    if( !getConf.waitForStarted() || !getConf.waitForFinished() ) {
        return QString();
    }

    QString result = QLatin1String( getConf.readAll() );
    return result.remove( QLatin1Char( '\n' ) );
}



bool XsetwacomAdaptor::setInvertScroll(const QString& value)
{
    Q_D( const XsetwacomAdaptor );

    if (StringUtils::asBool(value)) {
        setParameter(d->device, XsetwacomProperty::Button4.key(), QLatin1String("5"));
        setParameter(d->device, XsetwacomProperty::Button5.key(), QLatin1String("4"));
    } else {
        setParameter(d->device, XsetwacomProperty::Button5.key(), QLatin1String("4"));
        setParameter(d->device, XsetwacomProperty::Button4.key(), QLatin1String("5"));
    }

    return true;
}



bool XsetwacomAdaptor::setParameter(const QString& device, const QString& param, const QString& value) const
{
    QString  cmd = QString::fromLatin1( "xsetwacom set \"%1\" %2 \"%3\"" ).arg( device ).arg( param ).arg( value );

    QProcess setConf;
    setConf.start( cmd );

    if( !setConf.waitForStarted() || !setConf.waitForFinished()) {
        return false;
    }

    QByteArray errorOutput = setConf.readAll();

    if( !errorOutput.isEmpty() ) {
        kDebug() << cmd << " : " << errorOutput;
        return false;
    }

    return true;
}

