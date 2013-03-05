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
 * D-Pointer class for private members.
 */
namespace Wacom {
    class PenWidgetPrivate {
        public:
            PenWidgetPrivate() : ui(new Ui::PenWidget) {}
            ~PenWidgetPrivate() {
                delete ui;
            }

            Ui::PenWidget* ui;
    };
} // NAMESPACE


PenWidget::PenWidget( QWidget* parent )
    : QWidget( parent ), d_ptr(new PenWidgetPrivate)
{
    setupUi();
}

PenWidget::~PenWidget()
{
    delete this->d_ptr;
}


void PenWidget::loadFromProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser feel / tip feel
    setPressureFeel  ( DeviceType::Eraser, eraserProfile.getProperty( Property::Threshold ) );
    setPressureCurve ( DeviceType::Eraser, eraserProfile.getProperty( Property::PressureCurve ) );
    setPressureFeel  ( DeviceType::Stylus, stylusProfile.getProperty( Property::Threshold ) );
    setPressureCurve ( DeviceType::Stylus, stylusProfile.getProperty( Property::PressureCurve ) );

    // Button Actions
    setButtonShortcut ( Property::Button2, stylusProfile.getProperty( Property::Button2 ) );
    setButtonShortcut ( Property::Button3, stylusProfile.getProperty( Property::Button3 ) );

    // Tracking Mode
    setTrackingMode ( stylusProfile.getProperty( Property::Mode ) );

    // Tap to Click
    setTabletPcButton ( stylusProfile.getProperty( Property::TabletPcButton ) );
}


void PenWidget::reloadWidget()
{
    // nothing to do
}


void PenWidget::saveToProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser / tip pressure
    eraserProfile.setProperty( Property::Threshold,     getPressureFeel(DeviceType::Eraser) );
    eraserProfile.setProperty( Property::PressureCurve, getPressureCurve(DeviceType::Eraser) );
    stylusProfile.setProperty( Property::Threshold,     getPressureFeel(DeviceType::Stylus) );
    stylusProfile.setProperty( Property::PressureCurve, getPressureCurve(DeviceType::Stylus) );

    // button 2 and 3 config
    eraserProfile.setProperty( Property::Button2, getButtonShortcut(Property::Button2) );
    eraserProfile.setProperty( Property::Button3, getButtonShortcut(Property::Button3) );
    stylusProfile.setProperty( Property::Button2, getButtonShortcut(Property::Button2) );
    stylusProfile.setProperty( Property::Button3, getButtonShortcut(Property::Button3) );

    // tracking mode
    stylusProfile.setProperty( Property::Mode, getTrackingMode() );
    eraserProfile.setProperty( Property::Mode, getTrackingMode() );

    // tap to click
    stylusProfile.setProperty( Property::TabletPcButton, getTabletPcButton() );

    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);
}


void PenWidget::onChangeEraserPressCurve()
{
    changePressCurve(DeviceType::Eraser);
}


void PenWidget::onChangeTipPressCurve()
{
    changePressCurve(DeviceType::Stylus);
}


void PenWidget::onProfileChanged()
{
    emit changed();
}


const QString PenWidget::getButtonShortcut(const Property& button) const
{
    Q_D( const PenWidget );

    ButtonShortcut shortcut;

    if (button == Property::Button2) {
        shortcut = d->ui->button2ActionSelector->getShortcut();
    } else if (button == Property::Button3) {
        shortcut = d->ui->button3ActionSelector->getShortcut();
    } else {
        kError() << QString::fromLatin1("Internal Error: Unknown button property '%1' provided!").arg(button.key());
    }

    return shortcut.toString();
}


const QString PenWidget::getPressureCurve(const DeviceType& type) const
{
    Q_D (const PenWidget);

    QString result;

    if (type == DeviceType::Stylus) {
        return d->ui->tipPressureButton->property( "curve" ).toString();

    } else if (type == DeviceType::Eraser) {
        return d->ui->eraserPressureButton->property( "curve" ).toString();

    } else {
        kError() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }

    return QString();
}


const QString PenWidget::getPressureFeel(const DeviceType& type) const
{
    Q_D (const PenWidget);

    if (type == DeviceType::Stylus) {
        return QString::number(d->ui->tipSlider->value());

    } else if (type == DeviceType::Eraser) {
        return QString::number(d->ui->eraserSlider->value());

    } else {
        kError() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }

    return QString();
}


const QString PenWidget::getTabletPcButton() const
{
    Q_D (const PenWidget);
    return (d->ui->tpcCheckBox->isChecked() ? QLatin1String("on") : QLatin1String("off"));
}


const QString PenWidget::getTrackingMode() const
{
    Q_D (const PenWidget);
    return (d->ui->radioButton_Absolute->isChecked() ? QLatin1String("absolute") : QLatin1String("relative"));
}


void PenWidget::setButtonShortcut(const Property& button, const QString& shortcut)
{
    Q_D( PenWidget );

    if (button == Property::Button2) {
        d->ui->button2ActionSelector->setShortcut(ButtonShortcut(shortcut));

    } else if (button == Property::Button3) {
        d->ui->button3ActionSelector->setShortcut(ButtonShortcut(shortcut));

    } else {
        kError() << QString::fromLatin1("Internal Error: Unknown button property '%1' provided!").arg(button.key());
    }
}


void PenWidget::setPressureCurve(const DeviceType& type, const QString& value)
{
    Q_D( PenWidget );

    if (type == DeviceType::Stylus) {
        d->ui->tipPressureButton->setProperty( "curve", value );

    } else if (type == DeviceType::Eraser) {
        d->ui->eraserPressureButton->setProperty( "curve", value );

    } else {
        kError() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }
}


void PenWidget::setPressureFeel(const DeviceType& type, const QString& value)
{
    Q_D( PenWidget );
    if (type == DeviceType::Stylus) {
        d->ui->tipSlider->setValue(value.toInt());

    } else if (type == DeviceType::Eraser) {
        d->ui->eraserSlider->setValue(value.toInt());

    } else {
        kError() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }
}


void PenWidget::setTabletPcButton(const QString& value)
{
    Q_D( PenWidget );

    if( value.compare(QLatin1String( "on" ), Qt::CaseInsensitive) == 0 ) {
        d->ui->tpcCheckBox->setChecked( true );
    } else {
        d->ui->tpcCheckBox->setChecked( false );
    }
}


void PenWidget::setTrackingMode(const QString& value)
{
    Q_D( PenWidget );

    if( value.toInt() == 1 || value.compare(QLatin1String( "absolute" ), Qt::CaseInsensitive) == 0 ) {
        d->ui->radioButton_Absolute->setChecked( true );
        d->ui->radioButton_Relative->setChecked( false );
    }
    else {
        d->ui->radioButton_Absolute->setChecked( false );
        d->ui->radioButton_Relative->setChecked( true );
    }
}


void PenWidget::changePressCurve(const DeviceType& deviceType)
{
    PressCurveDialog selectPC(this);

    QString startValue = getPressureCurve(deviceType);
    QString result (startValue);

    selectPC.setDeviceType( deviceType );
    selectPC.setControllPoints( startValue );

    if( selectPC.exec() == KDialog::Accepted ) {
        result = selectPC.getControllPoints();

    } else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        DBusTabletInterface::instance().setProperty( deviceType, Property::PressureCurve, startValue );
    }

    if (result != startValue) {
        setPressureCurve( deviceType, selectPC.getControllPoints() );
        emit changed();
    }
}


void PenWidget::setupUi()
{
    Q_D( PenWidget );

    d->ui->setupUi( this );

    d->ui->penLabel->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/pen.png"))));

    connect ( d->ui->button2ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onProfileChanged()) );
    connect ( d->ui->button3ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onProfileChanged()) );
}


