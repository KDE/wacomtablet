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

#include "tabletarea.h"
#include "screenarea.h"

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

    m_screenArea = new ScreenArea();

    m_ui->screenAreaBox->layout()->addWidget( m_screenArea );

    connect( m_screenArea, SIGNAL( selectedArea( QString ) ), this, SLOT( profileChanged() ) );
    connect(m_ui->mapToScreen, SIGNAL(clicked()), m_screenArea, SLOT(resetSelection()));
}

PadMapping::~PadMapping()
{
    delete m_ui;
}

void PadMapping::setTool( int tool )
{
    m_tool = tool;

    m_tabletArea = new TabletArea();

    QString toolName;
    if( tool == 0 ) {
        QDBusReply<QString> stylusName = m_deviceInterface->call( QLatin1String( "stylusName" ) );
        toolName = stylusName.value();
    }
    else if( tool == 1 ) {
        QDBusReply<QString> touchName = m_deviceInterface->call( QLatin1String( "touchName" ) );
        toolName = touchName.value();
    }

    m_tabletArea->setTool( toolName );
    m_ui->tabletAreaBox->layout()->addWidget( m_tabletArea );

    connect( m_tabletArea, SIGNAL( selectedArea( QString ) ), this, SLOT( profileChanged() ) );
    connect(m_ui->fullTablet, SIGNAL(clicked()), m_tabletArea, SLOT(resetSelection()));
}

void PadMapping::saveToProfile()
{
    if( m_tool == 0 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
        KConfigGroup eraserConfig = m_profileManagement->configGroup( QLatin1String( "eraser" ) );

        stylusConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );
        eraserConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

        if( m_ui->xrandrRotationCheckBox->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
            eraserConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
        }
        else {
            stylusConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
            eraserConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
        }

        if( !m_ui->screenComboBox->currentText().isEmpty()  || m_ui->screenComboBox->currentIndex() != 0 ) {
            stylusConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
            eraserConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
        }

        if( m_ui->fullTablet->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
            eraserConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
        }
        else {
            stylusConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
            eraserConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
        }

        stylusConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );
        eraserConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );

        stylusConfig.writeEntry( QLatin1String( "ScreenSpace" ), m_screenArea->getSelectedAreaString() );
        eraserConfig.writeEntry( QLatin1String( "ScreenSpace" ), m_screenArea->getSelectedAreaString() );

        stylusConfig.sync();
        eraserConfig.sync();
    }
    else if( m_tool == 1 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        KConfigGroup touchConfig = m_profileManagement->configGroup( QLatin1String( "touch" ) );

        touchConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

        if( m_ui->xrandrRotationCheckBox->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "RotateWithScreen" ), "false" );
        }

        if( !m_ui->screenComboBox->currentText().isEmpty()  || m_ui->screenComboBox->currentIndex() != 0 ) {
            touchConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
        }

        if( m_ui->fullTablet->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
        }

        touchConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );
        touchConfig.writeEntry( QLatin1String( "ScreenSpace" ), m_screenArea->getSelectedAreaString() );

        touchConfig.sync();
    }
}

void PadMapping::loadFromProfile()
{
    // read in from stylus.
    // values for stylus/eraser will be the same
    KConfigGroup config;

    if( m_tool == 0 ) {
        config = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
    }
    else {
        config = m_profileManagement->configGroup( QLatin1String( "touch" ) );
    }

    m_ui->rotationComboBox->setCurrentIndex( config.readEntry( QLatin1String( "Rotate" ) ).toInt() );

    if( config.readEntry( QLatin1String( "RotateWithScreen" ) ) == QLatin1String( "true" ) ) {
        m_ui->xrandrRotationCheckBox->setChecked( true );
    }

    int index = m_ui->screenComboBox->findText( config.readEntry( QLatin1String( "MapToOutput" ) ) );
    m_ui->screenComboBox->setCurrentIndex( index );

    if( config.readEntry( QLatin1String( "0ChangeArea" ) ) == QLatin1String( "true" ) ) {
        m_ui->partOfTablet->setChecked( true );
    }

    QString workingArea = config.readEntry( QLatin1String( "Area" ) );

    m_tabletArea->setSelection( workingArea );

    QString screenArea = config.readEntry( QLatin1String( "ScreenSpace" ) );
    m_screenArea->setSelection( screenArea );
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

    if( m_tool == 0 ) {
        QDBusReply<QString> stylusName = m_deviceInterface->call( QLatin1String( "stylusName" ) );
        toolName = stylusName.value();
    }
    else {
        QDBusReply<QString> stylusName = m_deviceInterface->call( QLatin1String( "touchName" ) );
        toolName = stylusName.value();
    }

    CalibrationDialog cdlg( toolName );

    cdlg.exec();

    QRect newCalibration = cdlg.calibratedArea();
    QString area = QString::fromLatin1( "%1 %2 %3 %4" )
                   .arg( newCalibration.x() )
                   .arg( newCalibration.y() )
                   .arg( newCalibration.width() )
                   .arg( newCalibration.height() );

    m_tabletArea->setSelection( area );

    if( oldstate ) {
        config.writeEntry( QLatin1String( "Enabled" ), true );
        mKWinConfig->sync();

        QDBusMessage message = QDBusMessage::createSignal( QLatin1String( "/KWin" ),
                               QLatin1String( "org.kde.KWin" ),
                               QLatin1String( "reloadConfig" ) );
        QDBusConnection::sessionBus().send( message );
    }
}
