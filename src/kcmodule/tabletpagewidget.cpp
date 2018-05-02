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

#include "tabletpagewidget.h"
#include "ui_tabletpagewidget.h"

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

namespace Wacom
{
    class TabletPageWidgetPrivate
    {
        public:
            TabletPageWidgetPrivate() : ui(new Ui::TabletPageWidget), tabletRotation(ScreenRotation::NONE) {}
            ~TabletPageWidgetPrivate() {
                delete ui;
            }

            Ui::TabletPageWidget* ui;                // Our UI widget.

            ScreenRotation        tabletRotation;    // The current tablet rotation with a hardware viewpoint (not the canvas!).
            TabletArea            tabletGeometry;    // The full tablet area as rectangle.
            ScreenMap             screenMap;         // The current tablet to screen mapping of the pad.
            ScreenSpace           screenSpace;       // The current screen mapping of the pad.

            QString               deviceNameStylus;  // The Xinput name of the stylus device of the current tablet.
            QString               deviceNameTouch;   // The Xinput name of the touch device of the current tablet.
            QString               tabletId;

    };
}

TabletPageWidget::TabletPageWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new TabletPageWidgetPrivate)
{
    setupUi();
}


TabletPageWidget::~TabletPageWidget()
{
    delete this->d_ptr;
}

void TabletPageWidget::setTabletId(const QString &tabletId)
{
    Q_D( TabletPageWidget );
    d->tabletId = tabletId;
}

void TabletPageWidget::loadFromProfile()
{
    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      stylusProfile     = profileManagement->loadDeviceProfile( DeviceType::Stylus );

    setRotation( stylusProfile.getProperty( Property::Rotate ) );
    setScreenSpace( stylusProfile.getProperty( Property::ScreenSpace ) );
    setScreenMap( stylusProfile.getProperty( Property::ScreenMap ) );
    setTrackingMode(stylusProfile.getProperty( Property::Mode ));
}


void TabletPageWidget::reloadWidget()
{
    Q_D( TabletPageWidget );

    // get all tablet device names we need
    QDBusReply<QString> stylusDeviceNameReply = DBusTabletInterface::instance().getDeviceName(d->tabletId, DeviceType::Stylus.key());
    QDBusReply<QString> touchDeviceNameReply  = DBusTabletInterface::instance().getDeviceName(d->tabletId, DeviceType::Touch.key());

    // update name and maximum tablet area for all devices
    d->deviceNameStylus.clear();
    d->deviceNameTouch.clear();
    d->tabletGeometry = TabletArea();
    d->screenMap      = ScreenMap();

    if (stylusDeviceNameReply.isValid()) {
        d->deviceNameStylus = stylusDeviceNameReply.value();
        d->tabletGeometry   = X11Wacom::getMaximumTabletArea(stylusDeviceNameReply.value());
        d->screenMap        = ScreenMap(d->tabletGeometry);
    }

    if (touchDeviceNameReply.isValid()) {
        d->deviceNameTouch = touchDeviceNameReply.value();
    }
}


void TabletPageWidget::saveToProfile()
{
    Q_D( const TabletPageWidget);

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile padProfile    = profileManagement->loadDeviceProfile( DeviceType::Pad );
    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );
    DeviceProfile touchProfile  = profileManagement->loadDeviceProfile( DeviceType::Touch );

    stylusProfile.setProperty ( Property::Rotate,      getRotation() );
    eraserProfile.setProperty ( Property::Rotate,      getRotation() );
    touchProfile.setProperty  ( Property::Rotate,      getRotation() );
    padProfile.setProperty    ( Property::Rotate,      QString()); // make sure it is not set on the pad - will mess up touch

    stylusProfile.setProperty ( Property::ScreenSpace, getScreenSpaceAsString() );
    eraserProfile.setProperty ( Property::ScreenSpace, getScreenSpaceAsString() );
    padProfile.setProperty    ( Property::ScreenSpace, QString()); // should not be set on the pad, causes trouble
    padProfile.setProperty    ( Property::Area,        QString()); // should not be set on the pad, causes trouble

    stylusProfile.setProperty ( Property::ScreenMap,   getScreenMapAsString() );
    eraserProfile.setProperty ( Property::ScreenMap,   getScreenMapAsString() );
    padProfile.setProperty    ( Property::ScreenMap,   QString() ); // make sure it is not set on the pad

    stylusProfile.setProperty ( Property::Mode,        getTrackingMode());
    eraserProfile.setProperty ( Property::Mode,        getTrackingMode());

    profileManagement->saveDeviceProfile(padProfile);
    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);

    if (!d->deviceNameTouch.isEmpty()) {
        profileManagement->saveDeviceProfile(touchProfile);
    }
}


void TabletPageWidget::onAutoRotateChanged(int state)
{
    setAutoRotationEnabled(state == Qt::Checked);
    onProfileChanged();
}


void TabletPageWidget::onProfileChanged()
{
    emit changed();
}


void TabletPageWidget::onTabletMappingClicked()
{
    Q_D(TabletPageWidget);

    // get current rotation settings
    // we need to invert it as our rotation settings in this widget have a canvas viewpoint
    const ScreenRotation* lookupRotation = ScreenRotation::find(getRotation());
    ScreenRotation        rotation       = lookupRotation ? lookupRotation->invert() : ScreenRotation::NONE;

    TabletAreaSelectionDialog selectionDialog;
    selectionDialog.setupWidget( getScreenMap(), d->deviceNameStylus, rotation);
    selectionDialog.select( getScreenSpace() );

    if (selectionDialog.exec() == QDialog::Accepted) {
        setScreenMap(selectionDialog.getScreenMap());
        setScreenSpace(selectionDialog.getScreenSpace());
        onProfileChanged();
    }
}


void TabletPageWidget::onRotationChanged()
{
    Q_D(TabletPageWidget);

    // determin rotation
    const ScreenRotation* lookupRotation = ScreenRotation::find(getRotation());

    // we need to invert it as our rotation settings in this widget have a canvas viewpoint
    // and our private member variable has a tablet viewpoint
    d->tabletRotation = lookupRotation ? lookupRotation->invert() : ScreenRotation::NONE;

    emit rotationChanged(d->tabletRotation);
}


void TabletPageWidget::onTrackingModeAbsolute(bool activated)
{
    if (!activated) {
        return;
    }

    setTrackingMode(QLatin1String("absolute"));
    onProfileChanged();
}


void TabletPageWidget::onTrackingModeRelative(bool activated)
{
    if (!activated) {
        return;
    }

    setTrackingMode(QLatin1String("relative"));
    onProfileChanged();
}


const QString TabletPageWidget::getRotation() const
{
    Q_D (const TabletPageWidget);

    QString rotation = ScreenRotation::NONE.key();

    if (isAutoRotationEnabled()) {
        if (isAutoRotateInversionEnabled()) {
            rotation = ScreenRotation::AUTO_INVERTED.key();
        } else {
            rotation = ScreenRotation::AUTO.key();
        }
    } else {
        int index = d->ui->rotatationSelectionComboBox->currentIndex();
        rotation  = d->ui->rotatationSelectionComboBox->itemData(index).toString();
    }

    return rotation;
}


const ScreenMap& TabletPageWidget::getScreenMap() const
{
    Q_D (const TabletPageWidget);

    return d->screenMap;
}


const QString TabletPageWidget::getScreenMapAsString() const
{
    return getScreenMap().toString();
}


const ScreenSpace& TabletPageWidget::getScreenSpace() const
{
    Q_D (const TabletPageWidget);

    return d->screenSpace;
}


const QString TabletPageWidget::getScreenSpaceAsString() const
{
    return getScreenSpace().toString();
}


const QString TabletPageWidget::getTrackingMode() const
{
    Q_D (const TabletPageWidget);

    if (d->ui->trackAbsoluteRadioButton->isChecked()) {
        return QLatin1String("absolute");
    }

    return QLatin1String("relative");
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

    if (value) {
        setRotation(ScreenRotation::NONE.key());
    } else {
        setAutoRotateInversionEnabled(false);
    }

    d->ui->rotateWithScreenCheckBox->blockSignals(true);
    d->ui->rotateWithScreenCheckBox->setChecked(value);
    d->ui->rotateWithScreenCheckBox->blockSignals(false);
}


void TabletPageWidget::setRotation(const QString& value)
{
    Q_D (TabletPageWidget);

    const ScreenRotation* lookup        = ScreenRotation::find(value);
    ScreenRotation        rotation      = lookup ? *lookup : ScreenRotation::NONE;
    QString               rotationValue = rotation.key();

    if (rotation == ScreenRotation::AUTO) {
        setAutoRotationEnabled(true);
        rotationValue = ScreenRotation::NONE.key();

    } else if (rotation == ScreenRotation::AUTO_INVERTED) {
        setAutoRotationEnabled(true);
        setAutoRotateInversionEnabled(true);
        rotationValue = ScreenRotation::NONE.key();
    }

    int rotationIndex = d->ui->rotatationSelectionComboBox->findData(rotationValue);

    d->ui->rotatationSelectionComboBox->blockSignals(true);
    d->ui->rotatationSelectionComboBox->setCurrentIndex(rotationIndex >= 0 ? rotationIndex : 0);
    d->ui->rotatationSelectionComboBox->blockSignals(false);

    onRotationChanged();
}


void TabletPageWidget::setScreenMap(const ScreenMap &screenMap)
{
    Q_D (TabletPageWidget);

    d->screenMap = screenMap;

    assertValidTabletMapping();
}


void TabletPageWidget::setScreenMap(const QString& value)
{
    setScreenMap(ScreenMap(value));
}


void TabletPageWidget::setScreenSpace(const ScreenSpace& screenSpace)
{
    Q_D (TabletPageWidget);

    d->screenSpace = screenSpace;

    assertValidTabletMapping();
}


void TabletPageWidget::setScreenSpace(const QString& value)
{
    setScreenSpace(ScreenSpace(value));
}


void TabletPageWidget::setTrackingMode(const QString& value)
{
    Q_D (TabletPageWidget);

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


void TabletPageWidget::assertValidTabletMapping()
{
    Q_D (TabletPageWidget);

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


void TabletPageWidget::setupUi()
{
    Q_D (TabletPageWidget);

    d->ui->setupUi(this);

    // init screen mapping warning
    d->ui->trackingWarningIcon->setPixmap(QIcon::fromTheme(QLatin1String("dialog-warning")).pixmap(QSize(16,16)));
    d->ui->trackingWarningIcon->setVisible(false);
    d->ui->trackingWarningLabel->setVisible(false);

    // fill rotation combo box
    // xsetwacom's rotation is based on coordinate rotation, but we are asking the user for a tablet rotation.
    // Therefore we have to swap the values for clockwise and counterclockwise rotation.
    d->ui->rotatationSelectionComboBox->addItem(i18nc("Either no orientation or the current screen orientation is applied to the tablet.", "Default Orientation"), ScreenRotation::NONE.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated clockwise.", "Rotate Tablet Clockwise"), ScreenRotation::CCW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated counterclockwise.", "Rotate Tablet Counterclockwise"), ScreenRotation::CW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated up side down.", "Rotate Tablet Upside-Down"), ScreenRotation::HALF.key());
}


