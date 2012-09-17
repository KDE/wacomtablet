/*
 * Copyright 2009,2010,2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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
#include "selectkeybutton.h"
#include "selectkeystroke.h"
#include "presscurvedialog.h"
#include "profilemanagement.h"

// common includes
#include "property.h"
#include "deviceprofile.h"
#include "dbustabletinterface.h"

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

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class PenWidgetPrivate {
    public:
        std::auto_ptr<Ui::PenWidget>  m_ui;                /**< Handler to the penwidget.ui file */
}; // CLASS
}  // NAMESPACE


PenWidget::PenWidget( QWidget* parent )
    : QWidget( parent ), d_ptr(new PenWidgetPrivate)
{
    Q_D( PenWidget );

    d->m_ui = std::auto_ptr<Ui::PenWidget>(new Ui::PenWidget);
    d->m_ui->setupUi( this );

    fillComboBox( d->m_ui->button2ComboBox );
    fillComboBox( d->m_ui->button3ComboBox );

    d->m_ui->penLabel->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/pen.png"))));

    //TODO Don't show double click slider box
    // does not work yet
    d->m_ui->DblClickBox->hide();
}

PenWidget::~PenWidget()
{
    delete this->d_ptr;
}


void PenWidget::saveToProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( QLatin1String( "stylus" ) );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( QLatin1String( "eraser" ) );

    // eraser feel / tip feel
    eraserProfile.setProperty( Property::Threshold, QString::number(d->m_ui->eraserSlider->value()) );
    eraserProfile.setProperty( Property::PressureCurve, d->m_ui->eraserPressureButton->property( "curve" ).toString() );
    stylusProfile.setProperty( Property::Threshold, QString::number(d->m_ui->tipSlider->value()) );
    stylusProfile.setProperty( Property::PressureCurve, d->m_ui->tipPressureButton->property( "curve" ).toString() );

    // button 2 and 3 config
    eraserProfile.setProperty( Property::Button2, profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) d->m_ui->button2ComboBox->itemData( d->m_ui->button2ComboBox->currentIndex() ).toInt(), d->m_ui->button2ActionLabel->property("KeySquence").toString() ) );
    eraserProfile.setProperty( Property::Button3, profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) d->m_ui->button3ComboBox->itemData( d->m_ui->button3ComboBox->currentIndex() ).toInt(), d->m_ui->button3ActionLabel->property("KeySquence").toString() ) );
    stylusProfile.setProperty( Property::Button2, profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) d->m_ui->button2ComboBox->itemData( d->m_ui->button2ComboBox->currentIndex() ).toInt(), d->m_ui->button2ActionLabel->property("KeySquence").toString() ) );
    stylusProfile.setProperty( Property::Button3, profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) d->m_ui->button3ComboBox->itemData( d->m_ui->button3ComboBox->currentIndex() ).toInt(), d->m_ui->button3ActionLabel->property("KeySquence").toString() ) );

    //stylusProfile.setProperty( "DoubleClickInterval", m_ui->doubleClickSlider->value() );
    //eraserProfile.setProperty( "DoubleClickInterval", m_ui->doubleClickSlider->value() );

    if( d->m_ui->radioButton_Absolute->isChecked() ) {
        stylusProfile.setProperty( Property::Mode, QLatin1String("absolute") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("absolute") );
    }
    else {
        stylusProfile.setProperty( Property::Mode, QLatin1String("relative") );
        eraserProfile.setProperty( Property::Mode, QLatin1String("relative") );
    }

    if( d->m_ui->tpcCheckBox->isChecked() ) {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("on") );
    }
    else {
        stylusProfile.setProperty( Property::TabletPcButton, QLatin1String("off") );
    }

    profileManagement->saveDeviceProfile(stylusProfile);
    profileManagement->saveDeviceProfile(eraserProfile);

    // now save the cursor specific settings
    //KConfigGroup cursorConfig = m_profileManagement->configGroup( QLatin1String( "cursor" ) );

    //cursorConfig.writeEntry( "CursorProximity", m_ui->cursorProximity->value() );
    // does not work and cause a lot of troubles with wacom mouse devices
}

void PenWidget::loadFromProfile()
{
    Q_D( PenWidget );

    ProfileManagement* profileManagement = &ProfileManagement::instance();

    DeviceProfile stylusProfile = profileManagement->loadDeviceProfile( QLatin1String( "stylus" ) );
    DeviceProfile eraserProfile = profileManagement->loadDeviceProfile( QLatin1String( "eraser" ) );
    DeviceProfile cursorProfile = profileManagement->loadDeviceProfile( QLatin1String( "cursor" ) );

    // eraser feel / tip feel
    d->m_ui->eraserSlider->setValue( eraserProfile.getProperty( Property::Threshold ).toInt() );
    d->m_ui->eraserPressureButton->setProperty( "curve", eraserProfile.getProperty( Property::PressureCurve ) );
    d->m_ui->tipSlider->setValue( stylusProfile.getProperty( Property::Threshold ).toInt() );
    d->m_ui->tipPressureButton->setProperty( "curve", stylusProfile.getProperty( Property::PressureCurve ) );

    // button 2 and 3 config
    QString readEntry;
    ProfileManagement::PenButton modeSwitch;

    // we show only stylus button config
    // later these settings are saved for stylus and erase and thus both are always the same
    readEntry = stylusProfile.getProperty( Property::Button2 );
    modeSwitch = profileManagement->getPenButtonFunction( readEntry );
    d->m_ui->button2ComboBox->blockSignals( true );
    d->m_ui->button2ComboBox->setCurrentIndex( modeSwitch );
    d->m_ui->button2ComboBox->blockSignals( false );
    d->m_ui->button2ActionLabel->setText( profileManagement->transformButtonFromConfig( modeSwitch, readEntry ) );
    d->m_ui->button2ActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->button2ActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    readEntry = stylusProfile.getProperty( Property::Button3 );
    modeSwitch = profileManagement->getPenButtonFunction( readEntry );
    d->m_ui->button3ComboBox->blockSignals( true );
    d->m_ui->button3ComboBox->setCurrentIndex( modeSwitch );
    d->m_ui->button3ComboBox->blockSignals( false );
    d->m_ui->button3ActionLabel->setText( profileManagement->transformButtonFromConfig( modeSwitch, readEntry ) );
    d->m_ui->button3ActionLabel->setText(transformShortcut(profileManagement->transformButtonFromConfig(modeSwitch, readEntry)));
    d->m_ui->button3ActionLabel->setProperty("KeySquence", profileManagement->transformButtonFromConfig(modeSwitch, readEntry));

    //Double Click Distance
    //m_ui->doubleClickSlider->setValue( stylusProfile.getProperty( "DoubleClickInterval" ).toInt() );

    // Cursor Settings
    if( stylusProfile.getProperty( Property::Mode ).toInt() == 1 || stylusProfile.getProperty( Property::Mode ) == QLatin1String( "absolute" ) ) {
        d->m_ui->radioButton_Absolute->setChecked( true );
        d->m_ui->radioButton_Relative->setChecked( false );
    }
    else {
        d->m_ui->radioButton_Absolute->setChecked( false );
        d->m_ui->radioButton_Relative->setChecked( true );
    }

    // hover click settings
    QString tabletPCButton = stylusProfile.getProperty( Property::TabletPcButton );
    if( tabletPCButton == QLatin1String( "on" ) ) {
        d->m_ui->tpcCheckBox->setChecked( true );
    }
    else {
        d->m_ui->tpcCheckBox->setChecked( false );
    }

    //m_ui->cursorProximity->setValue( cursorConfig.readEntry( QLatin1String( "CursorProximity" ) ).toInt() );
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
    QString senderName = sender->objectName();
    KComboBox *cb = d->m_ui->buttonGroupBox->findChild<KComboBox *>( senderName );
    senderName.replace( QRegExp( QLatin1String( "ComboBox" ) ), QLatin1String( "ActionLabel" ) );

    QLabel *buttonActionLabel = d->m_ui->buttonGroupBox->findChild<QLabel *>( senderName );

    if( !buttonActionLabel ) {
        kError() << "No ActionLabel found!";
        return;
    }

    QPointer <SelectKeyButton> skb = new SelectKeyButton( this );
    QPointer <SelectKeyStroke> sks = new SelectKeyStroke( this );
    int ret;

    //returns the saved enum data for this index
    ProfileManagement::PenButton selectionEnum = ( ProfileManagement::PenButton )(( cb->itemData( selection ) ).toInt() );

    switch( selectionEnum ) {
    case ProfileManagement::Pen_LeftClick:
        buttonActionLabel->setText( cb->currentText() );
        buttonActionLabel->setProperty("KeySquence", cb->currentText());
        break;

    case ProfileManagement::Pen_RightClick:
        buttonActionLabel->setText( cb->currentText() );
        buttonActionLabel->setProperty("KeySquence", cb->currentText());
        break;

    case ProfileManagement::Pen_MiddleClick:
        buttonActionLabel->setText( cb->currentText() );
        buttonActionLabel->setProperty("KeySquence", cb->currentText());
        break;

    case ProfileManagement::Pen_Button:
        ret = skb->exec();

        if( ret == QDialog::Accepted ) {
            buttonActionLabel->setText( transformShortcut(skb->keyButton()) );
            buttonActionLabel->setProperty("KeySquence", skb->keyButton());
        }
        break;

    case ProfileManagement::Pen_Keystroke:
        ret = sks->exec();
        if( ret == KDialog::Accepted ) {
            buttonActionLabel->setText( transformShortcut(sks->keyStroke()) );
            buttonActionLabel->setProperty("KeySquence", sks->keyStroke());
        }
        break;

    case ProfileManagement::Pen_ModeToggle:
        buttonActionLabel->setText( cb->currentText() );
        buttonActionLabel->setProperty("KeySquence", cb->currentText());
        break;

    case ProfileManagement::Pen_DisplayToggle:
        buttonActionLabel->setText( cb->currentText() );
        buttonActionLabel->setProperty("KeySquence", cb->currentText());
        break;

    case ProfileManagement::Pen_Disable:
        buttonActionLabel->setText( QString() );
        buttonActionLabel->setProperty("KeySquence", QString());
        break;
    }

    emit changed();

    delete skb;
    delete sks;
}

void PenWidget::changeEraserPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Eraser, d->m_ui->eraserPressureButton->property( "curve" ).toString());
    d->m_ui->eraserPressureButton->setProperty( "curve", result );
}

void PenWidget::changeTipPressCurve()
{
    Q_D( PenWidget );

    QString result = changePressCurve(DeviceType::Stylus, d->m_ui->tipPressureButton->property( "curve" ).toString());
    d->m_ui->tipPressureButton->setProperty( "curve", result );
}

QString PenWidget::changePressCurve(const DeviceType& deviceType, const QString& startValue)
{
    QString              result(startValue);
    PressCurveDialog     selectPC( this );

    selectPC.setDeviceType(deviceType);
    selectPC.setControllPoints( startValue );

    if( selectPC.exec() == KDialog::Accepted ) {
        result = selectPC.getControllPoints();
        emit changed();
    }
    else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        DBusTabletInterface::instance().setProperty( deviceType, Property::PressureCurve, startValue );
    }

    return result;
}

void PenWidget::fillComboBox( KComboBox *comboBox )
{
    comboBox->blockSignals( true );
    comboBox->addItem( i18nc( "Disable button function", "Disable" ), ProfileManagement::Pen_Disable );
    comboBox->addItem( i18nc( "Left mouse click", "Left Click" ), ProfileManagement::Pen_LeftClick );
    comboBox->addItem( i18nc( "Middle mouse click", "Middle Click" ), ProfileManagement::Pen_MiddleClick );
    comboBox->addItem( i18nc( "Right mouse click", "Right Click" ), ProfileManagement::Pen_RightClick );
    comboBox->addItem( i18nc( "Indicates the use of one of the standard buttons (1-32)", "Button..." ), ProfileManagement::Pen_Button );
    comboBox->addItem( i18nc( "Indicates the use of a specific key/keystroke", "Keystroke..." ), ProfileManagement::Pen_Keystroke );
    comboBox->addItem( i18nc( "Function to toggle between absolute/relative mousemode", "Mode Toggle" ), ProfileManagement::Pen_ModeToggle );
    comboBox->addItem( i18nc( "Function to toggle between single/multi display support", "Display Toggle" ), ProfileManagement::Pen_DisplayToggle );
    comboBox->blockSignals( false );
}

QString PenWidget::transformShortcut(QString sequence)
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
