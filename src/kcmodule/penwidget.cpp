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

#include "penwidget.h"
#include "ui_penwidget.h"

#include "buttonactionselectiondialog.h"
#include "presscurvedialog.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"
#include "buttonshortcut.h"

// stdlib
#include <memory>

//KDE includes
#include <KDE/KStandardDirs>
#include <KDE/KGlobalAccel>
#include <kglobalshortcutinfo.h>
#include <KDE/KDebug>

//Qt includes
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtCore/QPointer>
#include <QtDBus/QDBusReply>

using namespace Wacom;

/*
 * Static class members.
 */
const char* PenWidget::LABEL_PROPERTY_KEYSEQUENCE = "KeySequence";

/*
 * D-Pointer class for private members.
 */
namespace Wacom {
    class PenWidgetPrivate {
        public:
            std::auto_ptr<Ui::PenWidget>  ui;                /**< Handler to the penwidget.ui file */
    };
} // NAMESPACE


PenWidget::PenWidget( QWidget* parent )
    : QWidget( parent ), d_ptr(new PenWidgetPrivate)
{
    Q_D( PenWidget );

    d->ui = std::auto_ptr<Ui::PenWidget>(new Ui::PenWidget);
    d->ui->setupUi( this );

    fillComboBox( d->ui->button2ComboBox );
    fillComboBox( d->ui->button3ComboBox );

    d->ui->penLabel->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/pen.png"))));

    //TODO Don't show double click slider box
    // does not work yet
    d->ui->DblClickBox->hide();
}

PenWidget::~PenWidget()
{
    delete this->d_ptr;
}


void PenWidget::saveToProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );

    // eraser feel / tip feel
    eraserProfile.setProperty( Property::Threshold, QString::number(d->ui->eraserSlider->value()) );
    eraserProfile.setProperty( Property::PressureCurve, d->ui->eraserPressureButton->property( "curve" ).toString() );
    stylusProfile.setProperty( Property::Threshold, QString::number(d->ui->tipSlider->value()) );
    stylusProfile.setProperty( Property::PressureCurve, d->ui->tipPressureButton->property( "curve" ).toString() );

    // button 2 and 3 config
    eraserProfile.setProperty( Property::Button2, getButtonActionShortcut(d->ui->button2ActionLabel) );
    eraserProfile.setProperty( Property::Button3, getButtonActionShortcut(d->ui->button3ActionLabel) );
    stylusProfile.setProperty( Property::Button2, getButtonActionShortcut(d->ui->button2ActionLabel) );
    stylusProfile.setProperty( Property::Button3, getButtonActionShortcut(d->ui->button3ActionLabel) );

    //stylusProfile.setProperty( "DoubleClickInterval", ui->doubleClickSlider->value() );
    //eraserProfile.setProperty( "DoubleClickInterval", ui->doubleClickSlider->value() );

    if( d->ui->radioButton_Absolute->isChecked() ) {
        stylusProfile.setProperty( Property::Mode, QLatin1String("absolute") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("absolute") );
    }
    else {
        stylusProfile.setProperty( Property::Mode, QLatin1String("relative") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("relative") );
    }

    if( d->ui->tpcCheckBox->isChecked() ) {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("on") );
    }
    else {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("off") );
    }

    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);
}



void PenWidget::loadFromProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( DeviceType::Stylus );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( DeviceType::Eraser );
    DeviceProfile cursorProfile = profileManagement->loadDeviceProfile( DeviceType::Cursor );

    // eraser feel / tip feel
    d->ui->eraserSlider->setValue( eraserProfile.getProperty( Property::Threshold ).toInt() );
    d->ui->eraserPressureButton->setProperty( "curve", eraserProfile.getProperty( Property::PressureCurve ) );
    d->ui->tipSlider->setValue( stylusProfile.getProperty( Property::Threshold ).toInt() );
    d->ui->tipPressureButton->setProperty( "curve", stylusProfile.getProperty( Property::PressureCurve ) );

    // button 2 and 3 config
    QString propertyValue;

    propertyValue = stylusProfile.getProperty( Property::Button2 );
    setButtonActionShortcut(d->ui->button2ComboBox, d->ui->button2ActionLabel, propertyValue);

    propertyValue = stylusProfile.getProperty( Property::Button3 );
    setButtonActionShortcut(d->ui->button3ComboBox, d->ui->button3ActionLabel, propertyValue);

    //Double Click Distance
    //ui->doubleClickSlider->setValue( stylusProfile.getProperty( "DoubleClickInterval" ).toInt() );

    // Cursor Settings
    if( stylusProfile.getProperty( Property::Mode ).toInt() == 1 || stylusProfile.getProperty( Property::Mode ) == QLatin1String( "absolute" ) ) {
        d->ui->radioButton_Absolute->setChecked( true );
        d->ui->radioButton_Relative->setChecked( false );
    }
    else {
        d->ui->radioButton_Absolute->setChecked( false );
        d->ui->radioButton_Relative->setChecked( true );
    }

    // hover click settings
    QString tabletPCButton = stylusProfile.getProperty( Property::TabletPcButton );
    if( tabletPCButton == QLatin1String( "on" ) ) {
        d->ui->tpcCheckBox->setChecked( true );
    }
    else {
        d->ui->tpcCheckBox->setChecked( false );
    }

    //ui->cursorProximity->setValue( cursorConfig.readEntry( QLatin1String( "CursorProximity" ) ).toInt() );
}

void PenWidget::profileChanged()
{
    emit changed();
}

void PenWidget::reloadWidget()
{
}

void PenWidget::selectKeyFunction( int selection )
{
    Q_D( PenWidget );

    QObject *sender = const_cast<QObject *>( QObject::sender() );

    if (!sender) {
        kWarning() << QLatin1String("PenWidget::selectKeyFunction() was called manually but it should only be called by Qt's signal-slot mechanism!");
        return;
    }

    QString buttonActionComboBoxName = sender->objectName();
    QString buttonActionLabelName = buttonActionComboBoxName;
    buttonActionLabelName.replace( QLatin1String( "ComboBox" ), QLatin1String( "ActionLabel" ) );

    KComboBox *buttonActionComboBox = d->ui->buttonGroupBox->findChild<KComboBox *>( buttonActionComboBoxName );
    QLabel    *buttonActionLabel    = d->ui->buttonGroupBox->findChild<QLabel *>( buttonActionLabelName );

    if( !buttonActionComboBox || !buttonActionLabel ) {
        kError() << "No ActionLabel, ComboBox pair found!";
        return;
    }

    onButtonActionSelectionChanged(selection, *buttonActionComboBox, *buttonActionLabel);
}

void PenWidget::changeEraserPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Eraser, d->ui->eraserPressureButton->property( "curve" ).toString());
    d->ui->eraserPressureButton->setProperty( "curve", result );
}

void PenWidget::changeTipPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Stylus, d->ui->tipPressureButton->property( "curve" ).toString());
    d->ui->tipPressureButton->setProperty( "curve", result );
}

QString PenWidget::changePressCurve(const DeviceType& deviceType, const QString& startValue)
{
    QString              result(startValue);
    QPointer<PressCurveDialog> selectPC = new PressCurveDialog(this);

    selectPC->setDeviceType(deviceType);
    selectPC->setControllPoints( startValue );

    if( selectPC->exec() == KDialog::Accepted ) {
        result = selectPC->getControllPoints();
        emit changed();
    }
    else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        DBusTabletInterface::instance().setProperty( deviceType, Property::PressureCurve, startValue );
    }

    delete selectPC;
    return result;
}

void PenWidget::fillComboBox( KComboBox *comboBox )
{
    comboBox->blockSignals( true );
    comboBox->addItem( i18nc( "Disable button function",      "Disabled" ),  PenWidget::ActionDisabled );
    comboBox->addItem( i18nc("Indicates an assigned action.", "Action..." ), PenWidget::ActionSelected);
    comboBox->blockSignals( false );
}


PenWidget::PenButtonAction PenWidget::getButtonAction(const ButtonShortcut& shortcut) const
{
    return (shortcut.isSet() ? PenWidget::ActionSelected : PenWidget::ActionDisabled);
}


const QString PenWidget::getButtonActionShortcut(QLabel* label) const
{
    if (!label) {
        return QString();
    }

    return ButtonShortcut(label->property(LABEL_PROPERTY_KEYSEQUENCE).toString()).toString();
}


const QString PenWidget::getShortcutDisplayName(const ButtonShortcut& shortcut) const
{
    QString displayName;

    // lookup keystrokes from the global list of shortcuts
    if (shortcut.isKeystroke()) {
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


void PenWidget::onButtonActionSelectionChanged(int selection, KComboBox& combo, QLabel& label)
{
    QPointer <ButtonActionSelectionDialog> selectionDialog = new ButtonActionSelectionDialog(this);

    // get the action associated with this selection
    PenButtonAction action = (PenButtonAction)(combo.itemData(selection).toInt());

    // determine new shortcut
    ButtonShortcut previousShortcut(getButtonActionShortcut(&label));
    ButtonShortcut shortcut(previousShortcut);

    switch( action ) {
        case PenWidget::ActionDisabled:
            shortcut.clear();
            break;

        case PenWidget::ActionSelected:
            selectionDialog->setShortcut(previousShortcut);

            if (selectionDialog->exec() == QDialog::Accepted) {
                shortcut = selectionDialog->getShortcut();
            }
            break;
    }

    delete selectionDialog;

    // update UI widgets
    setButtonActionShortcut(&combo, &label, shortcut.toString());

    // emit changed signal if the shortcut changed
    if (shortcut != previousShortcut) {
        emit changed();
    }
}


bool PenWidget::setButtonActionShortcut(KComboBox* combo, QLabel* label, const QString& shortcutSequence) const
{
    if (!combo || !label) {
        return false;
    }

    ButtonShortcut shortcut(shortcutSequence);

    // determine combo box selection to set
    int comboItemData = getButtonAction(shortcut);
    int comboIndex    = combo->findData(comboItemData);

    if (comboIndex == -1) {
        return false;
    }

    // set combo box and label
    combo->blockSignals(true);
    combo->setCurrentIndex(comboIndex);
    combo->blockSignals(false);

    label->setText(getShortcutDisplayName(shortcut));
    label->setProperty(LABEL_PROPERTY_KEYSEQUENCE, shortcut.toString());

    return true;
}

