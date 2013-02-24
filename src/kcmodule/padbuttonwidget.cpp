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
#include "selectkeybutton.h"
#include "selectkeystroke.h"
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
 * Static class members.
 */
const char* PadButtonWidget::LABEL_PROPERTY_KEYSEQUENCE = "KeySequence";


/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class PadButtonWidgetPrivate {
        public:
            std::auto_ptr<Ui::PadButtonWidget>  ui;
    };
} // NAMESPACE



PadButtonWidget::PadButtonWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new PadButtonWidgetPrivate)
{
    Q_D( PadButtonWidget );

    d->ui = std::auto_ptr<Ui::PadButtonWidget>(new Ui::PadButtonWidget);
    d->ui->setupUi( this );

    init();
}


PadButtonWidget::~PadButtonWidget()
{
    delete this->d_ptr;
}


void PadButtonWidget::init()
{
    Q_D( PadButtonWidget );

    QDBusReply<QString> deviceModel  = DBusTabletInterface::instance().getInformation(TabletInfo::TabletModel);
    QDBusReply<QString> deviceId     = DBusTabletInterface::instance().getInformation(TabletInfo::TabletId);

    int padButtons = DBusTabletInterface::instance().getInformationAsInt(TabletInfo::NumPadButtons);

    QLabel    *buttonLabel;
    QLabel    *actionLabel;
    KComboBox *comboBox;

    for (int i = 1;i < 11;i++) {
        buttonLabel = d->ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1Label").arg(i));
        actionLabel = d->ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        comboBox    = d->ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));

        if (!buttonLabel || !actionLabel || !comboBox) {
            continue;
        }

        if (i <= padButtons) {
            buttonLabel->setVisible(true);
            actionLabel->setVisible(true);
            fillComboBox(comboBox);
            comboBox->setVisible(true);
        } else {
            buttonLabel->setVisible(false);
            actionLabel->setVisible(false);
            comboBox->setVisible(false);
        }
    }

    QString padLayout = DBusTabletInterface::instance().getInformationAsString(TabletInfo::ButtonLayout);
    if (KStandardDirs::exists(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout)))) {
        d->ui->padImage->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout))));
    }

    if (!DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasWheel)) {
        d->ui->wheelGroupBox->setVisible(false);
    } else {
        fillComboBox(d->ui->wheelUpComboBox);
        fillComboBox(d->ui->wheelDnComboBox);
        d->ui->wheelGroupBox->setVisible(true);
    }

    if (!DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasTouchRing)) {
        d->ui->tochRingGroupBox->setVisible(false);
    } else {
        fillComboBox(d->ui->ringUpComboBox);
        fillComboBox(d->ui->ringDnComboBox);
        d->ui->tochRingGroupBox->setVisible(true);
    }

    bool hasLeftTouchStrip = DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasLeftTouchStrip);

    if (!hasLeftTouchStrip) {
        d->ui->stripLUpLabel->setVisible(false);
        d->ui->stripLUpComboBox->setVisible(false);
        d->ui->stripLUpActionLabel->setVisible(false);
        d->ui->stripLDnLabel->setVisible(false);
        d->ui->stripLDnComboBox->setVisible(false);
        d->ui->stripLDnActionLabel->setVisible(false);
    } else {
        d->ui->stripLUpLabel->setVisible(true);
        fillComboBox(d->ui->stripLUpComboBox);
        d->ui->stripLUpComboBox->setVisible(true);
        d->ui->stripLUpActionLabel->setVisible(true);
        d->ui->stripLDnLabel->setVisible(true);
        fillComboBox(d->ui->stripLDnComboBox);
        d->ui->stripLDnComboBox->setVisible(true);
        d->ui->stripLDnActionLabel->setVisible(true);
    }

    bool hasRightTouchStrip = DBusTabletInterface::instance().getInformationAsBool(TabletInfo::HasRightTouchStrip);

    if (!hasRightTouchStrip) {
        d->ui->stripRUpLabel->setVisible(false);
        d->ui->stripRUpComboBox->setVisible(false);
        d->ui->stripRUpActionLabel->setVisible(false);
        d->ui->stripRDnLabel->setVisible(false);
        d->ui->stripRDnComboBox->setVisible(false);
        d->ui->stripRDnActionLabel->setVisible(false);
    } else {
        d->ui->stripRUpLabel->setVisible(true);
        fillComboBox(d->ui->stripRUpComboBox);
        d->ui->stripRUpComboBox->setVisible(true);
        d->ui->stripRUpActionLabel->setVisible(true);
        d->ui->stripRDnLabel->setVisible(true);
        fillComboBox(d->ui->stripRDnComboBox);
        d->ui->stripRDnComboBox->setVisible(true);
        d->ui->stripRDnActionLabel->setVisible(true);
    }

    if (hasLeftTouchStrip || hasRightTouchStrip) {
        d->ui->tochStripGroupBox->setVisible(true);
    } else {
        d->ui->tochStripGroupBox->setVisible(false);
    }
}


void PadButtonWidget::saveToProfile()
{
    Q_D( PadButtonWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);

    // save button shortcuts
    QLabel *buttonActionLabel;

    for (int i = 1 ; i < 11 ; ++i) {
        buttonActionLabel = d->ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        padProfile.setButton(i, getButtonActionShortcut(buttonActionLabel));
    }

    // save strip shortcuts
    padProfile.setProperty(Property::StripLeftUp, getButtonActionShortcut(d->ui->stripLUpActionLabel));
    padProfile.setProperty(Property::StripLeftDown, getButtonActionShortcut(d->ui->stripLDnActionLabel));
    padProfile.setProperty(Property::StripRightUp, getButtonActionShortcut(d->ui->stripRUpActionLabel));
    padProfile.setProperty(Property::StripRightDown, getButtonActionShortcut(d->ui->stripRDnActionLabel));

    // save wheel and ring shortcuts
    if (d->ui->ringUpComboBox->isVisible()) {
        padProfile.setProperty(Property::AbsWheelUp, getButtonActionShortcut(d->ui->ringUpActionLabel));
        padProfile.setProperty(Property::AbsWheelDown, getButtonActionShortcut(d->ui->ringDnActionLabel));
    }

    if (d->ui->wheelUpComboBox->isVisible()) {
        padProfile.setProperty(Property::AbsWheelUp, getButtonActionShortcut(d->ui->wheelUpActionLabel));
        padProfile.setProperty(Property::AbsWheelDown, getButtonActionShortcut(d->ui->wheelDnActionLabel));
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
    KComboBox *buttonComboBox;
    QLabel    *buttonActionLabel;

    for (int i = 1;i < 11 ;i++) {
        buttonActionLabel = d->ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        buttonComboBox    = d->ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));
        propertyValue     = padProfile.getButton(i);

        setButtonActionShortcut(buttonComboBox, buttonActionLabel, propertyValue);
    }

    // set wheel and ring shortcuts
    propertyValue = padProfile.getProperty(Property::AbsWheelUp);
    setButtonActionShortcut(d->ui->wheelUpComboBox, d->ui->wheelUpActionLabel, propertyValue);
    setButtonActionShortcut(d->ui->ringUpComboBox, d->ui->ringUpActionLabel, propertyValue);

    propertyValue = padProfile.getProperty(Property::AbsWheelDown);
    setButtonActionShortcut(d->ui->wheelDnComboBox, d->ui->wheelDnActionLabel, propertyValue);
    setButtonActionShortcut(d->ui->ringDnComboBox, d->ui->ringDnActionLabel, propertyValue);

    // set strip shortcuts
    propertyValue = padProfile.getProperty(Property::StripLeftDown);
    setButtonActionShortcut(d->ui->stripLDnComboBox, d->ui->stripLDnActionLabel, propertyValue);

    propertyValue = padProfile.getProperty(Property::StripLeftUp);
    setButtonActionShortcut(d->ui->stripLUpComboBox, d->ui->stripLUpActionLabel, propertyValue);

    propertyValue = padProfile.getProperty(Property::StripRightDown);
    setButtonActionShortcut(d->ui->stripRDnComboBox, d->ui->stripRDnActionLabel, propertyValue);

    propertyValue = padProfile.getProperty(Property::StripRightUp);
    setButtonActionShortcut(d->ui->stripRUpComboBox, d->ui->stripRUpActionLabel, propertyValue);
}

void PadButtonWidget::reloadWidget()
{
    init();
}



void PadButtonWidget::selectKeyFunction(int selection)
{
    Q_D( PadButtonWidget );

    QObject* sender = const_cast<QObject*>(QObject::sender());

    if (!sender) {
        kWarning() << QLatin1String("PadButtonWidget::selectKeyFunction() was called manually but it should only be called by Qt's signal-slot mechanism!");
        return;
    }

    QString buttonActionComboBoxName = sender->objectName();
    QString buttonActionLabelName = buttonActionComboBoxName;
    buttonActionLabelName.replace( QLatin1String( "ComboBox" ), QLatin1String( "ActionLabel" ) );

    KComboBox* buttonActionComboBox = d->ui->buttonGroupBox->findChild<KComboBox *>(buttonActionComboBoxName);
    QLabel*    buttonActionLabel    = d->ui->buttonGroupBox->findChild<QLabel *>(buttonActionLabelName);

    if (!buttonActionLabel || !buttonActionComboBox) {
        buttonActionComboBox = d->ui->tochStripGroupBox->findChild<KComboBox *>(buttonActionComboBoxName);
        buttonActionLabel    = d->ui->tochStripGroupBox->findChild<QLabel *>(buttonActionLabelName);

        if (!buttonActionLabel || !buttonActionComboBox) {
            buttonActionComboBox = d->ui->tochRingGroupBox->findChild<KComboBox *>(buttonActionComboBoxName);
            buttonActionLabel    = d->ui->tochRingGroupBox->findChild<QLabel *>(buttonActionLabelName);

            if (!buttonActionLabel || !buttonActionComboBox) {
                buttonActionComboBox = d->ui->wheelGroupBox->findChild<KComboBox *>(buttonActionComboBoxName);
                buttonActionLabel    = d->ui->wheelGroupBox->findChild<QLabel *>(buttonActionLabelName);

                if (!buttonActionLabel || !buttonActionComboBox) {
                    kError() << "Internal Error: No ActionLabel/ActionComboBox pair found!";
                    return;
                }
            }
        }
    }

    onButtonActionSelectionChanged(selection, *buttonActionComboBox, *buttonActionLabel);
}



void PadButtonWidget::fillComboBox(KComboBox *comboBox)
{
    comboBox->clear();
    comboBox->blockSignals(true);
    comboBox->addItem(i18nc("Disable button function", "Disabled"), PadButtonWidget::ActionDisabled);
    comboBox->addItem(i18nc("Indicates the use of one of the standard buttons (1-32)", "Mouse..."), PadButtonWidget::ActionMouseClick);
    comboBox->addItem(i18nc("Indicates the use of a specific key/keystroke", "Keyboard..."), PadButtonWidget::ActionKeyStroke);
    comboBox->blockSignals(false);
}


PadButtonWidget::PadButtonAction PadButtonWidget::getButtonAction(const ButtonShortcut& shortcut) const
{
    PadButtonAction action = PadButtonWidget::ActionDisabled;

    if (shortcut.isButton()) {
        action = PadButtonWidget::ActionMouseClick;
    } else if (shortcut.isKeystroke() || shortcut.isModifier()) {
        action = PadButtonWidget::ActionKeyStroke;
    }

    return action;
}


const QString PadButtonWidget::getButtonActionShortcut(QLabel* label) const
{
    if (!label || !label->isVisible()) {
        return QString();
    }

    return ButtonShortcut(label->property(LABEL_PROPERTY_KEYSEQUENCE).toString()).toString();
}


const QString PadButtonWidget::getShortcutDisplayName(const ButtonShortcut& shortcut) const
{
    QString displayName;

    // lookup keystrokes from the global list of shortcuts
    if (shortcut.isKeystroke() && shortcut.isSet()) {
        QList< KGlobalShortcutInfo > globalShortcutList = KGlobalAccel::getGlobalShortcutsByKey(QKeySequence(shortcut.toQKeySequenceString()));

        if(!globalShortcutList.isEmpty()) {
            displayName = globalShortcutList.at(0).uniqueName();
        }
    }

    // use the standard display string if a name could not be determined by now
    if (displayName.isEmpty()) {
        displayName = shortcut.toDisplayString();
    }

    return displayName;
}


void PadButtonWidget::onButtonActionSelectionChanged(int selection, KComboBox& combo, QLabel& label)
{
    QPointer <SelectKeyButton> skb      = new SelectKeyButton(this);
    QPointer <SelectKeyStroke> sks      = new SelectKeyStroke(this);

    // get the action associated with this selection
    PadButtonAction action = (PadButtonAction)(combo.itemData(selection).toInt());

    // determine new shortcut
    ButtonShortcut previousShortcut(getButtonActionShortcut(&label));
    ButtonShortcut shortcut(previousShortcut);

    switch (action) {
        case PadButtonWidget::ActionDisabled:
            shortcut.clear();
            break;

        case PadButtonWidget::ActionMouseClick:
            if (previousShortcut.isButton()) {
                skb->setButton(previousShortcut.getButton());
            }
            if (skb->exec() == QDialog::Accepted) {
                shortcut.setButton(skb->getButton());
            }
            break;

        case PadButtonWidget::ActionKeyStroke:
            if (sks->exec() == QDialog::Accepted) {
                shortcut = sks->shortcut();
            }
            break;
    }

    delete skb;
    delete sks;

    // update ui elements
    setButtonActionShortcut(&combo, &label, shortcut.toString());

    // signal change event if user changed something
    if (shortcut != previousShortcut) {
        emit changed();
    }
}



bool PadButtonWidget::setButtonActionShortcut(KComboBox* combo, QLabel* label, const QString& shortcutSequence) const
{
    if (!combo || !label) {
        return false;
    }

    ButtonShortcut shortcut(shortcutSequence);

    // find combo box entry
    int comboItemData = getButtonAction(shortcut);
    int comboIndex    = combo->findData(comboItemData);

    if (comboIndex == -1) {
        return false;
    }

    // update combo box and label
    combo->blockSignals(true);
    combo->setCurrentIndex(comboItemData);
    combo->blockSignals(false);

    label->setText(getShortcutDisplayName(shortcut));
    label->setProperty(LABEL_PROPERTY_KEYSEQUENCE, shortcut.toString());

    return true;
}
