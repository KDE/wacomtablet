/*
 * Copyright 2009,2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

using namespace Wacom;

PadButtonWidget::PadButtonWidget(ProfileManagement *profileManager, QWidget *parent)
        : QWidget(parent),
        m_ui(new Ui::PadButtonWidget),
        m_profileManagement(profileManager)
{
    m_ui->setupUi(this);

    init();
}

PadButtonWidget::~PadButtonWidget()
{
    delete m_ui;
}

void PadButtonWidget::init()
{
    //get information via DBus
    QDBusInterface *deviceInterface = new QDBusInterface(QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ));

    QDBusReply<QString> deviceModel = deviceInterface->call(QLatin1String( "deviceModel" ));
    QDBusReply<QString> deviceId  = deviceInterface->call(QLatin1String( "deviceId" ));

    delete deviceInterface;

    KSharedConfig::Ptr deviceConfig = KSharedConfig::openConfig(KStandardDirs::locate("data", QLatin1String( "wacomtablet/data/wacom_devicelist" )), KConfig::SimpleConfig, "data");
    KConfigGroup deviceGroup = KConfigGroup(deviceConfig, deviceId);

    int padButtons = deviceGroup.readEntry("padbuttons").toInt();

    QLabel *buttonLabel;
    QLabel *actionLabel;
    KComboBox *comboBox;
    for (int i = 1;i < 11;i++) {
        buttonLabel = m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1Label").arg(i));
        actionLabel = m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        comboBox = m_ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));

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
        m_ui->padImage->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/%1.png").arg(padLayout))));
    }

    if (deviceGroup.readEntry("wheel").contains( QLatin1String( "no" ))) {
        m_ui->wheelGroupBox->setVisible(false);
    } else {
        m_ui->wheelUpComboBox->clear();
        fillComboBox(m_ui->wheelUpComboBox);
        m_ui->wheelDnComboBox->clear();
        fillComboBox(m_ui->wheelDnComboBox);
        m_ui->wheelGroupBox->setVisible(true);
    }

    if (deviceGroup.readEntry("touchring").contains( QLatin1String( "no" ))) {
        m_ui->tochRingGroupBox->setVisible(false);
    } else {
        m_ui->ringUpComboBox->clear();
        fillComboBox(m_ui->ringUpComboBox);
        m_ui->ringDnComboBox->clear();
        fillComboBox(m_ui->ringDnComboBox);
        m_ui->tochRingGroupBox->setVisible(true);
    }

    if (deviceGroup.readEntry("touchstripl").contains( QLatin1String( "no" ))) {
        m_ui->stripLUpLabel->setVisible(false);
        m_ui->stripLUpComboBox->setVisible(false);
        m_ui->stripLUpActionLabel->setVisible(false);
        m_ui->stripLDnLabel->setVisible(false);
        m_ui->stripLDnComboBox->setVisible(false);
        m_ui->stripLDnActionLabel->setVisible(false);
    } else {
        m_ui->stripLUpLabel->setVisible(true);
        m_ui->stripLUpComboBox->clear();
        fillComboBox(m_ui->stripLUpComboBox);
        m_ui->stripLUpComboBox->setVisible(true);
        m_ui->stripLUpActionLabel->setVisible(true);
        m_ui->stripLDnLabel->setVisible(true);
        m_ui->stripLDnComboBox->clear();
        fillComboBox(m_ui->stripLDnComboBox);
        m_ui->stripLDnComboBox->setVisible(true);
        m_ui->stripLDnActionLabel->setVisible(true);
    }
    if (deviceGroup.readEntry("touchstripr").contains( QLatin1String( "no" ))) {
        m_ui->stripRUpLabel->setVisible(false);
        m_ui->stripRUpComboBox->setVisible(false);
        m_ui->stripRUpActionLabel->setVisible(false);
        m_ui->stripRDnLabel->setVisible(false);
        m_ui->stripRDnComboBox->setVisible(false);
        m_ui->stripRDnActionLabel->setVisible(false);
    } else {
        m_ui->stripRUpLabel->setVisible(true);
        m_ui->stripRUpComboBox->clear();
        fillComboBox(m_ui->stripRUpComboBox);
        m_ui->stripRUpComboBox->setVisible(true);
        m_ui->stripRUpActionLabel->setVisible(true);
        m_ui->stripRDnLabel->setVisible(true);
        m_ui->stripRDnComboBox->clear();
        fillComboBox(m_ui->stripRDnComboBox);
        m_ui->stripRDnComboBox->setVisible(true);
        m_ui->stripRDnActionLabel->setVisible(true);
    }
    if (deviceGroup.readEntry("touchstripl").contains(QLatin1String( "no" )) && deviceGroup.readEntry("touchstripr").contains( QLatin1String( "no" ))) {
        m_ui->tochStripGroupBox->setVisible(false);
    } else {
        m_ui->tochStripGroupBox->setVisible(true);
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
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));

    padConfig.writeEntry("Button1",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button1ComboBox->currentIndex(), m_ui->button1ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button2",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button2ComboBox->currentIndex(), m_ui->button2ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button3",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button3ComboBox->currentIndex(), m_ui->button3ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button4",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button4ComboBox->currentIndex(), m_ui->button4ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button5",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button5ComboBox->currentIndex(), m_ui->button5ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button6",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button6ComboBox->currentIndex(), m_ui->button6ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button7",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button7ComboBox->currentIndex(), m_ui->button7ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button8",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button8ComboBox->currentIndex(), m_ui->button8ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button9",  m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button9ComboBox->currentIndex(), m_ui->button9ActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("Button10", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->button10ComboBox->currentIndex(), m_ui->button10ActionLabel->property("KeySquence").toString()));

    padConfig.writeEntry("StripLeftUp", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->stripLUpComboBox->currentIndex(), m_ui->stripLUpActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("StripLeftDown", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->stripLDnComboBox->currentIndex(), m_ui->stripLDnActionLabel->property("KeySquence").toString()));

    padConfig.writeEntry("StripRightUp", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->stripRUpComboBox->currentIndex(), m_ui->stripRUpActionLabel->property("KeySquence").toString()));
    padConfig.writeEntry("StripRightDown", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->stripRDnComboBox->currentIndex(), m_ui->stripRDnActionLabel->property("KeySquence").toString()));

    if (m_ui->ringUpComboBox->isVisible()) {
        padConfig.writeEntry("XAbsWheelUp", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->ringUpComboBox->currentIndex(), m_ui->ringUpActionLabel->property("KeySquence").toString()));
        padConfig.writeEntry("XAbsWheelDown", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->ringDnComboBox->currentIndex(), m_ui->ringDnActionLabel->property("KeySquence").toString()));
    }
    if (m_ui->wheelUpComboBox->isVisible()) {
        padConfig.writeEntry("XAbsWheelUp", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->wheelUpComboBox->currentIndex(), m_ui->wheelUpActionLabel->property("KeySquence").toString()));
        padConfig.writeEntry("XAbsWheelDown", m_profileManagement->transformButtonToConfig((ProfileManagement::PadButton) m_ui->wheelDnComboBox->currentIndex(), m_ui->wheelDnActionLabel->property("KeySquence").toString()));
    }

    padConfig.sync();
}

void PadButtonWidget::loadFromProfile()
{
    KConfigGroup padConfig = m_profileManagement->configGroup(QLatin1String( "pad" ));

    KComboBox *buttonComboBox;
    QLabel *buttonActionLabel;
    QString readEntry;
    ProfileManagement::PadButton modeSwitch;

    for (int i = 1;i < 11 ;i++) {
        buttonActionLabel = m_ui->buttonGroupBox->findChild<QLabel *>(QString::fromLatin1("button%1ActionLabel").arg(i));
        buttonComboBox = m_ui->buttonGroupBox->findChild<KComboBox *>(QString::fromLatin1("button%1ComboBox").arg(i));

        if (!buttonActionLabel) {
            continue;
        }
        if (!buttonComboBox) {
            continue;
        }

        readEntry = padConfig.readEntry(QString::fromLatin1("Button%1").arg(i));
        modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);

        buttonComboBox->blockSignals(true);
        buttonComboBox->setCurrentIndex(modeSwitch);
        buttonComboBox->blockSignals(false);
        buttonActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
        buttonActionLabel->setProperty("KeySquence", m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
    }

    readEntry = padConfig.readEntry("XAbsWheelUp");           //RelWUp is ignored for now
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->wheelUpComboBox->blockSignals(true);
    m_ui->wheelUpComboBox->setCurrentIndex(modeSwitch);
    m_ui->wheelUpComboBox->blockSignals(false);
    m_ui->wheelUpActionLabel->setText(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
    m_ui->ringUpComboBox->blockSignals(true);
    m_ui->ringUpComboBox->setCurrentIndex(modeSwitch);
    m_ui->ringUpComboBox->blockSignals(false);
    m_ui->ringUpActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    m_ui->ringUpActionLabel->setProperty("KeySquence", m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padConfig.readEntry("XAbsWheelDown");           //RelWDown is ignored for now
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->wheelDnComboBox->blockSignals(true);
    m_ui->wheelDnComboBox->setCurrentIndex(modeSwitch);
    m_ui->wheelDnComboBox->blockSignals(false);
    m_ui->wheelDnActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    m_ui->ringDnComboBox->blockSignals(true);
    m_ui->ringDnComboBox->setCurrentIndex(modeSwitch);
    m_ui->ringDnComboBox->blockSignals(false);
    m_ui->ringDnActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    m_ui->ringDnActionLabel->setProperty("KeySquence", m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padConfig.readEntry("StripLeftDown");
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->stripLDnComboBox->blockSignals(true);
    m_ui->stripLDnComboBox->setCurrentIndex(modeSwitch);
    m_ui->stripLDnComboBox->blockSignals(false);
    m_ui->stripLDnActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    readEntry = padConfig.readEntry("StripLeftUp");
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->stripLUpComboBox->blockSignals(true);
    m_ui->stripLUpComboBox->setCurrentIndex(modeSwitch);
    m_ui->stripLUpComboBox->blockSignals(false);
    m_ui->stripLUpActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    m_ui->stripLUpActionLabel->setProperty("KeySquence", m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = padConfig.readEntry("StripRightDown");
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->stripRDnComboBox->blockSignals(true);
    m_ui->stripRDnComboBox->setCurrentIndex(modeSwitch);
    m_ui->stripRDnComboBox->blockSignals(false);
    m_ui->stripRDnActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    readEntry = padConfig.readEntry("StripRightUp");
    modeSwitch = m_profileManagement->getPadButtonFunction(readEntry);
    m_ui->stripRUpComboBox->blockSignals(true);
    m_ui->stripRUpComboBox->setCurrentIndex(modeSwitch);
    m_ui->stripRUpComboBox->blockSignals(false);
    m_ui->stripRUpActionLabel->setText(transformShortcut(m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    m_ui->stripRUpActionLabel->setProperty("KeySquence", m_profileManagement->transformButtonFromConfig(modeSwitch, readEntry));
}

void PadButtonWidget::reloadWidget()
{
    init();
}

void PadButtonWidget::selectKeyFunction(int selection)
{
    QObject* sender = const_cast<QObject*>(QObject::sender());
    QString senderName = sender->objectName();
    senderName.replace(QRegExp( QLatin1String( "ComboBox") ), QLatin1String( "ActionLabel" ));

    QLabel *buttonActionLabel = m_ui->buttonGroupBox->findChild<QLabel *>(senderName);

    if (!buttonActionLabel) {
        buttonActionLabel = m_ui->tochStripGroupBox->findChild<QLabel *>(senderName);
        if (!buttonActionLabel) {
            buttonActionLabel = m_ui->tochRingGroupBox->findChild<QLabel *>(senderName);
            if (!buttonActionLabel) {
                buttonActionLabel = m_ui->wheelGroupBox->findChild<QLabel *>(senderName);
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
