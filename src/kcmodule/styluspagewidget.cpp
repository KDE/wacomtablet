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

#include "debug.h" // always needs to be first include

#include "styluspagewidget.h"
#include "ui_styluspagewidget.h"

#include "buttonactionselectiondialog.h"
#include "pressurecurvedialog.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "buttonshortcut.h"

//Qt includes
#include <QStandardPaths>
#include <QPixmap>

using namespace Wacom;

/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class StylusPageWidgetPrivate {
        public:
            StylusPageWidgetPrivate() : ui(new Ui::StylusPageWidget) {}
            ~StylusPageWidgetPrivate() {
                delete ui;
            }

            Ui::StylusPageWidget* ui;
            QString               tabletId;
    };
} // NAMESPACE


StylusPageWidget::StylusPageWidget( QWidget* parent )
    : QWidget( parent ), d_ptr(new StylusPageWidgetPrivate)
{
    setupUi();
}

StylusPageWidget::~StylusPageWidget()
{
    delete this->d_ptr;
}


void StylusPageWidget::setTabletId(const QString &tabletId)
{
    Q_D( StylusPageWidget );
    d->tabletId = tabletId;
}

void StylusPageWidget::loadFromProfile()
{
    Q_D( const StylusPageWidget );
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser feel / tip feel
    setPressureFeel  ( DeviceType::Eraser, eraserProfile.getProperty( Property::Threshold ) );
    setPressureCurve ( DeviceType::Eraser, eraserProfile.getProperty( Property::PressureCurve ) );
    setPressureFeel  ( DeviceType::Stylus, stylusProfile.getProperty( Property::Threshold ) );
    setPressureCurve ( DeviceType::Stylus, stylusProfile.getProperty( Property::PressureCurve ) );

    // Button Actions
    setButtonShortcut ( Property::Button1, stylusProfile.getProperty( Property::Button1 ) );
    setButtonShortcut ( Property::Button2, stylusProfile.getProperty( Property::Button2 ) );
    setButtonShortcut ( Property::Button3, stylusProfile.getProperty( Property::Button3 ) );

    // Tap to Click
    setTabletPcButton ( stylusProfile.getProperty( Property::TabletPcButton ) );


    //Raw Sample Rate
    d->ui->horizontalSliderRawSample->setValue( stylusProfile.getProperty( Property::RawSample ).toInt() );

    //Suppress Rate
    d->ui->horizontalSliderSuppress->setValue( stylusProfile.getProperty( Property::Suppress ).toInt() );
}


void StylusPageWidget::reloadWidget()
{
    // nothing to do
}


void StylusPageWidget::saveToProfile()
{
    Q_D( const StylusPageWidget );
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser / tip pressure
    eraserProfile.setProperty( Property::Threshold,     getPressureFeel(DeviceType::Eraser) );
    eraserProfile.setProperty( Property::PressureCurve, getPressureCurve(DeviceType::Eraser) );
    stylusProfile.setProperty( Property::Threshold,     getPressureFeel(DeviceType::Stylus) );
    stylusProfile.setProperty( Property::PressureCurve, getPressureCurve(DeviceType::Stylus) );

    // button 2 and 3 config
    eraserProfile.setProperty( Property::Button1, getButtonShortcut(Property::Button1) );
    eraserProfile.setProperty( Property::Button2, getButtonShortcut(Property::Button2) );
    eraserProfile.setProperty( Property::Button3, getButtonShortcut(Property::Button3) );
    stylusProfile.setProperty( Property::Button1, getButtonShortcut(Property::Button1) );
    stylusProfile.setProperty( Property::Button2, getButtonShortcut(Property::Button2) );
    stylusProfile.setProperty( Property::Button3, getButtonShortcut(Property::Button3) );

    // tap to click
    stylusProfile.setProperty( Property::TabletPcButton, getTabletPcButton() );

    //Raw Sample Rate
    eraserProfile.setProperty( Property::RawSample, QString::number(d->ui->horizontalSliderRawSample->value()) );
    stylusProfile.setProperty( Property::RawSample, QString::number(d->ui->horizontalSliderRawSample->value()) );

    //Suppress Rate
    eraserProfile.setProperty( Property::Suppress, QString::number(d->ui->horizontalSliderSuppress->value()) );
    stylusProfile.setProperty( Property::Suppress, QString::number(d->ui->horizontalSliderSuppress->value()) );

    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);
}


void StylusPageWidget::onChangeEraserPressureCurve()
{
    changePressureCurve(DeviceType::Eraser);
}


void StylusPageWidget::onChangeTipPressureCurve()
{
    changePressureCurve(DeviceType::Stylus);
}


void StylusPageWidget::onProfileChanged()
{
    emit changed();
}


const QString StylusPageWidget::getButtonShortcut(const Property& button) const
{
    Q_D( const StylusPageWidget );

    ButtonShortcut shortcut;

    if (button == Property::Button1) {
        shortcut = d->ui->button1ActionSelector->getShortcut();
    } else if (button == Property::Button2) {
        shortcut = d->ui->button2ActionSelector->getShortcut();
    } else if (button == Property::Button3) {
        shortcut = d->ui->button3ActionSelector->getShortcut();
    } else {
        qCritical() << QString::fromLatin1("Internal Error: Unknown button property '%1' provided!").arg(button.key());
    }

    return shortcut.toString();
}


const QString StylusPageWidget::getPressureCurve(const DeviceType& type) const
{
    Q_D (const StylusPageWidget);

    if (type == DeviceType::Stylus) {
        return d->ui->tipPressureButton->property( "curve" ).toString();

    } else if (type == DeviceType::Eraser) {
        return d->ui->eraserPressureButton->property( "curve" ).toString();

    } else {
        qCritical() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }

    return QString();
}


const QString StylusPageWidget::getPressureFeel(const DeviceType& type) const
{
    Q_D (const StylusPageWidget);

    if (type == DeviceType::Stylus) {
        return QString::number(d->ui->tipSlider->value());

    } else if (type == DeviceType::Eraser) {
        return QString::number(d->ui->eraserSlider->value());

    } else {
        qCritical() << QString::fromLatin1("Invalid device type '%1' provided!").arg(type.key());
    }

    return QString();
}


const QString StylusPageWidget::getTabletPcButton() const
{
    Q_D (const StylusPageWidget);
    return (d->ui->tpcCheckBox->isChecked() ? QLatin1String("on") : QLatin1String("off"));
}


void StylusPageWidget::setButtonShortcut(const Property& button, const QString& shortcut)
{
    Q_D( StylusPageWidget );

    if (button == Property::Button1) {
        d->ui->button1ActionSelector->setShortcut(ButtonShortcut(shortcut));

    } else if (button == Property::Button2) {
        d->ui->button2ActionSelector->setShortcut(ButtonShortcut(shortcut));

    } else if (button == Property::Button3) {
        d->ui->button3ActionSelector->setShortcut(ButtonShortcut(shortcut));

    } else {
        qCritical() << QString::fromLatin1("Internal Error: Unknown button property '%1' provided!").arg(button.key());
    }
}


void StylusPageWidget::setPressureCurve(const DeviceType& type, const QString& value)
{
    Q_D( StylusPageWidget );

    if (type == DeviceType::Stylus) {
        d->ui->tipPressureButton->setProperty( "curve", value );

    } else if (type == DeviceType::Eraser) {
        d->ui->eraserPressureButton->setProperty( "curve", value );

    } else {
        qCritical() << QString::fromLatin1("Internal Error: Invalid device type '%1' provided!").arg(type.key());
    }
}


void StylusPageWidget::setPressureFeel(const DeviceType& type, const QString& value)
{
    Q_D( StylusPageWidget );

    if (type == DeviceType::Stylus) {
        d->ui->tipSlider->setValue(value.toInt());

    } else if (type == DeviceType::Eraser) {
        d->ui->eraserSlider->setValue(value.toInt());

    } else {
        qCritical() << QString::fromLatin1("Internal Error: Invalid device type '%1' provided!").arg(type.key());
    }
}


void StylusPageWidget::setTabletPcButton(const QString& value)
{
    Q_D( StylusPageWidget );

    if( value.compare(QLatin1String( "on" ), Qt::CaseInsensitive) == 0 ) {
        d->ui->tpcCheckBox->setChecked( true );
    } else {
        d->ui->tpcCheckBox->setChecked( false );
    }
}


void StylusPageWidget::changePressureCurve(const DeviceType& deviceType)
{
    Q_D( StylusPageWidget );

    PressureCurveDialog selectPC(this);

    QString startValue = getPressureCurve(deviceType);
    QString result (startValue);

    selectPC.setTabletId(d->tabletId);
    selectPC.setDeviceType( deviceType );
    selectPC.setControllPoints( startValue );

    if( selectPC.exec() == QDialog::Accepted ) {
        result = selectPC.getControllPoints();

    } else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        DBusTabletInterface::instance().setProperty( d->tabletId, deviceType, Property::PressureCurve, startValue );
    }

    if (result != startValue) {
        setPressureCurve( deviceType, result );
        emit changed();
    }
}


void StylusPageWidget::setupUi()
{
    Q_D( StylusPageWidget );

    d->ui->setupUi( this );

    d->ui->penLabel->setPixmap(QPixmap(QStandardPaths::locate(QStandardPaths::DataLocation, QString::fromLatin1("wacomtablet/images/pen.png"))));

    connect ( d->ui->button1ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onProfileChanged()) );
    connect ( d->ui->button2ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onProfileChanged()) );
    connect ( d->ui->button3ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT(onProfileChanged()) );
}


