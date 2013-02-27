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

#include "penwidget.h"
#include "ui_penwidget.h"

#include "buttonactionselectiondialog.h"
#include "presscurvedialog.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "buttonshortcut.h"

// stdlib
#include <memory>

//KDE includes
#include <KDE/KStandardDirs>
#include <KDE/KGlobalAccel>
#include <kglobalshortcutinfo.h>
#include <KDE/KDebug>

//Qt includes
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtCore/QPointer>
#include <QtDBus/QDBusReply>

using namespace Wacom;

/*
 * Static class members.
 */
const char* PenWidget::LABEL_PROPERTY_KEYSEQUENCE = "KeySequence";

/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class PenWidgetPrivate {
        public:
            std::auto_ptr<Ui::PenWidget>  ui;                /**< Handler to the penwidget.ui file */
    };
} // NAMESPACE


PenWidget::PenWidget( QWidget* parent )
    : QWidget( parent ), d_ptr(new PenWidgetPrivate)
{
    Q_D( PenWidget );

    d->ui = std::auto_ptr<Ui::PenWidget>(new Ui::PenWidget);
    d->ui->setupUi( this );

    d->ui->penLabel->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/pen.png"))));

    connect ( d->ui->button2ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onButtonActionChanged()) );
    connect ( d->ui->button3ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onButtonActionChanged()) );
}

PenWidget::~PenWidget()
{
    delete this->d_ptr;
}


void PenWidget::saveToProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser feel / tip feel
    eraserProfile.setProperty( Property::Threshold, QString::number(d->ui->eraserSlider->value()) );
    eraserProfile.setProperty( Property::PressureCurve, d->ui->eraserPressureButton->property( "curve" ).toString() );
    stylusProfile.setProperty( Property::Threshold, QString::number(d->ui->tipSlider->value()) );
    stylusProfile.setProperty( Property::PressureCurve, d->ui->tipPressureButton->property( "curve" ).toString() );

    // button 2 and 3 config
    eraserProfile.setProperty( Property::Button2, d->ui->button2ActionSelector->getShortcut().toString() );
    eraserProfile.setProperty( Property::Button3, d->ui->button3ActionSelector->getShortcut().toString() );
    stylusProfile.setProperty( Property::Button2, d->ui->button2ActionSelector->getShortcut().toString() );
    stylusProfile.setProperty( Property::Button3, d->ui->button3ActionSelector->getShortcut().toString() );

    //stylusProfile.setProperty( "DoubleClickInterval", ui->doubleClickSlider->value() );
    //eraserProfile.setProperty( "DoubleClickInterval", ui->doubleClickSlider->value() );

    if( d->ui->radioButton_Absolute->isChecked() ) {
        stylusProfile.setProperty( Property::Mode, QLatin1String("absolute") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("absolute") );
    }
    else {
        stylusProfile.setProperty( Property::Mode, QLatin1String("relative") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("relative") );
    }

    if( d->ui->tpcCheckBox->isChecked() ) {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("on") );
    }
    else {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("off") );
    }

    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);
}



void PenWidget::loadFromProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );
    DeviceProfile cursorProfile = profileManagement->loadDeviceProfile( DeviceType::Cursor );

    // eraser feel / tip feel
    d->ui->eraserSlider->setValue( eraserProfile.getProperty( Property::Threshold ).toInt() );
    d->ui->eraserPressureButton->setProperty( "curve", eraserProfile.getProperty( Property::PressureCurve ) );
    d->ui->tipSlider->setValue( stylusProfile.getProperty( Property::Threshold ).toInt() );
    d->ui->tipPressureButton->setProperty( "curve", stylusProfile.getProperty( Property::PressureCurve ) );

    // Button Actions
    QString propertyValue;

    propertyValue = stylusProfile.getProperty( Property::Button2 );
    d->ui->button2ActionSelector->setShortcut(ButtonShortcut(propertyValue));

    propertyValue = stylusProfile.getProperty( Property::Button3 );
    d->ui->button3ActionSelector->setShortcut(ButtonShortcut(propertyValue));

    // Cursor Settings
    if( stylusProfile.getProperty( Property::Mode ).toInt() == 1 || stylusProfile.getProperty( Property::Mode ) == QLatin1String( "absolute" ) ) {
        d->ui->radioButton_Absolute->setChecked( true );
        d->ui->radioButton_Relative->setChecked( false );
    }
    else {
        d->ui->radioButton_Absolute->setChecked( false );
        d->ui->radioButton_Relative->setChecked( true );
    }

    // Hover Settings
    QString tabletPCButton = stylusProfile.getProperty( Property::TabletPcButton );
    if( tabletPCButton == QLatin1String( "on" ) ) {
        d->ui->tpcCheckBox->setChecked( true );
    }
    else {
        d->ui->tpcCheckBox->setChecked( false );
    }
}

void PenWidget::profileChanged()
{
    emit changed();
}

void PenWidget::reloadWidget()
{
}

void PenWidget::changeEraserPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Eraser, d->ui->eraserPressureButton->property( "curve" ).toString());
    d->ui->eraserPressureButton->setProperty( "curve", result );
}

void PenWidget::changeTipPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Stylus, d->ui->tipPressureButton->property( "curve" ).toString());
    d->ui->tipPressureButton->setProperty( "curve", result );
}


void PenWidget::onButtonActionChanged()
{
    emit changed();
}


QString PenWidget::changePressCurve(const DeviceType& deviceType, const QString& startValue)
{
    QString              result(startValue);
    QPointer<PressCurveDialog> selectPC = new PressCurveDialog(this);

    selectPC->setDeviceType(deviceType);
    selectPC->setControllPoints( startValue );

    if( selectPC->exec() == KDialog::Accepted ) {
        result = selectPC->getControllPoints();
        emit changed();
    }
    else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        DBusTabletInterface::instance().setProperty( deviceType, Property::PressureCurve, startValue );
    }

    delete selectPC;
    return result;
}


