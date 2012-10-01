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

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class PadButtonWidgetPrivate {
    public:
        std::auto_ptr<Ui::PadButtonWidget>  m_ui;                /**< Handler to the padbuttonwidget.ui file */
}; // CLASS
}  // NAMESPACE


PadButtonWidget::PadButtonWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new PadButtonWidgetPrivate)
{
    Q_D( PadButtonWidget );

    d->m_ui = std::auto_ptr<Ui::PadButtonWidget>(new Ui::PadButtonWidget);
    d->m_ui->setupUi( this );

    init();
}

PadButtonWidget::~PadButtonWidget()
{
    delete this->d_ptr;
}


void PadButtonWidget::init()
{
    Q_D( PadButtonWidget );

    QDBusReply<QString> deviceModel = DBusTabletInterface::instance().getInformation(TabletInfo::TabletModel);
    QDBusReply<QString> deviceId    = DBusTabletInterface::instance().getInformation(TabletInfo::TabletId);

    KSharedConfig::Ptr deviceConfig = KSharedConfig::openConfig(KStandardDirs::locate("data", QLatin1String( "wacomtablet/data/wacom_devicelist" )), KConfig::SimpleConfig, "data");
    KConfigGroup deviceGroup = KConfigGroup(deviceConfig, deviceId);

    int padButtons = deviceGroup.readEntry("padbuttons").toInt();

    QLabel *buttonLabel;
    QLabel *actionLabel;
    KComboBox *comboBox;
    for (int i = 1;i < 11;i++) {
        buttonLabel = d->m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1Label").arg(i));
        actionLabel = d->m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        comboBox = d->m_ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));

        if (!buttonLabel) {
            continue;
        }
        if (!actionLabel) {
            continue;
        }
        if (!comboBox) {
            continue;
        }

        if (i <= padButtons) {
            buttonLabel->setVisible(true);
            actionLabel->setVisible(true);
            comboBox->clear();
            fillComboBox(comboBox);
            comboBox->setVisible(true);
        } else {
            buttonLabel->setVisible(false);
            actionLabel->setVisible(false);
            comboBox->setVisible(false);
        }
    }

    QString padLayout = deviceGroup.readEntry("layout");
    if (KStandardDirs::exists(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout)))) {
        d->m_ui->padImage->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout))));
    }

    if (deviceGroup.readEntry("wheel").contains( QLatin1String( "no" ))) {
        d->m_ui->wheelGroupBox->setVisible(false);
    } else {
        d->m_ui->wheelUpComboBox->clear();
        fillComboBox(d->m_ui->wheelUpComboBox);
        d->m_ui->wheelDnComboBox->clear();
        fillComboBox(d->m_ui->wheelDnComboBox);
        d->m_ui->wheelGroupBox->setVisible(true);
    }

    if (deviceGroup.readEntry("touchring").contains( QLatin1String( "no" ))) {
        d->m_ui->tochRingGroupBox->setVisible(false);
    } else {
        d->m_ui->ringUpComboBox->clear();
        fillComboBox(d->m_ui->ringUpComboBox);
        d->m_ui->ringDnComboBox->clear();
        fillComboBox(d->m_ui->ringDnComboBox);
        d->m_ui->tochRingGroupBox->setVisible(true);
    }

    if (deviceGroup.readEntry("touchstripl").contains( QLatin1String( "no" ))) {
        d->m_ui->stripLUpLabel->setVisible(false);
        d->m_ui->stripLUpComboBox->setVisible(false);
        d->m_ui->stripLUpActionLabel->setVisible(false);
        d->m_ui->stripLDnLabel->setVisible(false);
        d->m_ui->stripLDnComboBox->setVisible(false);
        d->m_ui->stripLDnActionLabel->setVisible(false);
    } else {
        d->m_ui->stripLUpLabel->setVisible(true);
        d->m_ui->stripLUpComboBox->clear();
        fillComboBox(d->m_ui->stripLUpComboBox);
        d->m_ui->stripLUpComboBox->setVisible(true);
        d->m_ui->stripLUpActionLabel->setVisible(true);
        d->m_ui->stripLDnLabel->setVisible(true);
        d->m_ui->stripLDnComboBox->clear();
        fillComboBox(d->m_ui->stripLDnComboBox);
        d->m_ui->stripLDnComboBox->setVisible(true);
        d->m_ui->stripLDnActionLabel->setVisible(true);
    }
    if (deviceGroup.readEntry("touchstripr").contains( QLatin1String( "no" ))) {
        d->m_ui->stripRUpLabel->setVisible(false);
        d->m_ui->stripRUpComboBox->setVisible(false);
        d->m_ui->stripRUpActionLabel->setVisible(false);
        d->m_ui->stripRDnLabel->setVisible(false);
        d->m_ui->stripRDnComboBox->setVisible(false);
        d->m_ui->stripRDnActionLabel->setVisible(false);
    } else {
        d->m_ui->stripRUpLabel->setVisible(true);
        d->m_ui->stripRUpComboBox->clear();
        fillComboBox(d->m_ui->stripRUpComboBox);
        d->m_ui->stripRUpComboBox->setVisible(true);
        d->m_ui->stripRUpActionLabel->setVisible(true);
        d->m_ui->stripRDnLabel->setVisible(true);
        d->m_ui->stripRDnComboBox->clear();
        fillComboBox(d->m_ui->stripRDnComboBox);
        d->m_ui->stripRDnComboBox->setVisible(true);
        d->m_ui->stripRDnActionLabel->setVisible(true);
    }
    if (deviceGroup.readEntry("touchstripl").contains(QLatin1String( "no" )) && deviceGroup.readEntry("touchstripr").contains( QLatin1String( "no" ))) {
        d->m_ui->tochStripGroupBox->setVisible(false);
    } else {
        d->m_ui->tochStripGroupBox->setVisible(true);
    }
}

void PadButtonWidget::fillComboBox(KComboBox *comboBox)
{
    comboBox->blockSignals(true);
    comboBox->addItem(i18nc("Disable button function", "Disable"), ProfileManagement::Pad_Disable);
    comboBox->addItem(i18nc("Indicates the use of one of the standard buttons (1-32)", "Button..."), ProfileManagement::Pad_Button);
    comboBox->addItem(i18nc("Indicates the use of a specific key/keystroke", "Keystroke..."), ProfileManagement::Pad_Keystroke);
    comboBox->blockSignals(false);
}

void PadButtonWidget::saveToProfile()
{
    Q_D( PadButtonWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);

    padProfile.setProperty(Property::Button1,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button1ComboBox->currentIndex(), d->m_ui->button1ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button2,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button2ComboBox->currentIndex(), d->m_ui->button2ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button3,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button3ComboBox->currentIndex(), d->m_ui->button3ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button4,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button4ComboBox->currentIndex(), d->m_ui->button4ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button5,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button5ComboBox->currentIndex(), d->m_ui->button5ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button6,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button6ComboBox->currentIndex(), d->m_ui->button6ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button7,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button7ComboBox->currentIndex(), d->m_ui->button7ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button8,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button8ComboBox->currentIndex(), d->m_ui->button8ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button9,  profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button9ComboBox->currentIndex(), d->m_ui->button9ActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::Button10, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->button10ComboBox->currentIndex(), d->m_ui->button10ActionLabel->property("KeySquence").toString()));

    padProfile.setProperty(Property::StripLeftUp, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->stripLUpComboBox->currentIndex(), d->m_ui->stripLUpActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::StripLeftDown, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->stripLDnComboBox->currentIndex(), d->m_ui->stripLDnActionLabel->property("KeySquence").toString()));

    padProfile.setProperty(Property::StripRightUp, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->stripRUpComboBox->currentIndex(), d->m_ui->stripRUpActionLabel->property("KeySquence").toString()));
    padProfile.setProperty(Property::StripRightDown, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->stripRDnComboBox->currentIndex(), d->m_ui->stripRDnActionLabel->property("KeySquence").toString()));

    if (d->m_ui->ringUpComboBox->isVisible()) {
        padProfile.setProperty(Property::AbsWheelUp, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->ringUpComboBox->currentIndex(), d->m_ui->ringUpActionLabel->property("KeySquence").toString()));
        padProfile.setProperty(Property::AbsWheelDown, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->ringDnComboBox->currentIndex(), d->m_ui->ringDnActionLabel->property("KeySquence").toString()));
    }
    if (d->m_ui->wheelUpComboBox->isVisible()) {
        padProfile.setProperty(Property::AbsWheelUp, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->wheelUpComboBox->currentIndex(), d->m_ui->wheelUpActionLabel->property("KeySquence").toString()));
        padProfile.setProperty(Property::AbsWheelDown, profileManagement->transformButtonToConfig((ProfileManagement::PadButton) d->m_ui->wheelDnComboBox->currentIndex(), d->m_ui->wheelDnActionLabel->property("KeySquence").toString()));
    }

    profileManagement->saveDeviceProfile(padProfile);
}

void PadButtonWidget::loadFromProfile()
{
    Q_D( PadButtonWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();
    DeviceProfile      padProfile        = profileManagement->loadDeviceProfile(DeviceType::Pad);

    KComboBox *buttonComboBox;
    QLabel *buttonActionLabel;
    QString readEntry;
    ProfileManagement::PadButton modeSwitch;

    for (int i = 1;i < 11 ;i++) {
        buttonActionLabel = d->m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        buttonComboBox = d->m_ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));

        if (!buttonActionLabel) {
            continue;
        }
        if (!buttonComboBox) {
            continue;
        }

        readEntry = padProfile.getButton(i);
        modeSwitch = profileManagement->getPadButtonFunction(readEntry);

        buttonComboBox->blockSignals(true);
        buttonComboBox->setCurrentIndex(modeSwitch);
        buttonComboBox->blockSignals(false);
        buttonActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
        buttonActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
    }

    readEntry = padProfile.getProperty(Property::AbsWheelUp);           //RelWUp is ignored for now
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->wheelUpComboBox->blockSignals(true);
    d->m_ui->wheelUpComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->wheelUpComboBox->blockSignals(false);
    d->m_ui->wheelUpActionLabel->setText(profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
    d->m_ui->ringUpComboBox->blockSignals(true);
    d->m_ui->ringUpComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->ringUpComboBox->blockSignals(false);
    d->m_ui->ringUpActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->ringUpActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padProfile.getProperty(Property::AbsWheelDown);           //RelWDown is ignored for now
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->wheelDnComboBox->blockSignals(true);
    d->m_ui->wheelDnComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->wheelDnComboBox->blockSignals(false);
    d->m_ui->wheelDnActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->ringDnComboBox->blockSignals(true);
    d->m_ui->ringDnComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->ringDnComboBox->blockSignals(false);
    d->m_ui->ringDnActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->ringDnActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padProfile.getProperty(Property::StripLeftDown);
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->stripLDnComboBox->blockSignals(true);
    d->m_ui->stripLDnComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->stripLDnComboBox->blockSignals(false);
    d->m_ui->stripLDnActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    readEntry = padProfile.getProperty(Property::StripLeftUp);
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->stripLUpComboBox->blockSignals(true);
    d->m_ui->stripLUpComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->stripLUpComboBox->blockSignals(false);
    d->m_ui->stripLUpActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->stripLUpActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padProfile.getProperty(Property::StripRightDown);
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->stripRDnComboBox->blockSignals(true);
    d->m_ui->stripRDnComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->stripRDnComboBox->blockSignals(false);
    d->m_ui->stripRDnActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    readEntry = padProfile.getProperty(Property::StripRightUp);
    modeSwitch = profileManagement->getPadButtonFunction(readEntry);
    d->m_ui->stripRUpComboBox->blockSignals(true);
    d->m_ui->stripRUpComboBox->setCurrentIndex(modeSwitch);
    d->m_ui->stripRUpComboBox->blockSignals(false);
    d->m_ui->stripRUpActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->stripRUpActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
}

void PadButtonWidget::reloadWidget()
{
    init();
}

void PadButtonWidget::selectKeyFunction(int selection)
{
    Q_D( PadButtonWidget );

    QObject* sender = const_cast<QObject*>(QObject::sender());
    QString senderName = sender->objectName();
    senderName.replace(QRegExp( QLatin1String( "ComboBox") ), QLatin1String( "ActionLabel" ));

    QLabel *buttonActionLabel = d->m_ui->buttonGroupBox->findChild<QLabel *>(senderName);

    if (!buttonActionLabel) {
        buttonActionLabel = d->m_ui->tochStripGroupBox->findChild<QLabel *>(senderName);
        if (!buttonActionLabel) {
            buttonActionLabel = d->m_ui->tochRingGroupBox->findChild<QLabel *>(senderName);
            if (!buttonActionLabel) {
                buttonActionLabel = d->m_ui->wheelGroupBox->findChild<QLabel *>(senderName);
                if (!buttonActionLabel) {
                    kError() << "No ActionLabel found!";
                    return;
                }
            }
        }
    }

    QPointer <SelectKeyButton> skb = new SelectKeyButton(this);
    QPointer <SelectKeyStroke> sks = new SelectKeyStroke(this);
    int ret;

    switch ((ProfileManagement::PadButton)selection) {
    case ProfileManagement::Pad_Button:
        ret = skb->exec();

        if (ret == QDialog::Accepted) {
            buttonActionLabel->setText(skb->keyButton());
            buttonActionLabel->setProperty("KeySquence", skb->keyButton());
        }
        break;

    case ProfileManagement::Pad_Keystroke:
        ret = sks->exec();

        if (ret == KDialog::Accepted) {
            buttonActionLabel->setText(transformShortcut(sks->keyStroke()));
            buttonActionLabel->setProperty("KeySquence", sks->keyStroke());
        }
        break;

    case ProfileManagement::Pad_Disable:
        buttonActionLabel->clear();
        buttonActionLabel->setProperty("KeySquence", QString());
        break;
    }

    emit changed();

    delete skb;
    delete sks;
}

QString PadButtonWidget::transformShortcut(QString sequence)
{
    QString transform = sequence;
    transform.replace( QRegExp( QLatin1String( "^\\s" ) ), QLatin1String( "" ) );
    transform.replace( QRegExp( QLatin1String( "\\s" ) ), QLatin1String( "+" ) );

    QList< KGlobalShortcutInfo > list = KGlobalAccel::getGlobalShortcutsByKey( QKeySequence(transform) );

    if(!list.isEmpty()) {
        return list.at(0).uniqueName();
    }
    else {
        sequence.replace( QRegExp( QLatin1String( "([^\\s])\\+" ) ), QLatin1String( "\\1 " ) );
        sequence = sequence.toLower();
        return sequence;
    }
}
