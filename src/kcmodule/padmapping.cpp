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

//Qt includes
#include <QtCore/QStringList>
#include <QtGui/QDialog>
#include <QtGui/QDesktopWidget>
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
        m_deviceInterface(deviceInterface),
      m_profileManagement( profileManager )
{
    m_ui->setupUi( this );

    XRRScreenResources *sr = XRRGetScreenResources( QX11Info::display(), RootWindow( QX11Info::display(), DefaultScreen( QX11Info::display() ) ) );

    for( int i = 0; i < sr->noutput; ++i ) {
        XRROutputInfo *output = XRRGetOutputInfo( QX11Info::display(), sr, sr->outputs[i] );
        m_ui->screenComboBox->addItem( QString::fromLatin1( output->name ) );
        XRRFreeOutputInfo( output );
    }
    XRRFreeScreenResources( sr );
}

PadMapping::~PadMapping()
{
    delete m_ui;
}

void PadMapping::saveToProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup( QLatin1String( "pad" ) );

    //@DEBUG write out rotation settings name not index numbers
    padConfig.writeEntry( QLatin1String( "Rotate" ), m_ui->rotationComboBox->currentIndex() );
    if( !m_ui->screenComboBox->currentText().isEmpty() ) {
        padConfig.writeEntry( QLatin1String( "MapToOutput" ), m_ui->screenComboBox->currentText() );
    }

    if( m_ui->workingAreaBox->isChecked() ) {
        padConfig.writeEntry( QLatin1String( "0ChangeArea" ), "true" );
    }
    else {
        padConfig.writeEntry( QLatin1String( "0ChangeArea" ), "false" );
    }

    padConfig.writeEntry( QLatin1String( "Area" ), QString::fromLatin1( "%1 %2 %3 %4" ).arg( m_ui->topX->value() ).arg( m_ui->topY->value() )
                          .arg( m_ui->bottomX->value() ).arg( m_ui->bottomY->value() ) );

    padConfig.sync();
}

void PadMapping::loadFromProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup( QLatin1String( "pad" ) );

    //@DEBUG write out rotation settings name not index numbers
    m_ui->rotationComboBox->setCurrentIndex( padConfig.readEntry( QLatin1String( "Rotate" ) ).toInt() );

    int index = m_ui->screenComboBox->findText( padConfig.readEntry( QLatin1String( "MapToOutput" ) ) );
    m_ui->screenComboBox->setCurrentIndex( index );


    if( padConfig.readEntry( QLatin1String( "0ChangeArea" ) ) == QLatin1String( "true" ) ) {
        m_ui->workingAreaBox->setChecked( true );
    }

    QString workingArea = padConfig.readEntry( QLatin1String( "Area" ) );
    QStringList waList = workingArea.split( QLatin1String( " " ) );

    if( waList.size() != 4 ) {
        kDebug() << "Error while parsing Area settings. Entry must have 4 ints seperated by a space. For example: 0 0 100 100";
        return;
    }

    m_ui->topX->setValue( waList.at( 0 ).toInt() );
    m_ui->topY->setValue( waList.at( 1 ).toInt() );
    m_ui->bottomX->setValue( waList.at( 2 ).toInt() );
    m_ui->bottomY->setValue( waList.at( 3 ).toInt() );
}

void PadMapping::profileChanged()
{
    emit changed();
}

void PadMapping::reloadWidget()
{
}

void PadMapping::showCalibrationDialog()
{
    QDBusReply<QString> padName = m_deviceInterface->call(QLatin1String( "padName" ));

    CalibrationDialog cdlg(padName);

    cdlg.exec();

    QRect newCalibration = cdlg.calibratedArea();
    m_ui->topX->setValue( newCalibration.x() );
    m_ui->topY->setValue( newCalibration.y() );
    m_ui->bottomX->setValue( newCalibration.width() );
    m_ui->bottomY->setValue( newCalibration.height() );
}
