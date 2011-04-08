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

//KDE includes
#include <KDE/KStandardDirs>
#include <KDE/KKeySequenceWidget>
#include <KDE/KDebug>

//Qt includes
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtCore/QPointer>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

using namespace Wacom;

PenWidget::PenWidget( ProfileManagement *profileManager, QWidget *parent )
    : QWidget( parent ),
      m_ui( new Ui::PenWidget ),
      m_profileManagement( profileManager )
{
    m_ui->setupUi( this );
    fillComboBox( m_ui->button2ComboBox );
    fillComboBox( m_ui->button3ComboBox );

    m_ui->penLabel->setPixmap(QPixmap(KStandardDirs::locate("data", QString::fromLatin1("wacomtablet/images/pen.png"))));

    //TODO Don't show double click slider box
    // does not work yet
    m_ui->DblClickBox->hide();
    // same for the mouse speed options
    m_ui->label->hide();
    m_ui->cursorProximity->hide();
    m_ui->label_2->hide();
    m_ui->cursorAcceleration->hide();
    m_ui->label_3->hide();
    m_ui->cursorThreshold->hide();
}

PenWidget::~PenWidget()
{
    delete m_ui;
}

void PenWidget::saveToProfile()
{
    KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
    KConfigGroup eraserConfig = m_profileManagement->configGroup( QLatin1String( "eraser" ) );

    // eraser feel / tip feel
    eraserConfig.writeEntry( "Threshold", m_ui->eraserSlider->value() );
    eraserConfig.writeEntry( "PressureCurve", m_ui->eraserPressureButton->property( "curve" ).toString() );
    stylusConfig.writeEntry( "Threshold", m_ui->tipSlider->value() );
    stylusConfig.writeEntry( "PressureCurve", m_ui->tipPressureButton->property( "curve" ).toString() );

    // button 2 and 3 config
    eraserConfig.writeEntry( "Button2", m_profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) m_ui->button2ComboBox->itemData( m_ui->button2ComboBox->currentIndex() ).toInt(), m_buttonConfig.value( QLatin1String( "button2ActionLabel" ) ) ) );
    eraserConfig.writeEntry( "Button3", m_profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) m_ui->button3ComboBox->itemData( m_ui->button3ComboBox->currentIndex() ).toInt(), m_buttonConfig.value( QLatin1String( "button3ActionLabel" ) ) ) );
    stylusConfig.writeEntry( "Button2", m_profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) m_ui->button2ComboBox->itemData( m_ui->button2ComboBox->currentIndex() ).toInt(), m_buttonConfig.value( QLatin1String( "button2ActionLabel" ) ) ) );
    stylusConfig.writeEntry( "Button3", m_profileManagement->transformButtonToConfig(( ProfileManagement::PenButton ) m_ui->button3ComboBox->itemData( m_ui->button3ComboBox->currentIndex() ).toInt(), m_buttonConfig.value( QLatin1String( "button3ActionLabel" ) ) ) );

    stylusConfig.writeEntry( "DoubleClickInterval", m_ui->doubleClickSlider->value() );
    eraserConfig.writeEntry( "DoubleClickInterval", m_ui->doubleClickSlider->value() );

    if( m_ui->radioButton_Absolute->isChecked() ) {
        stylusConfig.writeEntry( "Mode", "absolute" );
        eraserConfig.writeEntry( "Mode", "absolute" );
    }
    else {
        stylusConfig.writeEntry( "Mode", "relative" );
        eraserConfig.writeEntry( "Mode", "relative" );
    }

    if( m_ui->tpcCheckBox->isChecked() ) {
        stylusConfig.writeEntry( "TabletPCButton", "on" );
    }
    else {
        stylusConfig.writeEntry( "TabletPCButton", "off" );
    }

    stylusConfig.sync();
    eraserConfig.sync();

    // now save the cursor specific settings
    KConfigGroup cursorConfig = m_profileManagement->configGroup( QLatin1String( "cursor" ) );

    cursorConfig.writeEntry( "CursorProximity", m_ui->cursorProximity->value() );
}

void PenWidget::loadFromProfile()
{
    KConfigGroup stylusConfig = m_profileManagement->configGroup( QLatin1String( "stylus" ) );
    KConfigGroup eraserConfig = m_profileManagement->configGroup( QLatin1String( "eraser" ) );

    // eraser feel / tip feel
    m_ui->eraserSlider->setValue( eraserConfig.readEntry( "Threshold" ).toInt() );
    m_ui->eraserPressureButton->setProperty( "curve", eraserConfig.readEntry( "PressureCurve" ) );
    m_ui->tipSlider->setValue( stylusConfig.readEntry( "Threshold" ).toInt() );
    m_ui->tipPressureButton->setProperty( "curve", stylusConfig.readEntry( "PressureCurve" ) );

    // button 2 and 3 config
    QString readEntry;
    ProfileManagement::PenButton modeSwitch;

    // we show only stylus button config
    // later these settings are saved for stylus and erase and thus both are always the same
    readEntry = stylusConfig.readEntry( "Button2" );
    modeSwitch = m_profileManagement->getPenButtonFunction( readEntry );
    m_ui->button2ComboBox->blockSignals( true );
    m_ui->button2ComboBox->setCurrentIndex( modeSwitch );
    m_ui->button2ComboBox->blockSignals( false );
    m_ui->button2ActionLabel->setText( m_profileManagement->transformButtonFromConfig( modeSwitch, readEntry ) );
    m_buttonConfig.insert( QLatin1String( "button2ActionLabel" ), m_ui->button2ActionLabel->text() );

    readEntry = stylusConfig.readEntry( "Button3" );
    modeSwitch = m_profileManagement->getPenButtonFunction( readEntry );
    m_ui->button3ComboBox->blockSignals( true );
    m_ui->button3ComboBox->setCurrentIndex( modeSwitch );
    m_ui->button3ComboBox->blockSignals( false );
    m_ui->button3ActionLabel->setText( m_profileManagement->transformButtonFromConfig( modeSwitch, readEntry ) );
    m_buttonConfig.insert( QLatin1String( "button3ActionLabel" ), m_ui->button3ActionLabel->text() );

    //Double Click Distance
    m_ui->doubleClickSlider->setValue( stylusConfig.readEntry( "DoubleClickInterval" ).toInt() );

    // Cursor Settings
    if( stylusConfig.readEntry( "Mode" ).toInt() == 1 || stylusConfig.readEntry( "Mode" ) == QLatin1String( "absolute" ) ) {
        m_ui->radioButton_Absolute->setChecked( true );
        m_ui->radioButton_Relative->setChecked( false );
    }
    else {
        m_ui->radioButton_Absolute->setChecked( false );
        m_ui->radioButton_Relative->setChecked( true );
    }

    // hover click settings
    QString tabletPCButton = stylusConfig.readEntry( QLatin1String( "TabletPCButton" ) );
    if( tabletPCButton == QLatin1String( "on" ) ) {
        m_ui->tpcCheckBox->setChecked( true );
    }
    else {
        m_ui->tpcCheckBox->setChecked( false );
    }

    m_ui->cursorProximity->setValue( cursorConfig.readEntry( QLatin1String( "CursorProximity" ) ).toInt() );
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
    QObject *sender = const_cast<QObject *>( QObject::sender() );
    QString senderName = sender->objectName();
    KComboBox *cb = m_ui->buttonGroupBox->findChild<KComboBox *>( senderName );
    senderName.replace( QRegExp( QLatin1String( "ComboBox" ) ), QLatin1String( "ActionLabel" ) );

    QLabel *buttonActionLabel = m_ui->buttonGroupBox->findChild<QLabel *>( senderName );

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
        m_buttonConfig.insert( senderName, QLatin1String( "button 1" ) );
        break;

    case ProfileManagement::Pen_RightClick:
        buttonActionLabel->setText( cb->currentText() );
        m_buttonConfig.insert( senderName, QLatin1String( "button 3" ) );
        break;

    case ProfileManagement::Pen_MiddleClick:
        buttonActionLabel->setText( cb->currentText() );
        m_buttonConfig.insert( senderName, QLatin1String( "button 2" ) );
        break;

    case ProfileManagement::Pen_Button:
        ret = skb->exec();

        if( ret == QDialog::Accepted ) {
            buttonActionLabel->setText( skb->keyButton() );
            m_buttonConfig.insert( senderName, skb->keyButton() );
        }
        break;

    case ProfileManagement::Pen_Keystroke:
        ret = sks->exec();
        if( ret == KDialog::Accepted ) {
            buttonActionLabel->setText( sks->keyStroke() );
            m_buttonConfig.insert( senderName, sks->keyStroke() );
        }
        break;

    case ProfileManagement::Pen_ModeToggle:
        buttonActionLabel->setText( cb->currentText() );
        m_buttonConfig.insert( senderName, QLatin1String( "modetoggle" ) );
        break;

    case ProfileManagement::Pen_DisplayToggle:
        buttonActionLabel->setText( cb->currentText() );
        m_buttonConfig.insert( senderName, QLatin1String( "displaytoggle" ) );
        break;

    case ProfileManagement::Pen_Disable:
        buttonActionLabel->setText( QString() );
        m_buttonConfig.insert( senderName, QString() );
        break;
    }

    emit changed();

    delete skb;
    delete sks;
}

void PenWidget::changeEraserPressCurve()
{
    QPointer <PressCurveDialog> selectPC = new PressCurveDialog( this );

    QDBusInterface *deviceInterface = new QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ) );

    QDBusReply<QString>  deviceName;
    if( deviceInterface->isValid() ) {
        deviceName = deviceInterface->call( QLatin1String( "eraserName" ) );

        if( deviceName.isValid() ) {
            selectPC->setDeviceHandler( deviceInterface, deviceName );
        }
    }

    selectPC->setControllPoints( m_ui->eraserPressureButton->property( "curve" ).toString() );

    int ret = selectPC->exec();

    if( ret == KDialog::Accepted ) {
        m_ui->eraserPressureButton->setProperty( "curve", selectPC->getControllPoints() );

        emit changed();
    }
    else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        deviceInterface->call( QLatin1String( "setConfiguration" ), QString( deviceName ), QLatin1String( "PressCurve" ), m_ui->eraserPressureButton->property( "curve" ).toString() );
    }
    delete selectPC;
    delete deviceInterface;
}

void PenWidget::changeTipPressCurve()
{
    QPointer <PressCurveDialog> selectPC = new PressCurveDialog( this );

    QDBusInterface *deviceInterface = new QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ) );

    QDBusReply<QString>  deviceName;
    if( deviceInterface->isValid() ) {
        deviceName = deviceInterface->call( QLatin1String( "stylusName" ) );

        if( deviceName.isValid() ) {
            selectPC->setDeviceHandler( deviceInterface, deviceName );
        }
    }

    selectPC->setControllPoints( m_ui->tipPressureButton->property( "curve" ).toString() );

    int ret = selectPC->exec();

    if( ret == KDialog::Accepted ) {
        m_ui->tipPressureButton->setProperty( "curve", selectPC->getControllPoints() );

        emit changed();
    }
    else {
        // reset the current pressurecurve to what is specified in the profile
        // rather than stick to the curve the user declined in the dialogue
        deviceInterface->call( QLatin1String( "setConfiguration" ), QString( deviceName ), QLatin1String( "PressCurve" ), m_ui->tipPressureButton->property( "curve" ).toString() );
    }
    delete selectPC;
    delete deviceInterface;
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
