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

#include "wacominterface.h"
#include "devicehandler.h"

//KDE includes
#include <KDE/KConfigGroup>
#include <KDE/KSharedConfig>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

// Qt includes
#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QRegExp>

using namespace Wacom;

WacomInterface::WacomInterface()
    : DeviceInterface()
{

}

WacomInterface::~WacomInterface()
{
}

void WacomInterface::applyProfile( const QString &device, const QString &section, KConfigGroup *gtprofile )
{
    KConfigGroup deviceGroup( gtprofile, section );
    m_applyArea = false;

    foreach( const QString & key, deviceGroup.keyList() ) {
        setConfiguration( device, key, deviceGroup.readEntry( key ) );
    }
}

void WacomInterface::setConfiguration( const QString &device, const QString &param, const QString &value )
{
    if( value.isEmpty() ) {
        return;
    }

    if( value == QString::fromLatin1( "0ChangeArea" ) ) {
        if( param == QString::fromLatin1( "true" ) ) {
            m_applyArea = true;
        }
        else {
            m_applyArea = false;
        }
    }

    QString cmd;
    if( value == QString::fromLatin1( "Area" ) && !m_applyArea) {
        cmd = QString::fromLatin1( "xsetwacom set \"%1\" ResetArea 0" ).arg( device );
    }
    else {
        QString modifiedParam = param;
        cmd = QString::fromLatin1( "xsetwacom set \"%1\" %2 \"%3\"" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^button([0-9])" ) ), QLatin1String( "button\\1" ) ) ).arg( value );
    }

    QProcess setConf;
    setConf.start( cmd );

    if( !setConf.waitForStarted() ) {
        return;
    }

    if( !setConf.waitForFinished() ) {
        return;
    }

    QByteArray errorOutput = setConf.readAll();

    if( !errorOutput.isEmpty() ) {
        kDebug() << cmd;
        kDebug() << errorOutput;
    }
}

QString WacomInterface::getConfiguration( const QString &device, const QString &param ) const
{
    QString modifiedParam = param;
    QString cmd = QString::fromLatin1( "xsetwacom get \"%1\" %2" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^button([0-9])" ) ), QLatin1String( "button \\1" ) ) );
    QProcess getConf;
    getConf.start( cmd );

    if( !getConf.waitForStarted() ) {
        return QString();
    }

    if( !getConf.waitForFinished() ) {
        return QString();
    }

    QString result = QLatin1String( getConf.readAll() );
    return result.remove( QLatin1Char( '\n' ) );
}

QString WacomInterface::getDefaultConfiguration( const QString &device, const QString &param ) const
{
    QString modifiedParam = param;
    QString cmd = QString::fromLatin1( "xsetwacom get \"%1\" %2" ).arg( device ).arg( modifiedParam.replace( QRegExp( QLatin1String( "^button([0-9])" ) ), QLatin1String( "button \\1" ) ) );
    QProcess getConf;
    getConf.start( cmd );

    if( !getConf.waitForStarted() ) {
        return QString();
    }

    if( !getConf.waitForFinished() ) {
        return QString();
    }

    QString result = QLatin1String( getConf.readAll() );
    return result.remove( QLatin1Char( '\n' ) );
}
