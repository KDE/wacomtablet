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

#include "buttonpagewidget.h"
#include "ui_buttonpagewidget.h"

#include "buttonactionselectiondialog.h"
#include "buttonactionselectorwidget.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "tabletinfo.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "buttonshortcut.h"
#include "stringutils.h"

// stdlib
#include <memory>

//KDE includes
#include <KGlobalAccel>
#include <KGlobalShortcutInfo>

//Qt includes
#include <QStandardPaths>
#include <QPixmap>
#include <QDialog>
#include <QLabel>
#include <QKeySequence>
#include <QPointer>
#include <QDBusInterface>
#include <QDBusReply>
#include <QList>
#include <QFile>

using namespace Wacom;

/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class ButtonPageWidgetPrivate {
        public:
            ButtonPageWidgetPrivate() : ui(new Ui::ButtonPageWidget) {}
            ~ButtonPageWidgetPrivate() {
                delete ui;
            }

            Ui::ButtonPageWidget* ui;
            QString               tabletId;
    };
} // NAMESPACE

QString findLayoutFile(const QString &filename) {
    if (filename.isEmpty())
        return QString();

    const QString builtinLayout  = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QString::fromLatin1("wacomtablet/images/%1.png").arg(filename));
    if (QFile::exists(builtinLayout)) {
        return builtinLayout;
    } else if (QFile::exists(filename)) {
        return filename;
    }

    return QString();
}

ButtonPageWidget::ButtonPageWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new ButtonPageWidgetPrivate)
{
    setupUi();
    reloadWidget();
}


ButtonPageWidget::~ButtonPageWidget()
{
    delete this->d_ptr;
}

void ButtonPageWidget::setTabletId(const QString &tabletId)
{
    Q_D( ButtonPageWidget );
    d->tabletId = tabletId;
}

void ButtonPageWidget::saveToProfile()
{
    Q_D( ButtonPageWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);

    // save button shortcuts
    ButtonActionSelectorWidget* buttonSelector;

    for (int i = 1 ; i < 19 ; ++i) {
        buttonSelector = this->findChild<ButtonActionSelectorWidget*>(QString::fromLatin1("button%1ActionSelector").arg(i));

        if (buttonSelector && buttonSelector->isEnabled()) {
            padProfile.setButton(i, buttonSelector->getShortcut().toString());
        } else {
            // Make sure only valid buttons are set.
            // If invalid button numbers are set they might overwrite mapped buttons.
            // For instance, if button 4 gets mapped to 9, then setting button 9 will
            // overwrite the value from button 4 unless the device actually has a
            // button 9 which would then in turn have another mapping to another X11
            // button number, e.g. 13.
            padProfile.setButton(i, QString());
        }
    }

    // save strip shortcuts - reset invalid ones - same reasons as above
    QString stripLUp, stripRUp, stripLDown, stripRDown;

    if (d->ui->touchStripGroupBox->isEnabled()) {
        if (d->ui->leftStripWidget->isEnabled()) {
            stripLUp   = d->ui->leftStripUpSelector->getShortcut().toString();
            stripLDown = d->ui->leftStripDownSelector->getShortcut().toString();
        }

        if (d->ui->rightStripWidget->isEnabled()) {
            stripRUp   = d->ui->rightStripUpSelector->getShortcut().toString();
            stripRDown = d->ui->rightStripDownSelector->getShortcut().toString();
        }
    }

    padProfile.setProperty(Property::StripLeftUp,    stripLUp);
    padProfile.setProperty(Property::StripLeftDown,  stripLDown);
    padProfile.setProperty(Property::StripRightUp,   stripRUp);
    padProfile.setProperty(Property::StripRightDown, stripRDown);

    // save wheel and ring shortcuts - reset invalid values - same reasons as above
    QString absWUp, absWDown;

    if (d->ui->touchRingGroupBox->isEnabled() || d->ui->wheelGroupBox->isEnabled()) {
        // ring and wheel shortcuts are treated the same but only one value may be written,
        // as the other one could be empty. Use whichever value we can get our hands on first.
        if (d->ui->ringUpSelector->getShortcut().isSet()) {
            absWUp = d->ui->ringUpSelector->getShortcut().toString();
        } else {
            absWUp = d->ui->wheelUpSelector->getShortcut().toString();
        }

        if (d->ui->ringDownSelector->getShortcut().isSet()) {
            absWDown = d->ui->ringDownSelector->getShortcut().toString();
        } else {
            absWDown = d->ui->wheelDownSelector->getShortcut().toString();
        }
    }

    padProfile.setProperty(Property::AbsWheelUp,    absWUp);
    padProfile.setProperty(Property::AbsWheel2Up,   absWUp);
    padProfile.setProperty(Property::AbsWheelDown,  absWDown);
    padProfile.setProperty(Property::AbsWheel2Down, absWDown);

    // save device profile
    profileManagement->saveDeviceProfile(padProfile);
}


void ButtonPageWidget::loadFromProfile()
{
    Q_D( ButtonPageWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);
    QString            propertyValue;

    // set button shortcuts
    ButtonActionSelectorWidget* buttonSelector;

    for (int i = 1;i < 19 ;i++) {
        buttonSelector = this->findChild<ButtonActionSelectorWidget*>(QString::fromLatin1("button%1ActionSelector").arg(i));
        propertyValue  = padProfile.getButton(i);

        if (buttonSelector) {
            buttonSelector->setShortcut(ButtonShortcut(propertyValue));
        }
    }

    // set wheel and ring shortcuts
    propertyValue = padProfile.getProperty(Property::AbsWheelUp);
    d->ui->wheelUpSelector->setShortcut(ButtonShortcut(propertyValue));
    d->ui->ringUpSelector->setShortcut(ButtonShortcut(propertyValue));

    propertyValue = padProfile.getProperty(Property::AbsWheelDown);
    d->ui->wheelDownSelector->setShortcut(ButtonShortcut(propertyValue));
    d->ui->ringDownSelector->setShortcut(ButtonShortcut(propertyValue));

    // set strip shortcuts
    propertyValue = padProfile.getProperty(Property::StripLeftUp);
    d->ui->leftStripUpSelector->setShortcut(ButtonShortcut(propertyValue));

    propertyValue = padProfile.getProperty(Property::StripLeftDown);
    d->ui->leftStripDownSelector->setShortcut(ButtonShortcut(propertyValue));

    propertyValue = padProfile.getProperty(Property::StripRightUp);
    d->ui->rightStripUpSelector->setShortcut(ButtonShortcut(propertyValue));

    propertyValue = padProfile.getProperty(Property::StripRightDown);
    d->ui->rightStripDownSelector->setShortcut(ButtonShortcut(propertyValue));
}


void ButtonPageWidget::reloadWidget()
{
    Q_D( ButtonPageWidget );

    const int padButtons = DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::NumPadButtons.key()).value().toInt();

    QLabel *buttonLabel = nullptr;
    ButtonActionSelectorWidget *buttonSelector = nullptr;
    for (int i = 1; i < 19; i++) {
        buttonSelector = this->findChild<ButtonActionSelectorWidget*>(QString::fromLatin1("button%1ActionSelector").arg(i));
        buttonLabel    = this->findChild<QLabel *>(QString::fromLatin1("button%1Label").arg(i));

        if (!buttonSelector || !buttonLabel) {
            continue;
        }

        // we have to disable the widgets as well because when writing
        // the configuration file we can not reliably determine their state
        // based on visibility.
        if (i <= padButtons) {
            buttonLabel->setVisible(true);
            buttonSelector->setEnabled(true);
            buttonSelector->setVisible(true);
        } else {
            buttonLabel->setVisible(false);
            buttonSelector->setEnabled(false);
            buttonSelector->setVisible(false);
        }
    }

    const QString padLayoutProperty = DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::ButtonLayout.key());
    const QString layoutFile = findLayoutFile(padLayoutProperty);
    if (!layoutFile.isEmpty()) {
        // FIXME: libwacom svg's are large in size and have small labels
        // This looks ugly for some devices/with some themes, think of a better layout
        d->ui->padImage->setPixmap(QPixmap(layoutFile));
    }

    const bool anyButtonsOrLayout = padButtons > 0 || (!layoutFile.isEmpty());
    d->ui->buttonGroupBox->setVisible(anyButtonsOrLayout);

    bool hasLeftTouchStrip  = StringUtils::asBool(DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::HasLeftTouchStrip.key()));
    bool hasRightTouchStrip = StringUtils::asBool(DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::HasRightTouchStrip.key()));

    if (!hasLeftTouchStrip && !hasRightTouchStrip) {
        d->ui->touchStripGroupBox->setEnabled(false);
        d->ui->touchStripGroupBox->setVisible(false);

    } else {
        d->ui->touchStripGroupBox->setEnabled(true);
        d->ui->touchStripGroupBox->setVisible(true);

        // Hide the strip input widgets directly instead of
        // their parent widget, to keep the layout stable.
        // Hiding the parent widget will mess up the layout!
        // Also disable the widgets so we can reliable determine
        // which settings to save.
        if (!hasLeftTouchStrip) {
            d->ui->leftStripWidget->setEnabled(false);
            d->ui->leftStripUpLabel->setVisible(false);
            d->ui->leftStripUpSelector->setVisible(false);
            d->ui->leftStripDownLabel->setVisible(false);
            d->ui->leftStripDownSelector->setVisible(false);
        } else {
            d->ui->leftStripWidget->setEnabled(true);
            d->ui->leftStripUpLabel->setVisible(true);
            d->ui->leftStripUpSelector->setVisible(true);
            d->ui->leftStripDownLabel->setVisible(true);
            d->ui->leftStripDownSelector->setVisible(true);
        }

        if (!hasRightTouchStrip) {
            d->ui->rightStripWidget->setEnabled(false);
            d->ui->rightStripUpLabel->setVisible(false);
            d->ui->rightStripUpSelector->setVisible(false);
            d->ui->rightStripDownLabel->setVisible(false);
            d->ui->rightStripDownSelector->setVisible(false);
        } else {
            d->ui->rightStripWidget->setEnabled(true);
            d->ui->rightStripUpLabel->setVisible(true);
            d->ui->rightStripUpSelector->setVisible(true);
            d->ui->rightStripDownLabel->setVisible(true);
            d->ui->rightStripDownSelector->setVisible(true);
        }
    }

    if (!StringUtils::asBool(DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::HasTouchRing.key()))) {
        d->ui->touchRingGroupBox->setEnabled(false);
        d->ui->touchRingGroupBox->setVisible(false);
    } else {
        d->ui->touchRingGroupBox->setEnabled(true);
        d->ui->touchRingGroupBox->setVisible(true);
    }

    if (!StringUtils::asBool(DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::HasWheel.key()))) {
        d->ui->wheelGroupBox->setEnabled(false);
        d->ui->wheelGroupBox->setVisible(false);
    } else {
        d->ui->wheelGroupBox->setEnabled(true);
        d->ui->wheelGroupBox->setVisible(true);
    }
}


void ButtonPageWidget::onButtonActionChanged()
{
    emit changed();
}


void ButtonPageWidget::setupUi()
{
    Q_D (ButtonPageWidget);

    d->ui->setupUi( this );

    connect ( d->ui->button1ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button2ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button3ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button4ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button5ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button6ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button7ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button8ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button9ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button10ActionSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button11ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button12ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button13ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button14ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button15ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button16ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button17ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->button18ActionSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );

    connect ( d->ui->leftStripUpSelector,    SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->leftStripDownSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->rightStripUpSelector,   SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->rightStripDownSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );

    connect ( d->ui->ringUpSelector,         SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->ringDownSelector,       SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );

    connect ( d->ui->wheelUpSelector,        SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->wheelDownSelector,      SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
}
