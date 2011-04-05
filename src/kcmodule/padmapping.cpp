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
#include <KDE/KLocalizedString>

//Qt includes
#include <QtCore/QStringList>
#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtGui/QX11Info>

// X11 includes
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

using namespace Wacom;

PadMapping::PadMapping( QDBusInterface *deviceInterface, ProfileManagement *profileManager, QWidget *parent )
    : QWidget( parent ),
      m_ui( new Ui::PadMapping ),
      m_deviceInterface( deviceInterface ),
      m_profileManagement( profileManager ),
      m_tabletArea( 0 )
{
    m_ui->setupUi( this );

    m_screenArea = new ScreenArea();

    m_ui->screenAreaBox->layout()->addWidget( m_screenArea );

    connect( m_screenArea, SIGNAL( selectedArea( QString ) ), this, SLOT( profileChanged() ) );
    connect( m_screenArea, SIGNAL( selectedArea( QString ) ), this, SLOT( updateTabletArea() ) );
    connect( m_ui->forceProportions, SIGNAL( clicked() ), this, SLOT( updateTabletArea() ) );

    connect( m_ui->forceProportions, SIGNAL( clicked( bool ) ), this, SLOT( setForceProportions( bool ) ) );
    connect( m_ui->fullTablet, SIGNAL( clicked( bool ) ), this, SLOT( setFullTabletUsage( bool ) ) );

    connect(m_ui->fullScreen, SIGNAL( toggled( bool ) ), this, SLOT(setFullScreenUsage(bool)));
    connect(m_ui->mapToScreen, SIGNAL( toggled( bool ) ), this, SLOT(setMapToScreenUsage(bool)));
    connect(m_ui->mapToScreen, SIGNAL( toggled( bool ) ), m_ui->screenComboBox, SLOT(setEnabled(bool)));
    connect(m_ui->screenComboBox, SIGNAL(currentIndexChanged(int)), m_screenArea, SLOT(setScreenNumber(int)));
    connect(m_ui->partOfScreen, SIGNAL( toggled( bool ) ), this, SLOT(setPartOfScreenUsage(bool)));
}

PadMapping::~PadMapping()
{
    delete m_ui;
}

void PadMapping::setTool( int tool )
{
    m_tool = tool;

    delete m_tabletArea;
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
    connect( m_ui->fullTablet, SIGNAL( clicked() ), m_tabletArea, SLOT( resetSelection() ) );

    connect( m_ui->fullTablet, SIGNAL( toggled( bool ) ), m_ui->tabletAreaBox, SLOT( setDisabled( bool ) ) );
    connect( m_tabletArea, SIGNAL( sizeChanged( bool ) ), m_ui->forceProportions, SLOT( setChecked( bool ) ) );
    connect( m_ui->forceProportions, SIGNAL( toggled( bool ) ), m_ui->tabletAreaBox, SLOT( setDisabled( bool ) ) );
}

void PadMapping::saveToProfile()
{
    if( m_tool == 0 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
        KConfigGroup eraserConfig = m_profileManagement->configGroup( QLatin1String( "eraser" ) );

        // ##################
        // Orientation
        // ##################
        stylusConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );
        eraserConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

        if( m_ui->xrandrRotationCheckBox->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "true" );
            eraserConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "true" );
        }
        else {
            stylusConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "false" );
            eraserConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "false" );
        }

        // ##################
        // Screen Area
        // ##################
        if( m_ui->fullScreen->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "full" );
            eraserConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "full" );
        }
        else if( m_ui->mapToScreen->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "map" );
            eraserConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "map" );

            stylusConfig.writeEntry( QLatin1String( "0MapToOutput" ), m_ui->screenComboBox->currentIndex() );
            eraserConfig.writeEntry( QLatin1String( "0MapToOutput" ), m_ui->screenComboBox->currentIndex() );
        }
        else if( m_ui->partOfScreen->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "part" );
            eraserConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "part" );
        }

        stylusConfig.writeEntry( QLatin1String( "0ScreenSpace" ), m_screenArea->getSelectedAreaString() );
        eraserConfig.writeEntry( QLatin1String( "0ScreenSpace" ), m_screenArea->getSelectedAreaString() );

        // ##################
        // Tablet Area
        // ##################
        if( m_ui->fullTablet->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0TabletArea" ), "full" );
            eraserConfig.writeEntry( QLatin1String( "0TabletArea" ), "full" );
        }
        else {
            stylusConfig.writeEntry( QLatin1String( "0TabletArea" ), "part" );
            eraserConfig.writeEntry( QLatin1String( "0TabletArea" ), "part" );
        }

        stylusConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );
        eraserConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );

        if( m_ui->forceProportions->isChecked() ) {
            stylusConfig.writeEntry( QLatin1String( "0ForceProportions" ), "true" );
            eraserConfig.writeEntry( QLatin1String( "0ForceProportions" ), "true" );
        }
        else {
            stylusConfig.writeEntry( QLatin1String( "0ForceProportions" ), "false" );
            eraserConfig.writeEntry( QLatin1String( "0ForceProportions" ), "false" );
        }

        stylusConfig.sync();
        eraserConfig.sync();
    }
    else if( m_tool == 1 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        KConfigGroup touchConfig = m_profileManagement->configGroup( QLatin1String( "touch" ) );

        // ##################
        // Orientation
        // ##################
        touchConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );

        if( m_ui->xrandrRotationCheckBox->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "0RotateWithScreen" ), "false" );
        }

        // ##################
        // Screen Area
        // ##################
        if( m_ui->fullScreen->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "full" );
        }
        else if( m_ui->mapToScreen->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "map" );

            touchConfig.writeEntry( QLatin1String( "0MapToOutput" ), m_ui->screenComboBox->currentIndex() );
        }
        else if( m_ui->partOfScreen->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ScreenMapping" ), "part" );
        }

        touchConfig.writeEntry( QLatin1String( "0ScreenSpace" ), m_screenArea->getSelectedAreaString() );

        // ##################
        // Tablet Area
        // ##################
        if( m_ui->fullTablet->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
        }

        touchConfig.writeEntry( QLatin1String( "Area" ), m_tabletArea->getSelectedAreaString() );

        if( m_ui->forceProportions->isChecked() ) {
            touchConfig.writeEntry( QLatin1String( "0ForceProportions" ), "true" );
        }
        else {
            touchConfig.writeEntry( QLatin1String( "0ForceProportions" ), "false" );
        }

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

    // ##################
    // Orientation
    // ##################
    m_ui->rotationComboBox->setCurrentIndex( config.readEntry( QLatin1String( "Rotate" ) ).toInt() );

    if( config.readEntry( QLatin1String( "0RotateWithScreen" ) ) == QLatin1String( "true" ) ) {
        m_ui->xrandrRotationCheckBox->setChecked( true );
    }

    // ##################
    // Screen Area
    // ##################

    if( config.readEntry( QLatin1String( "0ScreenMapping" ) ) == QLatin1String( "full" ) ) {
        m_ui->fullScreen->setChecked( true );
        m_ui->screenAreaBox->setEnabled( false );
        m_screenArea->resetSelection();
    }
    else if( config.readEntry( QLatin1String( "0ScreenMapping" ) ) == QLatin1String( "map" ) ) {
        m_ui->mapToScreen->setChecked( true );
        m_ui->screenAreaBox->setEnabled( false );
        int screen = config.readEntry( QLatin1String( "0MapToOutput" ) ).toInt();
        m_ui->screenComboBox->setCurrentIndex( screen );
        m_screenArea->setSelection( config.readEntry( QLatin1String( "0ScreenSpace" ) ) );
    }
    else if( config.readEntry( QLatin1String( "0ScreenMapping" ) ) == QLatin1String( "part" ) ) {
        m_ui->partOfScreen->setChecked( true );
        m_ui->screenAreaBox->setEnabled( true );
        m_screenArea->setSelection( config.readEntry( QLatin1String( "0ScreenSpace" ) ) );
    }

    QString screenArea = config.readEntry( QLatin1String( "0ScreenSpace" ) );
    m_screenArea->setSelection( screenArea );

    // ##################
    // Tablet Area
    // ##################

    if( config.readEntry( QLatin1String( "0TabletArea" ) ) == QLatin1String( "full" ) ) {
        m_ui->fullTablet->setChecked( true );
        m_ui->tabletAreaBox->setEnabled( false );
    }

    if( config.readEntry( QLatin1String( "0ForceProportions" ) ) == QLatin1String( "true" ) ) {
        m_ui->forceProportions->setChecked( true );
        m_ui->partOfTablet->setChecked( true ); // this disables fullTablet, both is not possible
        m_ui->tabletAreaBox->setEnabled( true );
    }

    QString workingArea = config.readEntry( QLatin1String( "Area" ) );
    m_tabletArea->setSelection( workingArea );
}

void PadMapping::profileChanged()
{
    emit changed();
}

void PadMapping::reloadWidget()
{
    // get a list of all available screens.
    int num = QApplication::desktop()->numScreens();

    m_ui->screenComboBox->blockSignals( true );
    for( int i = 0; i < num; ++i ) {
        m_ui->screenComboBox->addItem( i18n( "Screen %1" ).arg( i+1 ) );
    }
    m_ui->screenComboBox->blockSignals( false );
}

void PadMapping::showCalibrationDialog()
{
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

    m_ui->forceProportions->setChecked( false );
    m_tabletArea->setSelection( area );
}

void PadMapping::updateTabletArea()
{
    // check if force proportions is set
    if( m_ui->forceProportions->isChecked() ) {
        QRect screenArea = m_screenArea->getSelectedArea();
        QRect maxTabletArea = m_tabletArea->getOriginalArea();

        qreal scale;
        qreal maxHeight;
        qreal maxWidth;

        if( screenArea.width() > screenArea.height() ) {
            // I assume all tablets have a higher width than height
            scale = ( qreal )maxTabletArea.width() / ( qreal )screenArea.width();
            maxHeight = ( qreal )screenArea.height() * scale;
            maxWidth = maxTabletArea.width();

            if( maxHeight > maxTabletArea.height() ) {
                scale = ( qreal )maxTabletArea.height() / ( qreal )screenArea.height();
                maxWidth = ( qreal )screenArea.width() * scale;
                maxHeight = maxTabletArea.height();
            }

            m_tabletArea->setSelection( maxTabletArea.width(), maxHeight );

        }
        else {
            scale = ( qreal )maxTabletArea.height() / ( qreal )screenArea.height();
            maxWidth = ( qreal )screenArea.width() * scale;
            maxHeight = maxTabletArea.height();

            if( maxWidth > maxTabletArea.width() ) {
                scale = ( qreal )maxTabletArea.width() / ( qreal )screenArea.width();
                maxHeight = ( qreal )screenArea.height() * scale;
                maxWidth = maxTabletArea.width();
            }

        }

        m_tabletArea->setSelection( maxWidth, maxHeight );
    }
}

void PadMapping::setFullTabletUsage( bool useFullArea )
{
    if( useFullArea ) {
        m_ui->forceProportions->setChecked( false );
        m_ui->tabletAreaBox->setEnabled( false );
    }
}

void PadMapping::setForceProportions( bool useProportionalArea )
{
    if( useProportionalArea ) {
        m_ui->partOfTablet->setChecked( true );
        m_ui->tabletAreaBox->setEnabled( true );
    }
}

void PadMapping::setFullScreenUsage( bool fullScreen )
{
    if( fullScreen ) {
        m_ui->screenAreaBox->setEnabled( false );
        m_screenArea->resetSelection();
    }
}

void PadMapping::setMapToScreenUsage( bool mapToScreen )
{
    if( mapToScreen ) {
        m_ui->screenAreaBox->setEnabled( false );
        m_screenArea->setScreenNumber( m_ui->screenComboBox->currentIndex() );
    }
}

void PadMapping::setPartOfScreenUsage( bool partOfScreen )
{
    if( partOfScreen ) {
        m_ui->screenAreaBox->setEnabled( true );
    }
}
