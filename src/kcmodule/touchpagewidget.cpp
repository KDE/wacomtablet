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

#include "touchpagewidget.h"
#include "ui_touchpagewidget.h"

#include "dbustabletinterface.h"
#include "deviceprofile.h"
#include "profilemanagement.h"
#include "property.h"
#include "stringutils.h"
#include "tabletareaselectiondialog.h"
#include "x11info.h"
#include "x11wacom.h"

#include <QRegExp>
#include <QStringList>

using namespace Wacom;

namespace Wacom {
    class TouchPageWidgetPrivate
    {
        public:
            TouchPageWidgetPrivate() : ui(new Ui::TouchPageWidget), tabletRotation(ScreenRotation::NONE) {}
            ~TouchPageWidgetPrivate() {
                delete ui;
            }

            Ui::TouchPageWidget* ui;

            ScreenRotation tabletRotation;     // The currently selected tablet rotation.
            TabletArea     tabletGeometry;     // The full touch area as rectangle.
            ScreenMap      screenMap;          // The current tablet to screen mapping of the touch device.
            ScreenSpace    screenSpace;        // The current screen mapping of the touch device.
            QString        touchDeviceName;    // The Xinput name of the touch device of the current tablet.
            QString        tabletId;

    }; // PRIVATE CLASS
} // NAMESPACE


TouchPageWidget::TouchPageWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new TouchPageWidgetPrivate)
{
    setupUi();
}


TouchPageWidget::~TouchPageWidget()
{
    delete this->d_ptr;
}

void TouchPageWidget::setTabletId(const QString &tabletId)
{
    Q_D( TouchPageWidget );

    d->tabletId = tabletId;

}

void TouchPageWidget::loadFromProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      touchProfile      = profileManagement->loadDeviceProfile( DeviceType::Touch );

    // set all properties no matter if the tablet supports that device
    // to get all widgets properly initialized.

    setTouchSupportEnabled( touchProfile.getPropertyAsBool( Property::Touch ) );
    setTrackingMode( touchProfile.getProperty( Property::Mode ) );
    setScreenSpace( touchProfile.getProperty( Property::ScreenSpace ) );
    setScreenMap( touchProfile.getProperty( Property::ScreenMap ) );
    setGesturesSupportEnabled( touchProfile.getPropertyAsBool( Property::Gesture ) );
    setScrollDistance( touchProfile.getProperty( Property::ScrollDistance ) );
    setScrollInversion( touchProfile.getProperty( Property::InvertScroll) );
    setZoomDistance( touchProfile.getProperty( Property::ZoomDistance ) );
    setTapTime( touchProfile.getProperty( Property::TapTime ) );
}


void TouchPageWidget::reloadWidget()
{
    Q_D( TouchPageWidget );

    // get all tablet device names we need
    QDBusReply<QString> touchDeviceNameReply  = DBusTabletInterface::instance().getDeviceName(d->tabletId, DeviceType::Touch.key());

    // update name and maximum tablet area for all devices
    d->touchDeviceName.clear();
    d->tabletGeometry = TabletArea();
    d->screenMap      = ScreenMap();

    if (touchDeviceNameReply.isValid()) {
        d->touchDeviceName = touchDeviceNameReply.value();
        if (!d->touchDeviceName.isEmpty()) { // touch device available
            d->tabletGeometry  = X11Wacom::getMaximumTabletArea(touchDeviceNameReply.value());
            d->screenMap       = ScreenMap(d->tabletGeometry);
        }
    }
}


void TouchPageWidget::saveToProfile()
{
    Q_D( const TouchPageWidget);

    if (d->touchDeviceName.isEmpty()) {
        return; // no touch device available
    }

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      touchProfile      = profileManagement->loadDeviceProfile( DeviceType::Touch );

    touchProfile.setProperty  ( Property::Touch,            getTouchSupportEnabled() );
    touchProfile.setProperty  ( Property::Mode,             getTrackingMode() );
    touchProfile.setProperty  ( Property::ScreenSpace,      getScreenSpaceAsString() );
    touchProfile.setProperty  ( Property::ScreenMap,        getScreenMapAsString() );
    touchProfile.setProperty  ( Property::Gesture,          getGestureSupportEnabled() );
    touchProfile.setProperty  ( Property::ScrollDistance,   getScrollDistance() );
    touchProfile.setProperty  ( Property::InvertScroll,     getScrollInversion() );
    touchProfile.setProperty  ( Property::ZoomDistance,     getZoomDistance() );
    touchProfile.setProperty  ( Property::TapTime,          getTapTime() );
    touchProfile.setProperty  ( Property::Rotate,           d->tabletRotation.key() );

    profileManagement->saveDeviceProfile(touchProfile);
}


void TouchPageWidget::onGesturesModeChanged(int state)
{
    setGesturesSupportEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TouchPageWidget::onProfileChanged()
{
    emit changed();
}


void TouchPageWidget::onRotationChanged(const ScreenRotation& rotation)
{
    Q_D(TouchPageWidget);

    d->tabletRotation = rotation;
}


void TouchPageWidget::onTabletMappingClicked()
{
    Q_D(TouchPageWidget);

    TabletAreaSelectionDialog selectionDialog;
    selectionDialog.setupWidget( getScreenMap(), d->touchDeviceName, d->tabletRotation);
    selectionDialog.select( getScreenSpace() );

    if (selectionDialog.exec() == QDialog::Accepted) {
        setScreenMap(selectionDialog.getScreenMap());
        setScreenSpace(selectionDialog.getScreenSpace());
        onProfileChanged();
    }
}


void TouchPageWidget::onTouchModeChanged(int state)
{
    setTouchSupportEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TouchPageWidget::onTrackingModeAbsolute(bool activated)
{
    if (!activated) {
        return;
    }

    setTrackingMode(QLatin1String("absolute"));
    onProfileChanged();
}


void TouchPageWidget::onTrackingModeRelative(bool activated)
{
    if (!activated) {
        return;
    }

    setTrackingMode(QLatin1String("relative"));
    onProfileChanged();
}


const QString TouchPageWidget::getGestureSupportEnabled() const
{
    return (isGesturesSupportEnabled() ? QLatin1String("on") : QLatin1String("off"));
}


const ScreenMap& TouchPageWidget::getScreenMap() const
{
    Q_D (const TouchPageWidget);

    return d->screenMap;
}


const QString TouchPageWidget::getScreenMapAsString() const
{
    return getScreenMap().toString();
}


const ScreenSpace& TouchPageWidget::getScreenSpace() const
{
    Q_D (const TouchPageWidget);

    return d->screenSpace;
}

const QString TouchPageWidget::getScreenSpaceAsString() const
{
    return getScreenSpace().toString();
}


const QString TouchPageWidget::getScrollDistance() const
{
    Q_D (const TouchPageWidget);

    return QString::number(d->ui->scrollDistanceSpinBox->value());
}


const QString TouchPageWidget::getScrollInversion() const
{
    Q_D (const TouchPageWidget);

    return (d->ui->scrollInversionCheckBox->isChecked() ? QLatin1String("on") : QLatin1String("off"));
}


const QString TouchPageWidget::getTapTime() const
{
    Q_D (const TouchPageWidget);

    return QString::number(d->ui->tapTimeSpinBox->value());
}


const QString TouchPageWidget::getTouchSupportEnabled() const
{
    return (isTouchSupportEnabled() ? QLatin1String("on") : QLatin1String("off"));
}


const QString TouchPageWidget::getTrackingMode() const
{
    Q_D (const TouchPageWidget);

    if (d->ui->trackAbsoluteRadioButton->isChecked()) {
        return QLatin1String("absolute");
    }

    return QLatin1String("relative");
}


const QString TouchPageWidget::getZoomDistance() const
{
    Q_D (const TouchPageWidget);

    return QString::number(d->ui->zoomDistanceSpinBox->value());
}


bool TouchPageWidget::isGesturesSupportEnabled() const
{
    Q_D (const TouchPageWidget);

    return (d->ui->gesturesCheckBox->isChecked() && d->ui->touchGroupBox->isEnabled());
}

bool TouchPageWidget::isTouchSupportEnabled() const
{
    Q_D (const TouchPageWidget);

    return (d->ui->touchCheckBox->isChecked() && d->ui->touchGroupBox->isEnabled());
}


void TouchPageWidget::setGesturesSupportEnabled(bool value)
{
    Q_D (TouchPageWidget);

    d->ui->gesturesGroupBox->setEnabled(value);

    d->ui->gesturesCheckBox->blockSignals(true);
    d->ui->gesturesCheckBox->setChecked(value);
    d->ui->gesturesCheckBox->blockSignals(false);
}


void TouchPageWidget::setScreenMap(const ScreenMap &screenMap)
{
    Q_D (TouchPageWidget);

    d->screenMap = screenMap;

    assertValidTabletMapping();
}


void TouchPageWidget::setScreenMap(const QString& value)
{
    setScreenMap(ScreenMap(value));
}


void TouchPageWidget::setScreenSpace(const ScreenSpace& screenSpace)
{
    Q_D (TouchPageWidget);

    d->screenSpace = screenSpace;

    assertValidTabletMapping();
}

void TouchPageWidget::setScreenSpace(const QString& value)
{
    setScreenSpace(ScreenSpace(value));
}


void TouchPageWidget::setScrollDistance(const QString& value)
{
    Q_D (TouchPageWidget);

    d->ui->scrollDistanceSpinBox->blockSignals(true);
    d->ui->scrollDistanceSpinBox->setValue(value.toInt());
    d->ui->scrollDistanceSpinBox->blockSignals(false);
}


void TouchPageWidget::setScrollInversion(const QString& value)
{
    Q_D(TouchPageWidget);

    d->ui->scrollInversionCheckBox->blockSignals(true);
    d->ui->scrollInversionCheckBox->setChecked(StringUtils::asBool(value));
    d->ui->scrollInversionCheckBox->blockSignals(false);
}


void TouchPageWidget::setTouchSupportEnabled(bool value)
{
    Q_D (TouchPageWidget);

    d->ui->trackingModeGroupBox->setEnabled(value);
    d->ui->touchMappingGroupBox->setEnabled(value);
    d->ui->gesturesCheckBox->setEnabled(value);

    if (isGesturesSupportEnabled()) {
        d->ui->gesturesGroupBox->setEnabled(value);
    }

    d->ui->touchCheckBox->blockSignals(true);
    d->ui->touchCheckBox->setChecked(value);
    d->ui->touchCheckBox->blockSignals(false);
}


void TouchPageWidget::setTapTime(const QString& value)
{
    Q_D (TouchPageWidget);

    d->ui->tapTimeSpinBox->blockSignals(true);
    d->ui->tapTimeSpinBox->setValue(value.toInt());
    d->ui->tapTimeSpinBox->blockSignals(false);
}


void TouchPageWidget::setTrackingMode(const QString& value)
{
    Q_D (TouchPageWidget);

    d->ui->trackAbsoluteRadioButton->blockSignals(true);
    d->ui->trackRelativeRadioButton->blockSignals(true);

    if (value.contains(QLatin1String("absolute"), Qt::CaseInsensitive)) {
        d->ui->trackAbsoluteRadioButton->setChecked(true);
        d->ui->trackRelativeRadioButton->setChecked(false);
    } else {
        d->ui->trackAbsoluteRadioButton->setChecked(false);
        d->ui->trackRelativeRadioButton->setChecked(true);
    }

    d->ui->trackAbsoluteRadioButton->blockSignals(false);
    d->ui->trackRelativeRadioButton->blockSignals(false);

    assertValidTabletMapping();
}


void TouchPageWidget::setZoomDistance(const QString& value)
{
    Q_D (TouchPageWidget);

    d->ui->zoomDistanceSpinBox->blockSignals(true);
    d->ui->zoomDistanceSpinBox->setValue(value.toInt());
    d->ui->zoomDistanceSpinBox->blockSignals(false);
}


void TouchPageWidget::assertValidTabletMapping()
{
    Q_D (TouchPageWidget);

    bool isWarningVisible = false;

    if (d->ui->trackRelativeRadioButton->isChecked()) {
        // Relative mode is selected. In relative mode a
        // device can not be mapped to a single monitor
        ScreenSpace screenSpace = getScreenSpace();

        if (screenSpace.isMonitor()) {
            isWarningVisible = true;
        }
    }

    d->ui->trackingWarningIcon->setVisible(isWarningVisible);
    d->ui->trackingWarningLabel->setVisible(isWarningVisible);
}


void TouchPageWidget::setupUi()
{
    Q_D (TouchPageWidget);

    d->ui->setupUi(this);

    // init screen mapping warning
    d->ui->trackingWarningIcon->setPixmap(QIcon::fromTheme(QLatin1String("dialog-warning")).pixmap(QSize(16,16)));
    d->ui->trackingWarningIcon->setVisible(false);
    d->ui->trackingWarningLabel->setVisible(false);
}
