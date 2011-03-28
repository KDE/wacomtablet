/*
 * Copyright 2009,2010, 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "padmapping.h"
#include "ui_padmapping.h"

#include "profilemanagement.h"
#include "calibrationdialog.h"

//KDE includes
#include <KDE/KDebug>
#include <KDE/KSharedConfigPtr>
#include <KDE/KConfigGroup>

//Qt includes
#include <QtCore/QStringList>
#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <QtCore/QDebug>

#include <QX11Info>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
}

using namespace Wacom;

PadMapping::PadMapping( QDBusInterface *deviceInterface, ProfileManagement *profileManager, QWidget *parent )
    : QWidget( parent ),
      m_ui( new Ui::PadMapping ),
      m_deviceInterface( deviceInterface ),
      m_profileManagement( profileManager )
{
    m_ui->setupUi( this );
}

PadMapping::~PadMapping()
{
    delete m_ui;
}

void PadMapping::saveToProfile()
{
    // save current calibration area to the right temp rect
    if( m_ui->toolCombobox->currentIndex() == 0 ) {
        m_stylusArea.setX( m_ui->topX->value() );
        m_stylusArea.setY( m_ui->topY->value() );
        m_stylusArea.setWidth( m_ui->bottomX->value() );
        m_stylusArea.setHeight( m_ui->bottomY->value() );
    }
    else {
        m_touchArea.setX( m_ui->topX->value() );
        m_touchArea.setY( m_ui->topY->value() );
        m_touchArea.setWidth( m_ui->bottomX->value() );
        m_touchArea.setHeight( m_ui->bottomY->value() );
    }

    // read in from stylus.
    // values for stylus/eraser/touch will be the same
    KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
    KConfigGroup eraserConfig = m_profileManagement->configGroup( QLatin1String( "eraser" ) );

    stylusConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );
    eraserConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

    if( !m_ui->screenComboBox->currentText().isEmpty() ) {
        stylusConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
        eraserConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
    }

    if( m_ui->xrandrRotationCheckBox->isChecked() ) {
        stylusConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
        eraserConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
    }
    else {
        stylusConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
        eraserConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
    }

    if( m_ui->workingAreaBox->isChecked() ) {
        stylusConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
        eraserConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
    }
    else {
        stylusConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
        eraserConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
    }

    stylusConfig.writeEntry( QLatin1String( "Area" ), QString::fromLatin1( "%1 %2 %3 %4" )
                             .arg( m_stylusArea.x() ).arg( m_stylusArea.y() )
                             .arg( m_stylusArea.width() ).arg( m_stylusArea.height() ) );
    eraserConfig.writeEntry( QLatin1String( "Area" ), QString::fromLatin1( "%1 %2 %3 %4" )
                             .arg( m_stylusArea.x() ).arg( m_stylusArea.y() )
                             .arg( m_stylusArea.width() ).arg( m_stylusArea.height() ) );

    stylusConfig.sync();
    eraserConfig.sync();

    // if we have a touch device, update these values too
    QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );

    QString validName = touchName.value();
    if( !validName.isEmpty() ) {
        KConfigGroup touchConfig = m_profileManagement->configGroup( QLatin1String( "touch" ) );
        touchConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

        if( !m_ui->screenComboBox->currentText().isEmpty() ) {
            touchConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
        }

        if( m_ui->xrandrRotationCheckBox->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
        }

        if( m_ui->workingAreaBox->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
        }

        touchConfig.writeEntry( QLatin1String( "Area" ), QString::fromLatin1( "%1 %2 %3 %4" )
                                .arg( m_touchArea.x() ).arg( m_touchArea.y() )
                                .arg( m_touchArea.width() ).arg( m_touchArea.height() ) );

        touchConfig.sync();
    }
}

void PadMapping::loadFromProfile()
{
    // read in from stylus.
    // values for stylus/eraser/touch will be the same
    KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );

    m_ui->rotationComboBox->setCurrentIndex( stylusConfig.readEntry( QLatin1String( "Rotate" ) ).toInt() );

    if( stylusConfig.readEntry( QLatin1String( "RotateWithScreen" ) ) == QLatin1String( "true" ) ) {
        m_ui->xrandrRotationCheckBox->setChecked( true );
    }

    int index = m_ui->screenComboBox->findText( stylusConfig.readEntry( QLatin1String( "MapToOutput" ) ) );
    m_ui->screenComboBox->setCurrentIndex( index );


    if( stylusConfig.readEntry( QLatin1String( "0ChangeArea" ) ) == QLatin1String( "true" ) ) {
        m_ui->workingAreaBox->setChecked( true );
    }

    // check if we have a touch tool available
    // this means we want to calibrate the tablet area in two different ways
    QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );

    QString validName = touchName.value();
    if( !validName.isEmpty() ) {
        KConfigGroup touchConfig = m_profileManagement->configGroup( QLatin1String( "touch" ) );

        QString workingArea = touchConfig.readEntry( QLatin1String( "Area" ) );
        QStringList waList = workingArea.split( QLatin1String( " " ) );

        if( waList.size() != 4 ) {
            kDebug() << "Error while parsing touch Area settings. Entry must have 4 ints seperated by a space. For example: 0 0 100 100";
            return;
        }

        m_touchArea.setX( waList.at( 0 ).toInt() );
        m_touchArea.setY( waList.at( 1 ).toInt() );
        m_touchArea.setWidth( waList.at( 2 ).toInt() );
        m_touchArea.setHeight( waList.at( 3 ).toInt() );
    }

    QString workingArea = stylusConfig.readEntry( QLatin1String( "Area" ) );
    QStringList waList = workingArea.split( QLatin1String( " " ) );

    if( waList.size() != 4 ) {
        kDebug() << "Error while parsing Area settings. Entry must have 4 ints seperated by a space. For example: 0 0 100 100";
        return;
    }

    m_stylusArea.setX( waList.at( 0 ).toInt() );
    m_stylusArea.setY( waList.at( 1 ).toInt() );
    m_stylusArea.setWidth( waList.at( 2 ).toInt() );
    m_stylusArea.setHeight( waList.at( 3 ).toInt() );

    // default to stylus/areaser area
    m_ui->topX->setValue( m_stylusArea.x() );
    m_ui->topY->setValue( m_stylusArea.y() );
    m_ui->bottomX->setValue( m_stylusArea.width() );
    m_ui->bottomY->setValue( m_stylusArea.height() );
}

void PadMapping::profileChanged()
{
    emit changed();
}

void PadMapping::reloadWidget()
{
    // get a list of all available XRandR screens.
    XRRScreenResources *sr = XRRGetScreenResources( QX11Info::display(), RootWindow( QX11Info::display(), DefaultScreen( QX11Info::display() ) ) );

    m_ui->screenComboBox->blockSignals( true );
    for( int i = 0; i < sr->noutput; ++i ) {
        XRROutputInfo *output = XRRGetOutputInfo( QX11Info::display(), sr, sr->outputs[i] );
        m_ui->screenComboBox->addItem( QString::fromLatin1( output->name ) );
        XRRFreeOutputInfo( output );
    }
    m_ui->screenComboBox->blockSignals( false );
    XRRFreeScreenResources( sr );

    // check if the connected tabled has touch features
    // hides the tool selection box if no touch is available
    // helps to reduce the ui clutter for "normal" tablets
    QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );

    QString validName = touchName.value();
    if( validName.isEmpty() ) {
        m_ui->toolLabel->hide();
        m_ui->toolCombobox->hide();
    }
}

void PadMapping::showCalibrationDialog()
{
    // disable compositing if available
    KSharedConfigPtr mKWinConfig = KSharedConfig::openConfig( QLatin1String( "kwinrc" ) );
    KConfigGroup config( mKWinConfig, QLatin1String( "Compositing" ) );
    bool oldstate = config.readEntry( QLatin1String( "Enabled" ), false );

    if( oldstate ) {
        config.writeEntry( QLatin1String( "Enabled" ), false );
        mKWinConfig->sync();

        QDBusMessage message = QDBusMessage::createSignal( QLatin1String( "/KWin" ),
                               QLatin1String( "org.kde.KWin" ),
                               QLatin1String( "reloadConfig" ) );
        QDBusConnection::sessionBus().send( message );
    }

    QString toolName;
    if( m_ui->toolCombobox->currentIndex() == 0 ) {
        QDBusReply<QString> stylusName = m_deviceInterface->call( QLatin1String( "stylusName" ) );
        toolName = stylusName.value();
    }
    else {
        QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );
        toolName = touchName.value();
    }

    CalibrationDialog cdlg( toolName );

    cdlg.exec();

    QRect newCalibration = cdlg.calibratedArea();
    m_ui->topX->setValue( newCalibration.x() );
    m_ui->topY->setValue( newCalibration.y() );
    m_ui->bottomX->setValue( newCalibration.width() );
    m_ui->bottomY->setValue( newCalibration.height() );


    if( oldstate ) {
        config.writeEntry( QLatin1String( "Enabled" ), true );
        mKWinConfig->sync();

        QDBusMessage message = QDBusMessage::createSignal( QLatin1String( "/KWin" ),
                               QLatin1String( "org.kde.KWin" ),
                               QLatin1String( "reloadConfig" ) );
        QDBusConnection::sessionBus().send( message );
    }
}

void PadMapping::switchCalibrationTool()
{
    if( m_ui->toolCombobox->currentIndex() == 1 ) {
        m_stylusArea.setX( m_ui->topX->value() );
        m_stylusArea.setY( m_ui->topY->value() );
        m_stylusArea.setWidth( m_ui->bottomX->value() );
        m_stylusArea.setHeight( m_ui->bottomY->value() );

        m_ui->topX->setValue( m_touchArea.x());
        m_ui->topY->setValue( m_touchArea.y());
        m_ui->bottomX->setValue( m_touchArea.width());
        m_ui->bottomY->setValue( m_touchArea.height());
    }
    else {
        m_touchArea.setX( m_ui->topX->value() );
        m_touchArea.setY( m_ui->topY->value() );
        m_touchArea.setWidth( m_ui->bottomX->value() );
        m_touchArea.setHeight( m_ui->bottomY->value() );

        m_ui->topX->setValue( m_stylusArea.x());
        m_ui->topY->setValue( m_stylusArea.y());
        m_ui->bottomX->setValue( m_stylusArea.width());
        m_ui->bottomY->setValue( m_stylusArea.height());
    }
}
