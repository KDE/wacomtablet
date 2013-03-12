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

#include "tabletpagewidget.h"
#include "ui_tabletpagewidget.h"

#include "dbustabletinterface.h"
#include "deviceprofile.h"
#include "profilemanagement.h"
#include "property.h"
#include "screenrotation.h"
#include "screenareaselectiondialog.h"
#include "stringutils.h"
#include "tabletareaselectiondialog.h"
#include "x11info.h"
#include "x11wacom.h"

#include <QtCore/QRect>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

using namespace Wacom;

namespace Wacom
{
    class TabletPageWidgetPrivate
    {
        public:
            TabletPageWidgetPrivate() : ui(new Ui::TabletPageWidget) {}
            ~TabletPageWidgetPrivate() {
                delete ui;
            }

            Ui::TabletPageWidget* ui;                      // Our UI widget.

            QString               padScreenAreaMapping;    // The current screen mapping of the pad.
            QString               padTabletAreaMapping;    // The current tablet mapping of the pad.
            QString               touchScreenAreaMapping;  // The current screen mapping of the touch device.
            QString               touchTabletAreaMapping;  // The current tablet mapping of the touch device.

            QString               deviceNameStylus;        // The Xinput name of the stylus device of the current tablet.
            QString               deviceNameTouch;         // The Xinput name of the touch device of the current tablet.

            QList< QRect >        screenAreas;             // A list of all X11 screens as rectangles.

            QRect                 tabletAreaPad;           // The maximum pad area as rectangle.
            QRect                 tabletAreaTouch;         // The maximum touch area as rectangle.
    };
}

TabletPageWidget::TabletPageWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new TabletPageWidgetPrivate)
{
    setupUi();
    reloadWidget();
}


TabletPageWidget::~TabletPageWidget()
{
    delete this->d_ptr;
}


void TabletPageWidget::loadFromProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile padProfile   = profileManagement->loadDeviceProfile( DeviceType::Pad );
    DeviceProfile touchProfile = profileManagement->loadDeviceProfile( DeviceType::Touch );

    setRotation( padProfile.getProperty( Property::Rotate ) );
    setAutoRotation( padProfile.getProperty( Property::RotateWithScreen ) );
    setPadScreenAreaMapping( padProfile.getProperty( Property::ScreenSpace ) );
    setPadTabletAreaMapping( padProfile.getProperty( Property::TabletArea ) );
    setTouchSupportEnabled( touchProfile.getPropertyAsBool( Property::Touch ) );
    setTrackingMode( touchProfile.getProperty( Property::Mode ) );
    setTouchScreenAreaMapping( touchProfile.getProperty( Property::ScreenSpace ) );
    setTouchTabletAreaMapping( touchProfile.getProperty( Property::TabletArea ) );
    setGesturesSupportEnabled( touchProfile.getPropertyAsBool( Property::Gesture ) );
    setScrollDistance( touchProfile.getProperty( Property::ScrollDistance ) );
    setZoomDistance( touchProfile.getProperty( Property::ZoomDistance ) );
    setTapTime( touchProfile.getProperty( Property::TapTime ) );
}


void TabletPageWidget::reloadWidget()
{
    Q_D( TabletPageWidget );

    // get all tablet device names we need
    QDBusReply<QString> stylusDeviceNameReply = DBusTabletInterface::instance().getDeviceName(DeviceType::Stylus);
    QDBusReply<QString> touchDeviceNameReply  = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);

    // update name and maximum tablet area for all devices
    d->deviceNameStylus.clear();
    d->deviceNameTouch.clear();
    d->tabletAreaPad   = QRect();
    d->tabletAreaTouch = QRect();

    if (stylusDeviceNameReply.isValid()) {
        d->deviceNameStylus = stylusDeviceNameReply.value();
        d->tabletAreaPad    = X11Wacom::getMaximumTabletArea(stylusDeviceNameReply.value());
    }

    if (touchDeviceNameReply.isValid()) {
        d->deviceNameTouch = touchDeviceNameReply.value();
        d->tabletAreaTouch = X11Wacom::getMaximumTabletArea(touchDeviceNameReply.value());
    }

    // get a list of all X11 screens
    d->screenAreas = X11Info::getScreens();
}


void TabletPageWidget::saveToProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile padProfile   = profileManagement->loadDeviceProfile( DeviceType::Pad );
    DeviceProfile touchProfile = profileManagement->loadDeviceProfile( DeviceType::Touch );

    padProfile.setProperty  ( Property::Rotate,           getRotation() );
    touchProfile.setProperty( Property::Rotate,           getRotation() );
    padProfile.setProperty  ( Property::RotateWithScreen, getAutoRotation() );
    touchProfile.setProperty( Property::RotateWithScreen, getAutoRotation() );
    padProfile.setProperty  ( Property::ScreenSpace,      getPadScreenAreaMapping() ); // TODO: does this belong in stylus/eraser profile?
    padProfile.setProperty  ( Property::TabletArea,       getPadTabletAreaMapping() ); // TODO: does this belong in stylus/eraser profile?
    touchProfile.setProperty( Property::Touch,            getTouchSupportEnabled() );
    touchProfile.setProperty( Property::Mode,             getTrackingMode() );
    touchProfile.setProperty( Property::ScreenSpace,      getTouchScreenAreaMapping() );
    touchProfile.setProperty( Property::TabletArea,       getTouchTabletAreaMapping() );
    touchProfile.setProperty( Property::Gesture,          getGestureSupportEnabled() );
    touchProfile.setProperty( Property::ScrollDistance,   getScrollDistance() );
    touchProfile.setProperty( Property::ZoomDistance,     getZoomDistance() );
    touchProfile.setProperty( Property::TapTime,          getTapTime() );

    profileManagement->saveDeviceProfile(padProfile);
    profileManagement->saveDeviceProfile(touchProfile);
}


void TabletPageWidget::onAutoRotateChanged(int state)
{
    setAutoRotationEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TabletPageWidget::onGesturesModeChanged(int state)
{
    setGesturesSupportEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TabletPageWidget::onPadScreenMappingClicked()
{
    Q_D(TabletPageWidget);

    QString tabletAreaCaption = i18n("Tablet Area");

    ScreenAreaSelectionDialog selectionDialog;

    selectionDialog.setupWidget( d->screenAreas, d->tabletAreaPad, getPadTabletAreaMappingRect(), tabletAreaCaption );
    selectionDialog.setSelection( d->padScreenAreaMapping );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->padScreenAreaMapping = selectionDialog.getSelection();
        onProfileChanged();
    }
}


void TabletPageWidget::onPadTabletMappingClicked()
{
    Q_D(TabletPageWidget);

    TabletAreaSelectionDialog selectionDialog;

    selectionDialog.setupWidget( d->tabletAreaPad, d->screenAreas, getPadScreenAreaMappingRect(), d->deviceNameStylus );
    selectionDialog.setSelection( getPadTabletAreaMapping() );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->padTabletAreaMapping = selectionDialog.getSelection();
        onProfileChanged();
    }
}


void TabletPageWidget::onProfileChanged()
{
    emit changed();
}


void TabletPageWidget::onTouchModeChanged(int state)
{
    setTouchSupportEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TabletPageWidget::onTouchScreenMappingClicked()
{
    Q_D(TabletPageWidget);

    QString tabletAreaCaption = i18n("Touch Area");

    ScreenAreaSelectionDialog selectionDialog;

    selectionDialog.setupWidget( d->screenAreas, d->tabletAreaTouch, getTouchTabletAreaMappingRect(), tabletAreaCaption );
    selectionDialog.setSelection( getTouchScreenAreaMapping() );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->touchScreenAreaMapping = selectionDialog.getSelection();
        onProfileChanged();
    }
}


void TabletPageWidget::onTouchTabletMappingClicked()
{
    Q_D(TabletPageWidget);

    TabletAreaSelectionDialog selectionDialog;

    selectionDialog.setupWidget( d->tabletAreaTouch, d->screenAreas, getTouchScreenAreaMappingRect(), d->deviceNameTouch );
    selectionDialog.setSelection( getTouchTabletAreaMapping() );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->touchTabletAreaMapping = selectionDialog.getSelection();
        onProfileChanged();
    }
}


const QString TabletPageWidget::getAutoRotation() const
{
    QString value = isAutoRotationEnabled() ? QLatin1String("on") : QLatin1String("off");

    if (isAutoRotateInversionEnabled()) {
        value = QLatin1String("inverted");
    }

    return value;
}


const QString TabletPageWidget::getGestureSupportEnabled() const
{
    return (isGesturesSupportEnabled() ? QLatin1String("on") : QLatin1String("off"));
}


const QString& TabletPageWidget::getPadScreenAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->padScreenAreaMapping;
}


const QRect TabletPageWidget::getPadScreenAreaMappingRect() const
{
    Q_D (const TabletPageWidget);

    return convertScreenAreaMappingToQRect(d->padScreenAreaMapping);
}


const QString& TabletPageWidget::getPadTabletAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->padTabletAreaMapping;
}


const QRect TabletPageWidget::getPadTabletAreaMappingRect() const
{
    Q_D (const TabletPageWidget);

    return convertTabletAreaMappingToQRect(d->padTabletAreaMapping, d->tabletAreaPad);
}


const QString TabletPageWidget::getRotation() const
{
    Q_D (const TabletPageWidget);

    int index = d->ui->rotatationSelectionComboBox->currentIndex();
    return d->ui->rotatationSelectionComboBox->itemData(index).toString();
}


const QString TabletPageWidget::getScrollDistance() const
{
    Q_D (const TabletPageWidget);

    return QString::number(d->ui->scrollDistanceSpinBox->value());
}


const QString TabletPageWidget::getTapTime() const
{
    Q_D (const TabletPageWidget);

    return QString::number(d->ui->tapTimeSpinBox->value());
}


const QString& TabletPageWidget::getTouchScreenAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->touchScreenAreaMapping;
}


const QRect TabletPageWidget::getTouchScreenAreaMappingRect() const
{
    Q_D (const TabletPageWidget);

    return convertScreenAreaMappingToQRect(d->touchScreenAreaMapping);
}


const QString TabletPageWidget::getTouchSupportEnabled() const
{
    return (isTouchSupportEnabled() ? QLatin1String("on") : QLatin1String("off"));
}


const QString& TabletPageWidget::getTouchTabletAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->touchTabletAreaMapping;
}


const QRect TabletPageWidget::getTouchTabletAreaMappingRect() const
{
    Q_D (const TabletPageWidget);

    return convertTabletAreaMappingToQRect(d->touchTabletAreaMapping, d->tabletAreaTouch);
}



const QString TabletPageWidget::getTrackingMode() const
{
    Q_D (const TabletPageWidget);

    if (d->ui->trackAbsoluteRadioButton->isChecked()) {
        return QLatin1String("absolute");
    }

    return QLatin1String("relative");
}


const QString TabletPageWidget::getZoomDistance() const
{
    Q_D (const TabletPageWidget);

    return QString::number(d->ui->zoomDistanceSpinBox->value());
}


bool TabletPageWidget::isAutoRotateInversionEnabled() const
{
    Q_D (const TabletPageWidget);

    return d->ui->rotateWithScreenInvertCheckBox->isChecked();
}


bool TabletPageWidget::isAutoRotationEnabled() const
{
    Q_D (const TabletPageWidget);

    return d->ui->rotateWithScreenCheckBox->isChecked();
}


bool TabletPageWidget::isGesturesSupportEnabled() const
{
    Q_D (const TabletPageWidget);

    return (d->ui->gesturesCheckBox->isChecked() && d->ui->touchGroupBox->isEnabled());
}

bool TabletPageWidget::isTouchSupportEnabled() const
{
    Q_D (const TabletPageWidget);

    return (d->ui->touchCheckBox->isChecked() && d->ui->touchGroupBox->isEnabled());
}


void TabletPageWidget::setAutoRotation(const QString& value)
{
    if (value.compare(QLatin1String("inverted"), Qt::CaseInsensitive) == 0) {
        setAutoRotationEnabled(true);
        setAutoRotateInversionEnabled(true);
    } else {
        setAutoRotateInversionEnabled(false);
        setAutoRotationEnabled(StringUtils::asBool(value));
    }
}


void TabletPageWidget::setAutoRotateInversionEnabled(bool value)
{
    Q_D (TabletPageWidget);

    d->ui->rotateWithScreenInvertCheckBox->blockSignals(true);
    d->ui->rotateWithScreenInvertCheckBox->setChecked(value);
    d->ui->rotateWithScreenInvertCheckBox->blockSignals(false);
}


void TabletPageWidget::setAutoRotationEnabled(bool value)
{
    Q_D (TabletPageWidget);

    d->ui->rotatationSelectionComboBox->setEnabled(!value);
    d->ui->rotateWithScreenInvertCheckBox->setEnabled(value);

    if (!value) {
        setAutoRotateInversionEnabled(false);
    }

    d->ui->rotateWithScreenCheckBox->blockSignals(true);
    d->ui->rotateWithScreenCheckBox->setChecked(value);
    d->ui->rotateWithScreenCheckBox->blockSignals(false);
}


void TabletPageWidget::setGesturesSupportEnabled(bool value)
{
    Q_D (TabletPageWidget);

    d->ui->gesturesGroupBox->setEnabled(value);

    d->ui->gesturesCheckBox->blockSignals(true);
    d->ui->gesturesCheckBox->setChecked(value);
    d->ui->gesturesCheckBox->blockSignals(false);
}


void TabletPageWidget::setPadScreenAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    d->padScreenAreaMapping = value;
}


void TabletPageWidget::setPadTabletAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    if (value.isEmpty() || value.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {
        d->padTabletAreaMapping = QLatin1String("-1 -1 -1 -1");

    } else {
        d->padTabletAreaMapping = value;
    }
}


void TabletPageWidget::setRotation(const QString& value)
{
    Q_D (TabletPageWidget);

    QString               rotationValue = ScreenRotation::NONE.key();
    const ScreenRotation* rotation      = ScreenRotation::find(value);

    if (rotation != NULL) {
        rotationValue = rotation->key();
    }

    int rotationIndex = d->ui->rotatationSelectionComboBox->findData(rotationValue);

    d->ui->rotatationSelectionComboBox->setCurrentIndex(rotationIndex >= 0 ? rotationIndex : 0);
}


void TabletPageWidget::setScrollDistance(const QString& value)
{
    Q_D (TabletPageWidget);

    d->ui->scrollDistanceSpinBox->blockSignals(true);
    d->ui->scrollDistanceSpinBox->setValue(value.toInt());
    d->ui->scrollDistanceSpinBox->blockSignals(false);
}


void TabletPageWidget::setTapTime(const QString& value)
{
    Q_D (TabletPageWidget);

    d->ui->tapTimeSpinBox->blockSignals(true);
    d->ui->tapTimeSpinBox->setValue(value.toInt());
    d->ui->tapTimeSpinBox->blockSignals(false);
}


void TabletPageWidget::setTouchScreenAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    d->touchScreenAreaMapping = value;
}


void TabletPageWidget::setTouchSupportEnabled(bool value)
{
    Q_D (TabletPageWidget);

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


void TabletPageWidget::setTouchTabletAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    if (value.isEmpty() || value.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {
        d->touchTabletAreaMapping = QLatin1String("-1 -1 -1 -1");

    } else {
        d->touchTabletAreaMapping = value;
    }
}


void TabletPageWidget::setTrackingMode(const QString& value)
{
    Q_D (TabletPageWidget);

    d->ui->trackAbsoluteRadioButton->blockSignals(true);
    d->ui->trackRelativeRadioButton->blockSignals(true);

    if (value.compare(QLatin1String("absolute"), Qt::CaseInsensitive) == 0) {
        d->ui->trackAbsoluteRadioButton->setChecked(true);
        d->ui->trackRelativeRadioButton->setChecked(false);
    } else {
        d->ui->trackAbsoluteRadioButton->setChecked(false);
        d->ui->trackRelativeRadioButton->setChecked(true);
    }

    d->ui->trackAbsoluteRadioButton->blockSignals(false);
    d->ui->trackRelativeRadioButton->blockSignals(false);
}


void TabletPageWidget::setZoomDistance(const QString& value)
{
    Q_D (TabletPageWidget);

    d->ui->zoomDistanceSpinBox->blockSignals(true);
    d->ui->zoomDistanceSpinBox->setValue(value.toInt());
    d->ui->zoomDistanceSpinBox->blockSignals(false);
}


const QRect TabletPageWidget::convertScreenAreaMappingToQRect(const QString& areaMapping) const
{
    Q_D( const TabletPageWidget );

    QRect result;

    QRegExp monitorRegExp(QLatin1String("^\\s*map(\\d+)\\s*$"), Qt::CaseInsensitive);

    if (areaMapping.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {

        // full screen mapping - unite all X11 screens to one big rectangle
        foreach (QRect screen, d->screenAreas) {
            result = result.united(screen);
        }

    } else if (monitorRegExp.indexIn(areaMapping, 0) != -1) {

        // monitor mapping
        int screenNum = monitorRegExp.cap(1).toInt();

        if ( 0 <= screenNum && screenNum < d->screenAreas.count() ) {
            result = d->screenAreas.at(screenNum);
        }

    } else {
        // area mapping
        result = StringUtils::toQRect(areaMapping, true);
    }

    return result;
}


const QRect TabletPageWidget::convertTabletAreaMappingToQRect(const QString& areaMapping, const QRect& areaMappingMax) const
{
    QRect result;

    if (areaMapping.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {

        result = areaMappingMax;

    } else {

        result = StringUtils::toQRect(areaMapping);

        // if all values are -1 then this is also a full screen mapping
        if (result.x() == -1 && result.y() == -1 && result.width() == -1 && result.height() == -1) {
            result = areaMappingMax;
        }
    }

    return result;
}



void TabletPageWidget::setupUi()
{
    Q_D (TabletPageWidget);

    d->ui->setupUi(this);

    // fill rotation combo box
    // TODO: check this - maybe a bug in xsetwacom 0.19
    // xsetwacom's rotation is based on screen rotation, but we are asking the user for a tablet rotation.
    // Therefore we have to swap the values for clockwise and counterclockwise rotation.
    d->ui->rotatationSelectionComboBox->addItem(i18nc("No rotation is applied to the tablet.", "No Rotation"), ScreenRotation::NONE.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated clockwise.", "Rotate Tablet Clockwise"), ScreenRotation::CCW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated counterclockwise.", "Rotate Tablet Counterclockwise"), ScreenRotation::CW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated up side down.", "Rotate Tablet Upside-Down"), ScreenRotation::HALF.key());
}


