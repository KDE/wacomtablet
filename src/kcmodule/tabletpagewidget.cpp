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

            Ui::TabletPageWidget* ui;                // Our UI widget.

            QRect                 tabletAreaFull;    // The full tablet area as rectangle.
            QString               tabletAreaMapping; // The current tablet mapping of the pad.
            QString               screenAreaMapping; // The current screen mapping of the pad.

            QString               deviceNameStylus;  // The Xinput name of the stylus device of the current tablet.
            QString               deviceNameTouch;   // The Xinput name of the touch device of the current tablet.

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
    DeviceProfile      stylusProfile     = profileManagement->loadDeviceProfile( DeviceType::Stylus );

    setRotation( stylusProfile.getProperty( Property::Rotate ) );
    setScreenAreaMapping( stylusProfile.getProperty( Property::ScreenSpace ) );
    setTabletAreaMapping( stylusProfile.getProperty( Property::ScreenMap ) );
    setTrackingMode(stylusProfile.getProperty( Property::Mode ));
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
    d->tabletAreaFull = QRect();

    if (stylusDeviceNameReply.isValid()) {
        d->deviceNameStylus = stylusDeviceNameReply.value();
        d->tabletAreaFull    = X11Wacom::getMaximumTabletArea(stylusDeviceNameReply.value());
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

    stylusProfile.setProperty ( Property::ScreenSpace, getScreenAreaMapping() );
    eraserProfile.setProperty ( Property::ScreenSpace, getScreenAreaMapping() );

    stylusProfile.setProperty ( Property::ScreenMap,   getTabletAreaMapping() );
    eraserProfile.setProperty ( Property::ScreenMap,   getTabletAreaMapping() );
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


void TabletPageWidget::onScreenMappingClicked()
{
    Q_D(TabletPageWidget);

    QString tabletAreaCaption = i18n("Tablet Area");

    ScreenAreaSelectionDialog selectionDialog;
    selectionDialog.setupWidget( d->screenAreaMapping, d->tabletAreaMapping, tabletAreaCaption, d->deviceNameStylus );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->screenAreaMapping = selectionDialog.getSelection();
        onProfileChanged();
    }
}


void TabletPageWidget::onTabletMappingClicked()
{
    Q_D(TabletPageWidget);

    TabletAreaSelectionDialog selectionDialog;
    selectionDialog.setupWidget( getTabletAreaMapping(), d->deviceNameStylus);
    selectionDialog.select( getScreenAreaMapping() );

    if (selectionDialog.exec() == KDialog::Accepted) {
        d->tabletAreaMapping = selectionDialog.getMappings();
        onProfileChanged();
    }
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


const QString& TabletPageWidget::getScreenAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->screenAreaMapping;
}


const QString& TabletPageWidget::getTabletAreaMapping() const
{
    Q_D (const TabletPageWidget);

    return d->tabletAreaMapping;
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
    ScreenRotation        rotation      = (lookup != NULL) ? *lookup : ScreenRotation::NONE;
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
}


void TabletPageWidget::setScreenAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    d->screenAreaMapping = value;
}


void TabletPageWidget::setTabletAreaMapping(const QString& value)
{
    Q_D (TabletPageWidget);

    if (value.isEmpty() || value.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {
        d->tabletAreaMapping = QLatin1String("-1 -1 -1 -1");

    } else {
        d->tabletAreaMapping = value;
    }
}


void TabletPageWidget::setTrackingMode(const QString& value)
{
    Q_D (TabletPageWidget);

    d->ui->trackAbsoluteRadioButton->blockSignals(true);
    d->ui->trackRelativeRadioButton->blockSignals(true);

    if (value.contains(QLatin1String("absolute"), Qt::CaseInsensitive)) {
        d->ui->padMappingScreenButton->setEnabled(true);
        d->ui->trackAbsoluteRadioButton->setChecked(true);
        d->ui->trackRelativeRadioButton->setChecked(false);
    } else {
        // screen mapping has to be reset and disabled, as it does not work in relative mode
        setScreenAreaMapping(QLatin1String("full"));
        d->ui->padMappingScreenButton->setEnabled(false);

        d->ui->trackAbsoluteRadioButton->setChecked(false);
        d->ui->trackRelativeRadioButton->setChecked(true);
    }

    d->ui->trackAbsoluteRadioButton->blockSignals(false);
    d->ui->trackRelativeRadioButton->blockSignals(false);
}


void TabletPageWidget::setupUi()
{
    Q_D (TabletPageWidget);

    d->ui->setupUi(this);

    // fill rotation combo box
    // xsetwacom's rotation is based on coordinate rotation, but we are asking the user for a tablet rotation.
    // Therefore we have to swap the values for clockwise and counterclockwise rotation.
    d->ui->rotatationSelectionComboBox->addItem(i18nc("Either no orientation or the current screen orientation is applied to the tablet.", "Default Orientation"), ScreenRotation::NONE.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated clockwise.", "Rotate Tablet Clockwise"), ScreenRotation::CCW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated counterclockwise.", "Rotate Tablet Counterclockwise"), ScreenRotation::CW.key());
    d->ui->rotatationSelectionComboBox->addItem(i18nc("The tablet will be rotated up side down.", "Rotate Tablet Upside-Down"), ScreenRotation::HALF.key());
}


