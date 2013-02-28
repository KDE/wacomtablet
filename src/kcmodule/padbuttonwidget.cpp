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

#include "padbuttonwidget.h"
#include "ui_padbuttonwidget.h"

#include "buttonactionselectiondialog.h"
#include "buttonactionselectorwidget.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "tabletinfo.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "buttonshortcut.h"

// stdlib
#include<memory>

//KDE includes
#include <KDE/KComboBox>
#include <KDE/KStandardDirs>
#include <KDE/KGlobalAccel>
#include <kglobalshortcutinfo.h>
#include <KDE/KDebug>

//Qt includes
#include <QtGui/QPixmap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QKeySequence>
#include <QtCore/QPointer>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QList>

using namespace Wacom;

/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class PadButtonWidgetPrivate {
        public:
            PadButtonWidgetPrivate() : ui(new Ui::PadButtonWidget) {}
            ~PadButtonWidgetPrivate() {
                delete ui;
            }

            Ui::PadButtonWidget* ui;
    };
} // NAMESPACE



PadButtonWidget::PadButtonWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new PadButtonWidgetPrivate)
{
    setupUi();
    reloadWidget();
}


PadButtonWidget::~PadButtonWidget()
{
    delete this->d_ptr;
}


void PadButtonWidget::saveToProfile()
{
    Q_D( PadButtonWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);

    // save button shortcuts
    ButtonActionSelectorWidget* buttonSelector;

    for (int i = 1 ; i < 11 ; ++i) {
        buttonSelector = this->findChild<ButtonActionSelectorWidget*>(QString::fromLatin1("button%1ActionSelector").arg(i));

        if (buttonSelector && buttonSelector->isVisible()) {
            padProfile.setButton(i, buttonSelector->getShortcut().toString());
        }
    }

    // save strip shortcuts
    if (d->ui->touchStripGroupBox->isVisible()) {
        if (d->ui->leftStripWidget->isVisible()) {
            padProfile.setProperty(Property::StripLeftUp,    d->ui->leftStripUpSelector->getShortcut().toString());
            padProfile.setProperty(Property::StripLeftDown,  d->ui->leftStripDownSelector->getShortcut().toString());
        }

        if (d->ui->rightStripWidget->isVisible()) {
            padProfile.setProperty(Property::StripRightUp,   d->ui->rightStripUpSelector->getShortcut().toString());
            padProfile.setProperty(Property::StripRightDown, d->ui->rightStripDownSelector->getShortcut().toString());
        }
    }

    // save wheel and ring shortcuts
    if (d->ui->touchRingGroupBox->isVisible() || d->ui->wheelGroupBox->isVisible()) {
        // ring and wheel shortcuts are treated the same but only one value may be written,
        // as the other one could be empty. Use whichever value we can get our hands on first.
        if (d->ui->ringUpSelector->getShortcut().isSet()) {
            padProfile.setProperty(Property::AbsWheelUp,   d->ui->ringUpSelector->getShortcut().toString());
            padProfile.setProperty(Property::AbsWheelUp,   d->ui->ringUpSelector->getShortcut().toString());
        } else {
            padProfile.setProperty(Property::AbsWheelUp,   d->ui->wheelUpSelector->getShortcut().toString());
            padProfile.setProperty(Property::AbsWheelUp,   d->ui->wheelUpSelector->getShortcut().toString());
        }

        if (d->ui->ringDownSelector->getShortcut().isSet()) {
            padProfile.setProperty(Property::AbsWheelDown, d->ui->ringDownSelector->getShortcut().toString());
            padProfile.setProperty(Property::AbsWheelDown, d->ui->ringDownSelector->getShortcut().toString());
        } else {
            padProfile.setProperty(Property::AbsWheelDown, d->ui->wheelDownSelector->getShortcut().toString());
            padProfile.setProperty(Property::AbsWheelDown, d->ui->wheelDownSelector->getShortcut().toString());
        }
    }

    profileManagement->saveDeviceProfile(padProfile);
}


void PadButtonWidget::loadFromProfile()
{
    Q_D( PadButtonWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);
    QString            propertyValue;

    // set button shortcuts
    ButtonActionSelectorWidget* buttonSelector;

    for (int i = 1;i < 11 ;i++) {
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


void PadButtonWidget::reloadWidget()
{
    Q_D( PadButtonWidget );

    QDBusReply<QString> deviceModel  = DBusTabletInterface::instance().getInformation(TabletInfo::TabletModel);
    QDBusReply<QString> deviceId     = DBusTabletInterface::instance().getInformation(TabletInfo::TabletId);

    int padButtons = DBusTabletInterface::instance().getInformationAsInt(TabletInfo::NumPadButtons);

    QLabel*                     buttonLabel;
    ButtonActionSelectorWidget* buttonSelector;

    for (int i = 1;i < 11;i++) {
        buttonSelector = this->findChild<ButtonActionSelectorWidget*>(QString::fromLatin1("button%1ActionSelector").arg(i));
        buttonLabel    = this->findChild<QLabel *>(QString::fromLatin1("button%1Label").arg(i));

        if (!buttonSelector || !buttonLabel) {
            continue;
        }

        if (i <= padButtons) {
            buttonLabel->setVisible(true);
            buttonSelector->setVisible(true);
        } else {
            buttonLabel->setVisible(false);
            buttonSelector->setVisible(false);
        }
    }

    QString padLayout = DBusTabletInterface::instance().getInformationAsString(TabletInfo::ButtonLayout);
    if (KStandardDirs::exists(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout)))) {
        d->ui->padImage->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout))));
    }

    bool hasLeftTouchStrip  = DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasLeftTouchStrip);
    bool hasRightTouchStrip = DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasRightTouchStrip);

    if (!hasLeftTouchStrip && !hasRightTouchStrip) {
        d->ui->touchStripGroupBox->setVisible(false);

    } else {
        d->ui->touchStripGroupBox->setVisible(true);

        // Hide the strip input widgets directly instead of
        // their parent widget, to keep the layout stable.
        // Hiding the parent widget will mess up the layout!
        if (!hasLeftTouchStrip) {
            d->ui->leftStripUpLabel->setVisible(false);
            d->ui->leftStripUpSelector->setVisible(false);
            d->ui->leftStripDownLabel->setVisible(false);
            d->ui->leftStripDownSelector->setVisible(false);
        } else {
            d->ui->leftStripUpLabel->setVisible(true);
            d->ui->leftStripUpSelector->setVisible(true);
            d->ui->leftStripDownLabel->setVisible(true);
            d->ui->leftStripDownSelector->setVisible(true);
        }

        if (!hasRightTouchStrip) {
            d->ui->rightStripUpLabel->setVisible(false);
            d->ui->rightStripUpSelector->setVisible(false);
            d->ui->rightStripDownLabel->setVisible(false);
            d->ui->rightStripDownSelector->setVisible(false);
        } else {
            d->ui->rightStripUpLabel->setVisible(true);
            d->ui->rightStripUpSelector->setVisible(true);
            d->ui->rightStripDownLabel->setVisible(true);
            d->ui->rightStripDownSelector->setVisible(true);
        }
    }

    if (!DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasTouchRing)) {
        d->ui->touchRingGroupBox->setVisible(false);
    } else {
        d->ui->touchRingGroupBox->setVisible(true);
    }

    if (!DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasWheel)) {
        d->ui->wheelGroupBox->setVisible(false);
    } else {
        d->ui->wheelGroupBox->setVisible(true);
    }
}


void PadButtonWidget::onButtonActionChanged()
{
    emit changed();
}


void PadButtonWidget::setupUi()
{
    Q_D (PadButtonWidget);

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

    connect ( d->ui->leftStripUpSelector,    SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->leftStripDownSelector,  SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->rightStripUpSelector,   SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->rightStripDownSelector, SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );

    connect ( d->ui->ringUpSelector,         SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->ringDownSelector,       SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );

    connect ( d->ui->wheelUpSelector,        SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
    connect ( d->ui->wheelDownSelector,      SIGNAL (buttonActionChanged(ButtonShortcut)), this, SLOT (onButtonActionChanged()) );
}
