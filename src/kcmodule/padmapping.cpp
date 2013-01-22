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

#include "padmapping.h"
#include "ui_padmapping.h"

#include "profilemanagement.h"
#include "calibrationdialog.h"

#include "tabletarea.h"
#include "screenarea.h"

// common includes
#include "property.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "stringutils.h"

// stdlib
#include <memory>

//KDE includes
#include <KDE/KDebug>
#include <KDE/KLocalizedString>

//Qt includes
#include <QtCore/QPointer>
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

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class PadMappingPrivate {
    public:
        int                             m_tool;              /**< The used tool for this widget (0=stylus/eraser and 1=touch tool) */
        std::auto_ptr<Ui::PadMapping>   m_ui;                /**< Handler to the padmapping.ui file */
        QPointer<TabletArea>            m_tabletArea;        /**< Widget to interactively change the used tablet area */
        QPointer<ScreenArea>            m_screenArea;        /**< Widget to interactively change the used screen area */

        QString                         m_tabletAreaFull;
        QString                         m_tabletAreaMapPart;
        QString                         m_tabletAreaMap0;
        QString                         m_tabletAreaMap1;
}; // CLASS
}  // NAMESPACE


PadMapping::PadMapping( QWidget *parent )
    : QWidget( parent ), d_ptr(new PadMappingPrivate)
{
    Q_D( PadMapping );

    d->m_ui = std::auto_ptr<Ui::PadMapping>(new Ui::PadMapping);
    d->m_ui->setupUi( this );

    d->m_screenArea = new ScreenArea();

    d->m_ui->screenAreaBox->layout()->addWidget( d->m_screenArea );

    connect( d->m_screenArea, SIGNAL( selectedArea( QString ) ), this, SLOT( profileChanged() ) );
    connect( d->m_screenArea, SIGNAL( selectedArea( QString ) ), this, SLOT( updateTabletArea() ) );

    connect(d->m_ui->fullScreen, SIGNAL( toggled( bool ) ), this, SLOT(updateTabletArea()));
    connect(d->m_ui->mapToScreen, SIGNAL( toggled( bool ) ), this, SLOT(updateTabletArea()));
    connect(d->m_ui->screenComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTabletArea()));
    connect(d->m_ui->partOfScreen, SIGNAL( toggled( bool ) ), this, SLOT(updateTabletArea()));

    connect( d->m_ui->forceProportions, SIGNAL( clicked( ) ), this, SLOT( setForceProportions( ) ) );
    connect( d->m_ui->fullTablet, SIGNAL( clicked( bool ) ), this, SLOT( setFullTabletUsage( bool ) ) );

    connect(d->m_ui->fullScreen, SIGNAL( toggled( bool ) ), this, SLOT(setFullScreenUsage(bool)));
    connect(d->m_ui->mapToScreen, SIGNAL( toggled( bool ) ), this, SLOT(setMapToScreenUsage(bool)));
    connect(d->m_ui->mapToScreen, SIGNAL( toggled( bool ) ), d->m_ui->screenComboBox, SLOT(setEnabled(bool)));
    connect(d->m_ui->screenComboBox, SIGNAL(currentIndexChanged(int)), d->m_screenArea, SLOT(setScreenNumber(int)));
    connect(d->m_ui->partOfScreen, SIGNAL( toggled( bool ) ), this, SLOT(setPartOfScreenUsage(bool)));

    setTool(0);
}

PadMapping::~PadMapping()
{
    delete this->d_ptr;
}

void PadMapping::setTool( int tool )
{
    Q_D( PadMapping );

    d->m_tool = tool;

    delete d->m_tabletArea;
    d->m_tabletArea = new TabletArea();

    QString toolName;
    if( tool == 0 ) {
        QDBusReply<QString> stylusName = DBusTabletInterface::instance().getDeviceName(DeviceType::Stylus);
        toolName = stylusName.value();
    }
    else if( tool == 1 ) {
        QDBusReply<QString> touchName = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);
        toolName = touchName.value();
    }

    d->m_tabletArea->setTool( toolName );
    d->m_ui->tabletAreaBox->layout()->addWidget( d->m_tabletArea );

    connect( d->m_tabletArea, SIGNAL( selectedArea( QString ) ), this, SLOT( profileChanged() ) );
    connect( d->m_ui->fullTablet, SIGNAL( clicked() ), d->m_tabletArea, SLOT( resetSelection() ) );

    connect( d->m_ui->fullTablet, SIGNAL( toggled( bool ) ), d->m_ui->tabletAreaBox, SLOT( setDisabled( bool ) ) );
    connect( d->m_tabletArea, SIGNAL( sizeChanged( bool ) ), this, SLOT( saveTabletChanges() ) );
}

void PadMapping::saveToProfile()
{
    Q_D( PadMapping );

    QString propertyValue;

    if( d->m_tool == 0 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        DeviceProfile stylusProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Stylus );
        DeviceProfile eraserProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Eraser );

        // ##################
        // Orientation
        // ##################
        stylusProfile.setProperty( Property::Rotate, QString::number(d->m_ui->rotationComboBox->currentIndex()) );
        eraserProfile.setProperty( Property::Rotate, QString::number(d->m_ui->rotationComboBox->currentIndex()) );

        propertyValue = d->m_ui->xrandrRotationCheckBox->isChecked() ? QLatin1String("true") : QLatin1String("false");
        stylusProfile.setProperty( Property::RotateWithScreen, propertyValue );
        eraserProfile.setProperty( Property::RotateWithScreen, propertyValue );

        // ##################
        // Screen Area
        // ##################
        if( d->m_ui->fullScreen->isChecked() ) {
            stylusProfile.setProperty( Property::ScreenSpace, QLatin1String("full") );
            eraserProfile.setProperty( Property::ScreenSpace, QLatin1String("full") );
        }
        else if( d->m_ui->mapToScreen->isChecked() ) {
            stylusProfile.setProperty( Property::ScreenSpace, QLatin1String("map") + QString::number(d->m_ui->screenComboBox->currentIndex()) );
            eraserProfile.setProperty( Property::ScreenSpace, QLatin1String("map") + QString::number(d->m_ui->screenComboBox->currentIndex()) );
        }
        else if( d->m_ui->partOfScreen->isChecked() ) {
            stylusProfile.setProperty( Property::ScreenSpace, d->m_screenArea->getSelectedAreaString() );
            eraserProfile.setProperty( Property::ScreenSpace, d->m_screenArea->getSelectedAreaString() );
        }


        // ##################
        // Tablet Area
        // ##################


        // update the current selected area according to teh selected screen
        if(d->m_ui->fullScreen->isChecked()) {
            d->m_tabletAreaFull = d->m_tabletArea->getSelectedAreaString();
        }
        else if(d->m_ui->partOfScreen->isChecked()) {
            d->m_tabletAreaMapPart = d->m_tabletArea->getSelectedAreaString();
        }
        else  if(d->m_ui->mapToScreen->isChecked()) {
            if( d->m_ui->screenComboBox->currentIndex() == 0 ) {
                d->m_tabletAreaMap0 = d->m_tabletArea->getSelectedAreaString();
            }
            else {
                d->m_tabletAreaMap1 = d->m_tabletArea->getSelectedAreaString();
            }
        }

        // keep current selected area as default "Area" usually we copy these values over once teh screen switches
        stylusProfile.setProperty( Property::Area, d->m_tabletArea->getSelectedAreaString() );
        stylusProfile.setProperty( Property::AreaMapFull, d->m_tabletAreaFull );
        stylusProfile.setProperty( Property::AreaMapPart, d->m_tabletAreaMapPart );
        stylusProfile.setProperty( Property::AreaMap0, d->m_tabletAreaMap0 );
        stylusProfile.setProperty( Property::AreaMap1, d->m_tabletAreaMap1 );

        eraserProfile.setProperty( Property::Area, d->m_tabletArea->getSelectedAreaString() );
        eraserProfile.setProperty( Property::AreaMapFull, d->m_tabletAreaFull );
        eraserProfile.setProperty( Property::AreaMapPart, d->m_tabletAreaMapPart );
        eraserProfile.setProperty( Property::AreaMap0, d->m_tabletAreaMap0 );
        eraserProfile.setProperty( Property::AreaMap1, d->m_tabletAreaMap1 );

        ProfileManagement::instance().saveDeviceProfile(stylusProfile);
        ProfileManagement::instance().saveDeviceProfile(eraserProfile);

    } else if( d->m_tool == 1 ) {
        // read in from stylus.
        // values for stylus/eraser/touch will be the same
        DeviceProfile touchProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Touch );

        // ##################
        // Orientation
        // ##################
        touchProfile.setProperty( Property::Rotate, QString::number(d->m_ui->rotationComboBox->currentIndex()) );

        propertyValue = d->m_ui->xrandrRotationCheckBox->isChecked() ? QLatin1String("true") : QLatin1String("false");
        touchProfile.setProperty(Property::RotateWithScreen, propertyValue);

        // ##################
        // Screen Area
        // ##################
        if( d->m_ui->fullScreen->isChecked() ) {
            touchProfile.setProperty( Property::ScreenSpace, QLatin1String("full") );
        }
        else if( d->m_ui->mapToScreen->isChecked() ) {
            touchProfile.setProperty( Property::ScreenSpace, QLatin1String("map") + QString::number(d->m_ui->screenComboBox->currentIndex()) );
        }
        else if( d->m_ui->partOfScreen->isChecked() ) {
            touchProfile.setProperty( Property::ScreenSpace, d->m_screenArea->getSelectedAreaString() );
        }


        // ##################
        // Tablet Area
        // ##################
        // update the current selected area according to teh selected screen
        if(d->m_ui->fullScreen->isChecked()) {
            d->m_tabletAreaFull = d->m_tabletArea->getSelectedAreaString();
        }
        else if(d->m_ui->partOfScreen->isChecked()) {
            d->m_tabletAreaMapPart = d->m_tabletArea->getSelectedAreaString();
        }
        else  if(d->m_ui->mapToScreen->isChecked()) {
            if( d->m_ui->screenComboBox->currentIndex() == 0 ) {
                d->m_tabletAreaMap0 = d->m_tabletArea->getSelectedAreaString();
            }
            else {
                d->m_tabletAreaMap1 = d->m_tabletArea->getSelectedAreaString();
            }
        }

        // keep current selected area as default "Area" usually we copy these values over once teh screen switches
        touchProfile.setProperty( Property::Area, d->m_tabletArea->getSelectedAreaString() );
        touchProfile.setProperty( Property::AreaMapFull, d->m_tabletAreaFull );
        touchProfile.setProperty( Property::AreaMapPart, d->m_tabletAreaMapPart );
        touchProfile.setProperty( Property::AreaMap0, d->m_tabletAreaMap0 );
        touchProfile.setProperty( Property::AreaMap1, d->m_tabletAreaMap1 );

        ProfileManagement::instance().saveDeviceProfile(touchProfile);
    }
}

void PadMapping::loadFromProfile()
{
    Q_D( PadMapping );

    // read in from stylus.
    // values for stylus/eraser will be the same
    DeviceProfile deviceProfile;

    if( d->m_tool == 0 ) {
        deviceProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Stylus );
    }
    else {
        deviceProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Touch );
    }

    // ##################
    // Orientation
    // ##################
    d->m_ui->rotationComboBox->setCurrentIndex( deviceProfile.getProperty( Property::Rotate ).toInt() );

    if ( StringUtils::asBool(deviceProfile.getProperty (Property::RotateWithScreen))) {
        d->m_ui->xrandrRotationCheckBox->setChecked( true );
    }

    // ##################
    // Screen Area
    // ##################

    if( deviceProfile.getProperty( Property::ScreenSpace ).compare(QLatin1String( "full" ), Qt::CaseInsensitive) == 0 ) {
        d->m_ui->fullScreen->setChecked( true );
        d->m_ui->screenAreaBox->setEnabled( false );
        d->m_screenArea->resetSelection();
    }
    else if( deviceProfile.getProperty( Property::ScreenSpace ).compare(QLatin1String( "map0" ), Qt::CaseInsensitive) == 0 ) {
        d->m_ui->mapToScreen->setChecked( true );
        d->m_ui->screenAreaBox->setEnabled( false );
        d->m_ui->screenComboBox->setCurrentIndex( 0 );
        d->m_screenArea->setScreenNumber( 0 );
    }
    else if( deviceProfile.getProperty( Property::ScreenSpace ).compare(QLatin1String( "map1" ), Qt::CaseInsensitive) == 0 ) {
        d->m_ui->mapToScreen->setChecked( true );
        d->m_ui->screenAreaBox->setEnabled( false );
        d->m_ui->screenComboBox->setCurrentIndex( 1 );
        d->m_screenArea->setScreenNumber( 1 );
    }
    else {
        d->m_ui->partOfScreen->setChecked( true );
        d->m_ui->screenAreaBox->setEnabled( true );
        d->m_screenArea->setSelection( deviceProfile.getProperty( Property::ScreenSpace ) );
    }

    // ##################
    // Tablet Area
    // ##################

    if( deviceProfile.getProperty( Property::TabletArea ).compare(QLatin1String( "full" ), Qt::CaseInsensitive) == 0 ) {
        d->m_ui->fullTablet->setChecked( true );
        d->m_ui->tabletAreaBox->setEnabled( false );
    } else {
        d->m_ui->partOfTablet->setChecked (true);
        d->m_ui->tabletAreaBox->setEnabled (true);
    }

    // read in all different tablet area proportions
    d->m_tabletAreaFull = deviceProfile.getProperty( Property::AreaMapFull );
    if(d->m_tabletAreaFull.isEmpty()) {
        d->m_tabletAreaFull = d->m_tabletArea->getOriginalAreaString();
    }
    d->m_tabletAreaMapPart = deviceProfile.getProperty( Property::AreaMapPart );
    if(d->m_tabletAreaMapPart.isEmpty()) {
        d->m_tabletAreaMapPart = d->m_tabletArea->getOriginalAreaString();
    }
    d->m_tabletAreaMap0 = deviceProfile.getProperty( Property::AreaMap0 );
    if(d->m_tabletAreaMap0.isEmpty()) {
        d->m_tabletAreaMap0 = d->m_tabletArea->getOriginalAreaString();
    }
    d->m_tabletAreaMap1 = deviceProfile.getProperty( Property::AreaMap1 );
    if(d->m_tabletAreaMap1.isEmpty()) {
        d->m_tabletAreaMap1 = d->m_tabletArea->getOriginalAreaString();
    }

    // set the one that fits the current selected screen mapping
    updateTabletArea();
}

void PadMapping::profileChanged()
{
    emit changed();
}

void PadMapping::reloadWidget()
{
    Q_D( PadMapping );

    // get a list of all available screens.
    int num = QApplication::desktop()->numScreens();

    d->m_ui->screenComboBox->blockSignals( true );
    for( int i = 0; i < num; ++i ) {
        d->m_ui->screenComboBox->addItem( i18n( "Screen %1" ).arg( i+1 ) );
    }
    d->m_ui->screenComboBox->blockSignals( false );
}

void PadMapping::showCalibrationDialog()
{
    Q_D( PadMapping );

    QString toolName;

    if( d->m_tool == 0 ) {
        QDBusReply<QString> stylusName = DBusTabletInterface::instance().getDeviceName(DeviceType::Stylus);
        toolName = stylusName.value();
    }
    else {
        QDBusReply<QString> stylusName = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);
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

    d->m_tabletArea->setSelection( area );
}

void PadMapping::updateTabletArea()
{
    Q_D( PadMapping );

    // set the one that fits the current selected screen mapping

    // also update the groupbox description here
    // so the user gets a response, that the table tarea is ment only for the current screen mapping
    if(d->m_ui->fullScreen->isChecked()) {
        d->m_tabletArea->setSelection( d->m_tabletAreaFull );
        d->m_ui->groupBox_TabletArea->setTitle(i18n("Fullscreen TabletArea"));
    }
    else if(d->m_ui->partOfScreen->isChecked()) {
        d->m_tabletArea->setSelection( d->m_tabletAreaMapPart );
        d->m_ui->groupBox_TabletArea->setTitle(i18n("Part of screen TabletArea"));
    }
    else  if(d->m_ui->mapToScreen->isChecked()) {
        if( d->m_ui->screenComboBox->currentIndex() == 0 ) {
            d->m_tabletArea->setSelection( d->m_tabletAreaMap0 );
            d->m_ui->groupBox_TabletArea->setTitle(i18n("Screen 1 TabletArea"));
        }
        else {
            d->m_tabletArea->setSelection( d->m_tabletAreaMap1 );
            d->m_ui->groupBox_TabletArea->setTitle(i18n("Screen 2 TabletArea"));
        }
    }
}

void PadMapping::saveTabletChanges()
{
    Q_D( PadMapping );

    // set the one that fits the current selected screen mapping
    if(d->m_ui->fullScreen->isChecked()) {
        d->m_tabletAreaFull =  d->m_tabletArea->getSelectedAreaString();
    }
    else if(d->m_ui->partOfScreen->isChecked()) {
        d->m_tabletAreaMapPart =  d->m_tabletArea->getSelectedAreaString();
    }
    else  if(d->m_ui->mapToScreen->isChecked()) {
        if( d->m_ui->screenComboBox->currentIndex() == 0 ) {
            d->m_tabletAreaMap0 =  d->m_tabletArea->getSelectedAreaString();
        }
        else {
            d->m_tabletAreaMap1 =  d->m_tabletArea->getSelectedAreaString();
        }
    }
}

void PadMapping::setFullTabletUsage( bool useFullArea )
{
    Q_D( PadMapping );

    if( useFullArea ) {
        d->m_ui->forceProportions->setChecked( false );
        d->m_ui->tabletAreaBox->setEnabled( false );

        d->m_tabletArea->setSelection(d->m_tabletArea->getOriginalAreaString());
    }

    profileChanged();
}

void PadMapping::setForceProportions( )
{
    Q_D( PadMapping );

    d->m_ui->partOfTablet->setChecked( true );
    d->m_ui->tabletAreaBox->setEnabled( true );

    // set the table tarea to use proportional rectangle
    QRect screenArea = d->m_screenArea->getSelectedArea();
    QRect maxTabletArea = d->m_tabletArea->getOriginalArea();

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

        d->m_tabletArea->setSelection( maxTabletArea.width(), maxHeight );

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

    d->m_tabletArea->setSelection( maxWidth, maxHeight );

    profileChanged();
}

void PadMapping::setFullScreenUsage( bool fullScreen )
{
    Q_D( PadMapping );

    if( fullScreen ) {
        d->m_ui->screenAreaBox->setEnabled( false );
        d->m_screenArea->resetSelection();
    }

    profileChanged();
}

void PadMapping::setMapToScreenUsage( bool mapToScreen )
{
    Q_D( PadMapping );

    if( mapToScreen ) {
        d->m_ui->screenAreaBox->setEnabled( false );
        d->m_screenArea->setScreenNumber( d->m_ui->screenComboBox->currentIndex() );
    }

    profileChanged();
}

void PadMapping::setPartOfScreenUsage( bool partOfScreen )
{
    Q_D( PadMapping );

    if( partOfScreen ) {
        d->m_ui->screenAreaBox->setEnabled( true );
    }

    profileChanged();
}
