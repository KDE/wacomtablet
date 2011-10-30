/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "profilemanagement.h"

//KDE includes
#include <KDE/KConfigGroup>
#include <KDE/KSharedConfig>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

//Qt includes
#include <QtCore/QRegExp>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <QtCore/QDebug>

using namespace Wacom;

ProfileManagement::ProfileManagement( QDBusInterface *deviceInterface )
    : m_deviceInterface( deviceInterface )
{
    reload();
}

void ProfileManagement::createNewProfile( const QString &profilename )
{
    //get information via DBus
    QDBusReply<QString> deviceName  = m_deviceInterface->call( QLatin1String( "deviceName" ) );
    m_deviceName = deviceName;
    QDBusReply<QString> padName = m_deviceInterface->call( QLatin1String( "padName" ) );
    QDBusReply<QString> stylusName = m_deviceInterface->call( QLatin1String( "stylusName" ) );
    QDBusReply<QString> eraserName = m_deviceInterface->call( QLatin1String( "eraserName" ) );

    if( m_deviceName.isEmpty() || !padName.isValid() || !stylusName.isValid() || !eraserName.isValid() ) {
        kDebug() << "no device information are found. Can't create a new profile";
        return;
    }

    kDebug() << "create a new profile for :: device:" << m_deviceName << "pad:" << padName;

    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );

    KConfigGroup *deviceGroup = new KConfigGroup( profilesConfig, m_deviceName );
    KConfigGroup *profileGroup = new KConfigGroup( deviceGroup, profilename );

    // write default config for the pad
    KConfigGroup *padGroup = new KConfigGroup( profileGroup, "pad" );

    //padGroup->writeEntry( "Button1", "1" );
    padGroup->writeEntry( "Button2", "2" );
    padGroup->writeEntry( "Button3", "3" );
    padGroup->writeEntry( "Button4", "4" );
    padGroup->writeEntry( "Button5", "5" );
    padGroup->writeEntry( "Button6", "6" );
    padGroup->writeEntry( "Button7", "7" );
    padGroup->writeEntry( "Button8", "8" );
    padGroup->writeEntry( "Button9", "9" );
    padGroup->writeEntry( "Button10", "10" );
    padGroup->writeEntry( "StripLeftUp", "11" );
    padGroup->writeEntry( "StripLeftDown", "12" );
    padGroup->writeEntry( "StripRightUp", "13" );
    padGroup->writeEntry( "StripRightDown", "14" );
    padGroup->writeEntry( "RelWheelUp", "15" );
    padGroup->writeEntry( "RelWheelDown", "16" );
    padGroup->writeEntry( "AbsWheelUp", "15" );
    padGroup->writeEntry( "AbsWheelDown", "16" );

    padGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete padGroup;

    // write default config for the stylus
    KConfigGroup *stylusGroup = new KConfigGroup( profileGroup, "stylus" );

    //stylusGroup->writeEntry( "Button1", "1" ); // removed beacuse the wacomdriver has an awfull bug when reset button 1 to 1 (leftclick) 
    stylusGroup->writeEntry( "Button2", "2" );
    stylusGroup->writeEntry( "Button3", "3" );
    stylusGroup->writeEntry( "Mode", "absolute" );
    stylusGroup->writeEntry( "PressureCurve", "0 0 100 100" );
    stylusGroup->writeEntry( "Threshold", "27" );

    stylusGroup->writeEntry( "Rotate", "none" );
    stylusGroup->writeEntry( "0ForceProportions", "false" );
    stylusGroup->writeEntry( "0ScreenMapping", "randr" );
    stylusGroup->writeEntry( "0TabletArea", "full" );

    QDBusReply<QString> stylusArea = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( stylusName ), QLatin1String( "Area" ) );
    if( stylusArea.isValid() ) {
        stylusGroup->writeEntry( "Area", stylusArea.value() );
    }
    else {
        stylusGroup->writeEntry( "Area", "0 0 0 0" );
    }

    QDBusReply<QString> stylusTpcButton = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( stylusName ), QLatin1String( "TabletPCButton" ) );
    if( stylusTpcButton.isValid() ) {
        stylusGroup->writeEntry( "TabletPCButton", stylusTpcButton.value() );
    }
    else {
        stylusGroup->writeEntry( "TabletPCButton", "on" );
    }

    stylusGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete stylusGroup;

    // write default config for the eraser
    KConfigGroup *eraserGroup = new KConfigGroup( profileGroup, "eraser" );

    //eraserGroup->writeEntry( "Button1", "1" ); // removed beacuse the wacomdriver has an awfull bug when reset button 1 to 1 (leftclick) 
    eraserGroup->writeEntry( "Button2", "2" );
    eraserGroup->writeEntry( "Button3", "3" );
    eraserGroup->writeEntry( "Mode", "absolute" );
    eraserGroup->writeEntry( "PressureCurve", "0 0 100 100" );
    eraserGroup->writeEntry( "Threshold", "27" );

    //stylusGroup->writeEntry("RawFilter", "todo");
    //eraserGroup->writeEntry("Suppress", "todo");
    //eraserGroup->writeEntry("RawSample", "todo");

    eraserGroup->writeEntry( "Rotate", "none" );
    eraserGroup->writeEntry( "0ForceProportions", "false" );
    eraserGroup->writeEntry( "0ScreenMapping", "randr" );
    eraserGroup->writeEntry( "0TabletArea", "full" );
    //eraserGroup->writeEntry("MapToOutput", "todo"); // if we don't specify, xsetwacom will use a default one

    QDBusReply<QString> eraserArea = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( eraserName ), QLatin1String( "Area" ) );
    if( eraserArea.isValid() ) {
        eraserGroup->writeEntry( "Area", eraserArea.value() );
    }
    else {
        eraserGroup->writeEntry( "Area", "0 0 0 0" );
    }

    eraserGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete eraserGroup;

    // also add section for the touch if we have a touch tool
    QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );

    QString validName = touchName.value();
    if( !validName.isEmpty() ) {

        KConfigGroup *touchGroup = new KConfigGroup( profileGroup, "touch" );

        touchGroup->writeEntry( "Rotate", "none" );
        touchGroup->writeEntry( "0ForceProportions", "false" );
        touchGroup->writeEntry( "0ScreenMapping", "randr" );
        touchGroup->writeEntry( "0TabletArea", "full" );
        touchGroup->writeEntry( "Mode", "absolute" );

        QDBusReply<QString> touchArea = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( touchName ), QLatin1String( "Area" ) );
        if( touchArea.isValid() ) {
            touchGroup->writeEntry( "Area", touchArea.value() );
        }
        else {
            touchGroup->writeEntry( "Area", "0 0 0 0" );
        }

        QDBusReply<QString> touch = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( touchName ), QLatin1String( "Touch" ) );
        if( touch.isValid() ) {
            touchGroup->writeEntry( "Touch", touch.value() );
        }
        else {
            touchGroup->writeEntry( "Touch", "on" );
        }

        QDBusReply<QString> gesture = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( touchName ), QLatin1String( "Gesture" ) );
        if( gesture.isValid() ) {
            touchGroup->writeEntry( "Gesture", gesture.value() );
        }
        else {
            touchGroup->writeEntry( "Gesture", "on" );
        }

        QDBusReply<QString> zoomDistance = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( touchName ), QLatin1String( "ZoomDistance" ) );
        if( zoomDistance.isValid() ) {
            touchGroup->writeEntry( "ZoomDistance", zoomDistance.value() );
        }
        else {
            touchGroup->writeEntry( "ZoomDistance", "50" );
        }

        QDBusReply<QString> scrollDistance = m_deviceInterface->call( QLatin1String( "getConfiguration" ), QString( touchName ), QLatin1String( "ScrollDistance" ) );
        if( scrollDistance.isValid() ) {
            touchGroup->writeEntry( "ScrollDistance", scrollDistance.value() );
        }
        else {
            touchGroup->writeEntry( "ScrollDistance", "50" );
        }

        touchGroup->writeEntry( "TapTime", "250" );

        delete touchGroup;
    }

    delete profileGroup;
    delete deviceGroup;
}

KConfigGroup ProfileManagement::availableProfiles()
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );

    return KConfigGroup( profilesConfig, m_deviceName );
}

void ProfileManagement::deleteProfile()
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );
    KConfigGroup deviceGroup = KConfigGroup( profilesConfig, m_deviceName );
    KConfigGroup profileGroup = KConfigGroup( &deviceGroup, m_profileName );

    profileGroup.deleteGroup();
    m_profileName.clear();

    profilesConfig->reparseConfiguration();

    if( deviceGroup.groupList().isEmpty() ) {
        createNewProfile();
        profilesConfig->reparseConfiguration();
    }
}

KConfigGroup ProfileManagement::configGroup( const QString &section )
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );
    KConfigGroup deviceGroup = KConfigGroup( profilesConfig, m_deviceName );
    KConfigGroup profileGroup = KConfigGroup( &deviceGroup, m_profileName );

    return KConfigGroup( &profileGroup, section );
}

void ProfileManagement::setProfileName( const QString &name )
{
    m_profileName = name;
}

QString ProfileManagement::profileName() const
{
    return m_profileName;
}

ProfileManagement::PadButton ProfileManagement::getPadButtonFunction( const QString &buttonParam )
{
    bool isNumber;
    buttonParam.toInt( &isNumber );
    if( isNumber ) {
        return Pad_Button;
    }
    if( buttonParam.contains( QLatin1String( "key" ), Qt::CaseInsensitive ) ) {
        return Pad_Keystroke;
    }

    return Pad_Disable;
}

ProfileManagement::PenButton ProfileManagement::getPenButtonFunction( const QString &buttonParam )
{
    if( buttonParam.contains( QLatin1String( "1" ), Qt::CaseInsensitive ) ) {
        return Pen_LeftClick;
    }

    if( buttonParam.contains( QLatin1String( "3" ), Qt::CaseInsensitive ) ) {
        return Pen_RightClick;
    }

    if( buttonParam.contains( QLatin1String( "2" ), Qt::CaseInsensitive ) ) {
        return Pen_MiddleClick;
    }

    bool isNumber;
    buttonParam.toInt( &isNumber );
    if( isNumber ) {
        return Pen_Button;
    }

    if( buttonParam.contains( QLatin1String( "modetoggle" ), Qt::CaseInsensitive ) ) {
        return Pen_ModeToggle;
    }

    if( buttonParam.contains( QLatin1String( "displaytoggle" ), Qt::CaseInsensitive ) ) {
        return Pen_DisplayToggle;
    }

    if( buttonParam.contains( QLatin1String( "key" ), Qt::CaseInsensitive ) ) {
        return Pen_Keystroke;
    }

    return Pen_Disable;
}

QString ProfileManagement::transformButtonToConfig( PadButton mode, const QString &buttonParam )
{
    QString configString;
    switch( mode ) {
    case Pad_Disable:
        configString.clear();
        break;
    case Pad_Button:
        configString = buttonParam;
        configString.remove( QLatin1String( "button " ), Qt::CaseInsensitive );
        break;
    case Pad_Keystroke:
        configString = QString::fromLatin1( "key %1" ).arg( buttonParam );
        configString.replace( QLatin1String( "\\s*" ), QLatin1String( " " ) );
        configString = configString.toLower();
        break;
    }

    return configString;
}

QString ProfileManagement::transformButtonFromConfig( PadButton mode, QString &buttonParam )
{
    Q_UNUSED( mode );
    buttonParam.remove( QLatin1String( "key" ), Qt::CaseInsensitive );
    buttonParam.remove( QLatin1String( "button " ), Qt::CaseInsensitive );
    return buttonParam;
}

QString ProfileManagement::transformButtonToConfig( PenButton mode, const QString &buttonParam )
{
    QString configString;
    switch( mode ) {
    case Pen_Disable:
        configString.clear();
        break;
    case Pen_Button:
        configString = buttonParam;
        configString.remove( QLatin1String( "button " ), Qt::CaseInsensitive );
        break;
    case Pen_LeftClick:
        configString = QLatin1String( "1" );
        break;
    case Pen_MiddleClick:
        configString = QLatin1String( "2" );
        break;
    case Pen_RightClick:
        configString = QLatin1String( "3" );
        break;
    case Pen_Keystroke:
        configString = QString::fromLatin1( "key %1" ).arg( buttonParam );
        configString = configString.toLower();
        break;
    case Pen_ModeToggle:
        configString = QLatin1String( "modetoggle" );
        break;
    case Pen_DisplayToggle:
        configString = QLatin1String( "displaytoggle" );
        break;
    }

    return configString;
}

QString ProfileManagement::transformButtonFromConfig( PenButton mode, QString &buttonParam )
{
    Q_UNUSED( mode );
    buttonParam.remove( QLatin1String( "key" ), Qt::CaseInsensitive );
    buttonParam.remove( QLatin1String( "button " ), Qt::CaseInsensitive );
    return buttonParam;
}

void ProfileManagement::reload()
{
    QDBusReply<QString> deviceName  = m_deviceInterface->call( QLatin1String( "deviceName" ) );

    if( deviceName.isValid() ) {
        m_deviceName = deviceName;
    }
}
