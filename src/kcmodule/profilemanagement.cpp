/*
 * Copyright 2009 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

ProfileManagement::ProfileManagement(QDBusInterface *deviceInterface)
        : m_deviceInterface(deviceInterface)
{
    reload();
}

void ProfileManagement::createNewProfile(const QString & profilename)
{
    //get information via DBus
    QDBusReply<QString> deviceName  = m_deviceInterface->call(QLatin1String( "deviceName" ));
    m_deviceName = deviceName;
    QDBusReply<QString> padName = m_deviceInterface->call(QLatin1String( "padName" ));

    if(m_deviceName.isEmpty() || !padName.isValid())
    {
        kDebug() << "no device information are found. Can't create a new profile";
        return;
    }

    kDebug() << "create a new profile for :: device:" << m_deviceName << "pad:" << padName;

    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig(QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig);

    KConfigGroup *deviceGroup = new KConfigGroup(profilesConfig, m_deviceName);
    KConfigGroup *profileGroup = new KConfigGroup(deviceGroup, profilename);

    // write default config for the pad
    KConfigGroup *padGroup = new KConfigGroup(profileGroup, "pad");

    padGroup->writeEntry("Button1", "1");
    padGroup->writeEntry("Button2", "2");
    padGroup->writeEntry("Button3", "3");
    padGroup->writeEntry("Button4", "4");
    padGroup->writeEntry("Button5", "5");
    padGroup->writeEntry("Button6", "6");
    padGroup->writeEntry("Button7", "7");
    padGroup->writeEntry("Button8", "8");
    padGroup->writeEntry("Button9", "9");
    padGroup->writeEntry("Button10", "10");
    padGroup->writeEntry("StripLUp", "11");
    padGroup->writeEntry("StripLDn", "12");
    padGroup->writeEntry("StripRUp", "13");
    padGroup->writeEntry("StripRDn", "14");
    padGroup->writeEntry("RelWUp", "15");
    padGroup->writeEntry("RelWDn", "16");
    padGroup->writeEntry("AbsWUp", "15");
    padGroup->writeEntry("AbsWDn", "16");
    //padGroup->writeEntry("TPCButton", "On");
    padGroup->writeEntry("Rotate", "none");


    // @bug The xf86-input-wacom driver does not have the getDefault call for its xsetwacom tool
    // Thus here I check if the return value is indeed an integer. otherwise I retry with the normal get call
    // This fix helps to get meaningfull numbers for the working area on devices used by the new xorg 1.7 server

    bool isInt;
    QString getCallname(QLatin1String( "getDefaultConfiguration" )); // name of the function we use. either getDefaultConfiguration or just getConfiguration

    QDBusReply<QString> topX = m_deviceInterface->call(getCallname, QString(padName), QLatin1String( "TopX" ));
    if (topX.isValid()) {
        // now check if the returned value is indeed an integer
        QString topXCheck = topX.value();
        topXCheck.toInt(&isInt);

        if(isInt) {
            // ok it is an integer just proceed as usual with the other calls
            padGroup->writeEntry("TopX", topX.value());
        } else {
            // it is not an integer thus the getDefaultConfiguration is not supported here
            // lets switch to the normal get calls with this and the other calls
            qDebug() << "not integer found ... seems to be an error on my side";
            getCallname = QLatin1String( "getConfiguration" );

            QDBusReply<QString> topX = m_deviceInterface->call(getCallname, QString(padName), QLatin1String( "TopX" ));
            if (topX.isValid()) {
                padGroup->writeEntry("TopX", topX.value());
            } else {
                padGroup->writeEntry("TopX", "0");
            }
        }
    } else {
        padGroup->writeEntry("TopX", "0");
    }


    QDBusReply<QString> topY = m_deviceInterface->call(getCallname, QString(padName), QLatin1String( "TopY" ));
    if (topY.isValid()) {
        padGroup->writeEntry("TopY", topY.value());
    } else {
        padGroup->writeEntry("TopY", "0");
    }
    QDBusReply<QString> bottomX = m_deviceInterface->call(getCallname, QString(padName), QLatin1String( "BottomX" ));
    if (bottomX.isValid()) {
        padGroup->writeEntry("BottomX", bottomX.value());
    } else {
        padGroup->writeEntry("BottomX", "0");
    }
    QDBusReply<QString> bottomY = m_deviceInterface->call(getCallname, QString(padName), QLatin1String( "BottomY" ));
    if (bottomY.isValid()) {
        padGroup->writeEntry("BottomY", bottomY.value());
    } else {
        padGroup->writeEntry("BottomY", "0");
    }

    padGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete padGroup;

    // write default config for the stylus
    KConfigGroup *stylusGroup = new KConfigGroup(profileGroup, "stylus");

    stylusGroup->writeEntry("Button1", "1");
    stylusGroup->writeEntry("Button2", "2");
    stylusGroup->writeEntry("Button3", "3");
    //stylusGroup->writeEntry("Suppress", "todo");
    //stylusGroup->writeEntry("RawSample", "todo");
    stylusGroup->writeEntry("PressCurve", "0 0 100 100");
    stylusGroup->writeEntry("Mode", "absolute");
    stylusGroup->writeEntry("CursorProx", "42"); //@todo get/set values that give a better feeling for it.Might have a bug in my wacom-tools because its alwas -1 here
    //stylusGroup->writeEntry("RawFilter", "todo");
    stylusGroup->writeEntry("SpeedLevel", "6");
    stylusGroup->writeEntry("ClickForce", "6");
    stylusGroup->writeEntry("0TwinView", "none");
    stylusGroup->writeEntry("1TVResolution0", "0 0");
    stylusGroup->writeEntry("1TVResolution1", "0 0");
    stylusGroup->writeEntry("mmonitor", "1");
    stylusGroup->writeEntry("Screen_No", "-1");

    stylusGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete stylusGroup;

    // write default config for the eraser
    KConfigGroup *eraserGroup = new KConfigGroup(profileGroup, "eraser");

    eraserGroup->writeEntry("Button1", "1");
    eraserGroup->writeEntry("Button2", "2");
    eraserGroup->writeEntry("Button3", "3");
    //eraserGroup->writeEntry("Suppress", "todo");
    //eraserGroup->writeEntry("RawSample", "todo");
    eraserGroup->writeEntry("PressCurve", "0 0 100 100");
    eraserGroup->writeEntry("Mode", "absolute");
    //eraserGroup->writeEntry("CursorProx", "42"); //@todo get/set values that give a better feeling for it.Might have a bug in my wacom-tools because its alwas -1 here
    //eraserGroup->writeEntry("RawFilter", "todo");
    eraserGroup->writeEntry("SpeedLevel", "6");
    eraserGroup->writeEntry("ClickForce", "6");
    eraserGroup->writeEntry("0TwinView", "none");
    eraserGroup->writeEntry("1TVResolution0", "0 0");
    eraserGroup->writeEntry("1TVResolution1", "0 0");
    eraserGroup->writeEntry("mmonitor", "1");
    eraserGroup->writeEntry("Screen_No", "-1");

    eraserGroup->sync();
    profileGroup->sync();
    deviceGroup->sync();
    delete eraserGroup;

    delete profileGroup;
    delete deviceGroup;
}

KConfigGroup ProfileManagement::availableProfiles()
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig(QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig);

    return KConfigGroup(profilesConfig, m_deviceName);
}

void ProfileManagement::deleteProfile()
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig(QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig);
    KConfigGroup deviceGroup = KConfigGroup(profilesConfig, m_deviceName);
    KConfigGroup profileGroup = KConfigGroup(&deviceGroup, m_profileName);

    profileGroup.deleteGroup();
    m_profileName.clear();

    profilesConfig->reparseConfiguration();

    if (deviceGroup.groupList().isEmpty()) {
        createNewProfile();
        profilesConfig->reparseConfiguration();
    }
}

KConfigGroup ProfileManagement::configGroup(const QString & section)
{
    KSharedConfig::Ptr profilesConfig = KSharedConfig::openConfig(QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig);
    KConfigGroup deviceGroup = KConfigGroup(profilesConfig, m_deviceName);
    KConfigGroup profileGroup = KConfigGroup(&deviceGroup, m_profileName);

    return KConfigGroup(&profileGroup, section);
}

void ProfileManagement::setProfileName(const QString & name)
{
    m_profileName = name;
}

QString ProfileManagement::profileName() const
{
    return m_profileName;
}

ProfileManagement::PadButton ProfileManagement::getPadButtonFunction(const QString & buttonParam)
{
    bool isNumber;
    buttonParam.toInt(&isNumber);
    if (isNumber) {
        return Pad_Button;
    }

    if (buttonParam.contains( QLatin1String( "quotedbl" ), Qt::CaseInsensitive)) {
        return Pad_QuoteDbl;
    }

    if (buttonParam.contains( QLatin1String( "core key" ), Qt::CaseInsensitive)) {
        return Pad_Keystroke;
    }

    return Pad_Disable;
}

ProfileManagement::PenButton ProfileManagement::getPenButtonFunction(const QString & buttonParam)
{
    if (buttonParam.contains( QLatin1String( "dblclick 1" ))) {
        return Pen_DoubleClick;
    }
    
    if (buttonParam.contains( QLatin1String( "1" ), Qt::CaseInsensitive)) {
        return Pen_LeftClick;
    }

    if (buttonParam.contains( QLatin1String( "3" ), Qt::CaseInsensitive)) {
        return Pen_RightClick;
    }

    if (buttonParam.contains( QLatin1String( "2" ), Qt::CaseInsensitive)) {
        return Pen_MiddleClick;
    }

    bool isNumber;
    buttonParam.toInt(&isNumber);
    if (isNumber) {
        return Pen_Button;
    }

    if (buttonParam.contains( QLatin1String( "quotedbl" ), Qt::CaseInsensitive)) {
        return Pen_QuoteDbl;
    }

    if (buttonParam.contains( QLatin1String( "modetoggle" ), Qt::CaseInsensitive)) {
        return Pen_ModeToggle;
    }

    if (buttonParam.contains( QLatin1String( "displaytoggle" ), Qt::CaseInsensitive)) {
        return Pen_DisplayToggle;
    }

    if (buttonParam.contains( QLatin1String( "screentoggle" ), Qt::CaseInsensitive)) {
        return Pen_ScreenToggle;
    }

    if (buttonParam.contains( QLatin1String( "core key" ), Qt::CaseInsensitive)) {
        return Pen_Keystroke;
    }

    return Pen_Disable;
}

QString ProfileManagement::transformButtonToConfig(PadButton mode, const QString & buttonParam)
{
    QString configString;
    switch (mode) {
    case Pad_Disable:
        configString.clear();
        break;
    case Pad_Button:
        configString = buttonParam;
        configString.remove(QLatin1String( "button " ), Qt::CaseInsensitive);
        break;
    case Pad_Keystroke:
        configString = QString::fromLatin1("core key %1").arg(buttonParam);
        configString.replace(QLatin1String( "\\s*" ), QLatin1String( " " ));
        configString = configString.toLower();
        break;
    case Pad_QuoteDbl:
        configString = QString::fromLatin1("core key quotedbl %1 quotedbl").arg(buttonParam);
        configString.replace(QLatin1Char( '"' ), QLatin1Char( '\"' ));
        break;
    }

    return configString;
}

QString ProfileManagement::transformButtonFromConfig(PadButton mode, QString & buttonParam)
{
    Q_UNUSED(mode);
    buttonParam.remove(QLatin1String( "core key" ), Qt::CaseInsensitive);
    buttonParam.remove(QRegExp( QLatin1String( "\\s?quotedbl\\s?" ), Qt::CaseInsensitive));
    buttonParam.remove(QLatin1String( "button " ), Qt::CaseInsensitive);
    return buttonParam;
}

QString ProfileManagement::transformButtonToConfig(PenButton mode, const QString & buttonParam)
{
    QString configString;
    switch (mode) {
    case Pen_Disable:
        configString.clear();
        break;
    case Pen_Button:
        configString = buttonParam;
        configString.remove(QLatin1String( "button " ), Qt::CaseInsensitive);
        break;
    case Pen_LeftClick:
        configString = QLatin1String("1");
        break;
    case Pen_MiddleClick:
        configString = QLatin1String("2");
        break;
    case Pen_RightClick:
        configString = QLatin1String("3");
        break;
    case Pen_DoubleClick:
        configString = QLatin1String("dblclick 1");
        break;
    case Pen_Keystroke:
        configString = QString::fromLatin1("core key %1").arg(buttonParam);
        configString = configString.toLower();
        break;
    case Pen_QuoteDbl:
        configString = QString::fromLatin1("core key quotedbl %1 quotedbl").arg(buttonParam);
        break;
    case Pen_ModeToggle:
        configString = QLatin1String("modetoggle");
        break;
    case Pen_ScreenToggle:
        configString = QLatin1String("screentoggle");
        break;
    case Pen_DisplayToggle:
        configString = QLatin1String("displaytoggle");
        break;
    }

    return configString;
}

QString ProfileManagement::transformButtonFromConfig(PenButton mode, QString & buttonParam)
{
    Q_UNUSED(mode);
    buttonParam.remove(QLatin1String( "core key" ), Qt::CaseInsensitive);
    buttonParam.replace(QRegExp( QLatin1String( "\\s?quotedbl\\s?" ), Qt::CaseInsensitive), QLatin1String( "\"" ));
    buttonParam.remove(QLatin1String( "button " ), Qt::CaseInsensitive);
    return buttonParam;
}

void ProfileManagement::reload()
{
    QDBusReply<QString> deviceName  = m_deviceInterface->call(QLatin1String( "deviceName" ));

    if (deviceName.isValid()) {
        m_deviceName = deviceName;
    }
}
