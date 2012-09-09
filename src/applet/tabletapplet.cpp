/*
 * Copyright 2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "property.h"
#include "tabletapplet.h"
#include "wacomtabletsettings.h"

// common includes
#include "dbustabletinterface.h"

//KDE inculdes
#include <KDE/KIconLoader>
#include <KDE/KSharedConfig>
#include <KDE/KComboBox>
#include <KDebug>

//Plasma includes
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <Plasma/ComboBox>
#include <Plasma/GroupBox>
#include <Plasma/PushButton>
#include <Plasma/RadioButton>
#include <Plasma/Separator>

//Qt includes
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QtDBus>

using namespace Wacom;

/*
QDBusArgument &operator<<( QDBusArgument &argument, const Wacom::TabletInformation &mystruct )
{
    argument.beginStructure();
    argument << mystruct.companyID << mystruct.deviceID << mystruct.companyName << mystruct.deviceName << mystruct.deviceModel << mystruct.deviceList << mystruct.padName << mystruct.stylusName << mystruct.eraserName << mystruct.cursorName << mystruct.touchName << mystruct.isDeviceAvailable << mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>( const QDBusArgument &argument, Wacom::TabletInformation &mystruct )
{
    argument.beginStructure();
    argument >> mystruct.companyID >> mystruct.deviceID >> mystruct.companyName
             >> mystruct.deviceName >> mystruct.deviceModel >> mystruct.deviceList
             >> mystruct.padName >> mystruct.stylusName >> mystruct.eraserName
             >> mystruct.cursorName >> mystruct.touchName >> mystruct.isDeviceAvailable >> mystruct.hasPadButtons;
    argument.endStructure();
    return argument;
}
*/

TabletApplet::TabletApplet( WacomTabletSettings *tabletSettings ) :
    m_tabletSettings( tabletSettings ),
    m_widget( 0 ),
    m_deviceName( 0 )
{
    buildDialog();

    qDBusRegisterMetaType<Wacom::TabletInformation>();
    qDBusRegisterMetaType< QList<Wacom::TabletInformation> >();
}

TabletApplet::~TabletApplet()
{
    delete m_deviceName;
    delete m_errorMsg;
    delete m_comboBoxProfile;
    delete m_radioButtonAbsolute;
    delete m_radioButtonRelative;
    delete m_radioButtonTouchOn;
    delete m_radioButtonTouchOff;
    delete m_layoutMain;
    delete m_errorWidget;
    delete m_configWidget;
    //delete m_widget;

    delete m_tabletSettings;
}

QGraphicsWidget *TabletApplet::dialog()
{
    return m_widget;
}

void TabletApplet::onDBusConnected()
{
    DBusTabletInterface::resetInterface();
    DBusTabletInterface::resetInterface();

    if( !DBusTabletInterface::instance().isValid() || !DBusTabletInterface::instance().isValid() ) {
        onDBusDisconnected();
        return;
    }

    connect( &DBusTabletInterface::instance(), SIGNAL( tabletAdded() ), this, SLOT( onTabletAdded() ) );
    connect( &DBusTabletInterface::instance(), SIGNAL( tabletRemoved() ), this, SLOT( onTabletRemoved() ) );
    connect( &DBusTabletInterface::instance(), SIGNAL( profileChanged( const QString ) ), this, SLOT( setProfile( const QString ) ) );

    QDBusReply<bool> isAvailable = DBusTabletInterface::instance().isAvailable();

    if( isAvailable ) {
        onTabletAdded();
    } else {
        onTabletRemoved();
    }
}

void TabletApplet::onDBusDisconnected()
{
    showError( i18n( "D-Bus connection to the kded daemon not available.\n\n"
                     "Please start the Wacom tablet daemon.\n"
                     "The daemon is responsible for tablet detection and profile support." ) );
}

void TabletApplet::updateWidget()
{
    QDBusReply<Wacom::TabletInformation> tabletInfo = DBusTabletInterface::instance().getInformation();

    if( tabletInfo.isValid() ) {
        m_deviceName->setText( tabletInfo.value().tabletName );
        m_padName = tabletInfo.value().padName;
        m_stylusName = tabletInfo.value().stylusName;
        m_eraserName = tabletInfo.value().eraserName;
        m_touchName = tabletInfo.value().touchName;
    }

    updateProfile();
}

void TabletApplet::updateProfile()
{
    //get list of all profiles
    QDBusReply<QStringList> profileList = DBusTabletInterface::instance().listProfiles();

    //fill comboBox
    m_comboBoxProfile->blockSignals( true );
    KComboBox *nativeBox = m_comboBoxProfile->nativeWidget();
    nativeBox->clear();
    nativeBox->addItems( profileList );

    //set current profile
    QDBusReply<QString> profileName = DBusTabletInterface::instance().getProfile();

    int index = nativeBox->findText( profileName );
    nativeBox->setCurrentIndex( index );
    m_comboBoxProfile->blockSignals( false );

    QDBusReply<QString> stylusMode = DBusTabletInterface::instance().getProperty(m_stylusName, Property::Mode);
    
    if( stylusMode.isValid() ) {
        if( QString( stylusMode ).contains( QLatin1String( "absolute" )) || QString( stylusMode ).contains( QLatin1String( "Absolute" )) ) {
            m_radioButtonRelative->setChecked( false );
            m_radioButtonAbsolute->setChecked( true );
        }
        else {
            m_radioButtonRelative->setChecked( true );
            m_radioButtonAbsolute->setChecked( false );
        }
    }

    if( m_touchName.isEmpty() ) {
        m_radioButtonTouchOn->setEnabled(false);
        m_radioButtonTouchOff->setEnabled(false);
    }
    else {
        m_radioButtonTouchOn->setEnabled(true);
        m_radioButtonTouchOff->setEnabled(true);

        QDBusReply<QString> touchMode = DBusTabletInterface::instance().getProperty(m_touchName, Property::Touch);
        
        if( touchMode.isValid() ) {
            if( QString( touchMode ).contains( QLatin1String( "on" ) ) ) {
                m_radioButtonTouchOff->setChecked( false );
                m_radioButtonTouchOn->setChecked( true );
            }
            else {
                m_radioButtonTouchOff->setChecked( true );
                m_radioButtonTouchOn->setChecked( false );
            }
        }
    }
}

void TabletApplet::setProfile( const QString &name )
{
    updateProfile();

    m_comboBoxProfile->blockSignals( true );
    KComboBox *nativeBox = m_comboBoxProfile->nativeWidget();
    int index = nativeBox->findText( name );
    nativeBox->setCurrentIndex( index );
    m_comboBoxProfile->blockSignals( false );
}

void TabletApplet::switchProfile( const QString &name )
{
    DBusTabletInterface::instance().setProfile(name);
}

void TabletApplet::rotateNorm()
{
    DBusTabletInterface::instance().setProperty(m_padName, Property::Rotate, QLatin1String("NONE"));
}

void TabletApplet::rotateCw()
{
    DBusTabletInterface::instance().setProperty(m_padName, Property::Rotate, QLatin1String("CW"));
}

void TabletApplet::rotateCcw()
{
    DBusTabletInterface::instance().setProperty(m_padName, Property::Rotate, QLatin1String("CCW"));
}

void TabletApplet::rotateHalf()
{
    DBusTabletInterface::instance().setProperty(m_padName, Property::Rotate, QLatin1String("HALF"));
}

void TabletApplet::selectAbsoluteMode( bool state )
{
    if( state ) {
        m_radioButtonRelative->setChecked( false );
        DBusTabletInterface::instance().setProperty(m_stylusName, Property::Mode, QLatin1String( "absolute" ));
        DBusTabletInterface::instance().setProperty(m_eraserName, Property::Mode, QLatin1String( "absolute" ));
    }
}

void TabletApplet::selectRelativeMode( bool state )
{
    if( state ) {
        m_radioButtonAbsolute->setChecked( false );
        DBusTabletInterface::instance().setProperty(m_stylusName, Property::Mode, QLatin1String( "relative" ));
        DBusTabletInterface::instance().setProperty(m_eraserName, Property::Mode, QLatin1String( "relative" ));
    }
}

void TabletApplet::setTouchModeOn( bool state )
{
    if( state ) {
        m_radioButtonTouchOn->setChecked( true );
        m_radioButtonTouchOff->setChecked( false );
        DBusTabletInterface::instance().setProperty(m_touchName, Property::Touch, QLatin1String( "on" ));
    }
}

void TabletApplet::setTouchModeOff( bool state )
{
    if( state ) {
        m_radioButtonTouchOn->setChecked( false );
        m_radioButtonTouchOff->setChecked( true );
        DBusTabletInterface::instance().setProperty(m_touchName, Property::Touch, QLatin1String( "off" ));
    }
}

void TabletApplet::buildDialog()
{
    //main widget
    m_widget = new QGraphicsWidget( m_tabletSettings );
    m_widget->setFocusPolicy( Qt::ClickFocus );
    m_widget->setMinimumSize( 350, 200 );
    m_widget->setPreferredSize( 350, 200 );

    m_layoutMain = new QGraphicsLinearLayout( Qt::Vertical, m_widget );
    m_layoutMain->setSpacing( 5 );
    m_layoutMain->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //create title block
    QGraphicsLinearLayout *layout_title = new QGraphicsLinearLayout;
    layout_title->setSpacing( 5 );
    layout_title->setOrientation( Qt::Horizontal );

    Plasma::IconWidget *icon = new Plasma::IconWidget( m_widget );
    icon->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    icon->setMaximumHeight( KIconLoader::SizeMedium );
    icon->setMinimumHeight( KIconLoader::SizeMedium );
    icon->setAcceptHoverEvents( false );
    m_deviceName = new Plasma::Label( m_widget );
    m_deviceName->setMaximumHeight( KIconLoader::SizeMedium );
    m_deviceName->nativeWidget()->setWordWrap( false );
    m_deviceName->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    layout_title->addItem( icon );
    layout_title->addItem( m_deviceName );
    layout_title->addStretch();

    m_layoutMain->addItem( layout_title );

    Plasma::Separator *separator = new Plasma::Separator();
    separator->setOrientation( Qt::Horizontal );
    separator->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    m_layoutMain->addItem( separator );

    //m_widget->setMinimumSize(300, 180);

    //build  the info dialog
    buildConfigDialog();

    //build the error dialog
    buildErrorDialog();
}

void TabletApplet::buildConfigDialog()
{
    //container for the settings when a tablet is available
    m_configWidget = new QGraphicsWidget();
    m_configWidget->setFocusPolicy( Qt::ClickFocus );
    m_configWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    QGraphicsLinearLayout *layout_tabletWidget = new QGraphicsLinearLayout( Qt::Vertical, m_configWidget );
    layout_tabletWidget->setSpacing( 0 );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // create profile box
    QGraphicsLinearLayout *layout_profile = new QGraphicsLinearLayout;
    layout_profile->setSpacing( 0 );
    layout_profile->setOrientation( Qt::Horizontal );

    Plasma::Label *label_profile = new Plasma::Label( m_configWidget );
    label_profile->setMaximumHeight( KIconLoader::SizeMedium );
    label_profile->nativeWidget()->setWordWrap( false );
    label_profile->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    label_profile->setText( i18n( "Select Profile:" ) );

    m_comboBoxProfile = new Plasma::ComboBox( m_configWidget );
    connect( m_comboBoxProfile, SIGNAL( textChanged( const QString ) ), this, SLOT( switchProfile( const QString ) ) );

    QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    sizePolicy.setHeightForWidth( m_comboBoxProfile->sizePolicy().hasHeightForWidth() );
    m_comboBoxProfile->setSizePolicy( sizePolicy );

    layout_profile->addItem( label_profile );
    layout_profile->addItem( m_comboBoxProfile );
    layout_tabletWidget->addItem( layout_profile );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // create settings groupbox
    Plasma::GroupBox *groupBox = new Plasma::GroupBox( m_configWidget );
    groupBox->setText( i18nc( "Groupbox Settings for the applet to change some values on the fly", "Settings" ) );
    groupBox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    QGraphicsLinearLayout *layout_groupbox = new QGraphicsLinearLayout( Qt::Vertical, groupBox );
    layout_groupbox->setSpacing( 10 );
    layout_groupbox->setContentsMargins(10,20,10,10);

    //add touch options
    QGraphicsLinearLayout *layout_touch = new QGraphicsLinearLayout;
    layout_touch->setSpacing( 0 );
    layout_touch->setOrientation( Qt::Horizontal );

    Plasma::Label *label_touch = new Plasma::Label( groupBox );
    label_touch->nativeWidget()->setWordWrap( false );
    label_touch->setText( i18nc( "Toggle between touch on/off", "Touch:" ) );
    layout_touch->addItem( label_touch );
    layout_touch->addStretch();

    m_radioButtonTouchOn = new Plasma::RadioButton( groupBox );
    m_radioButtonTouchOn->setText( i18nc( "Touch tool enabled", "On" ) );
    m_radioButtonTouchOn->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    m_radioButtonTouchOn->nativeWidget()->setAutoExclusive( false );
    m_radioButtonTouchOn->setZValue( 10 );
    connect( m_radioButtonTouchOn->nativeWidget(), SIGNAL( clicked( bool ) ), this, SLOT( setTouchModeOn( bool ) ) );
    layout_touch->addItem( m_radioButtonTouchOn );

    m_radioButtonTouchOff = new Plasma::RadioButton( groupBox );
    m_radioButtonTouchOff->setText( i18nc( "Touch tool disabled", "Off" ) );
    m_radioButtonTouchOff->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    m_radioButtonTouchOff->nativeWidget()->setAutoExclusive( false );
    m_radioButtonTouchOff->setZValue( 10 );
    connect( m_radioButtonTouchOff->nativeWidget(), SIGNAL( clicked( bool ) ), this, SLOT( setTouchModeOff( bool ) ) );
    layout_touch->addItem( m_radioButtonTouchOff );

    //layout_groupbox->addStretch( 20 ); //otherwise the title of the groupbox is not visible
    layout_groupbox->addItem( layout_touch );

    // create rotation buttons
    QGraphicsLinearLayout *layout_rotation = new QGraphicsLinearLayout;
    layout_rotation->setSpacing( 0 );
    layout_rotation->setOrientation( Qt::Horizontal );

    Plasma::Label *label_Rotation = new Plasma::Label( groupBox );
    label_Rotation->nativeWidget()->setWordWrap( false );
    label_Rotation->setText( i18nc( "Rotation of the tablet pad", "Rotation:" ) );
    layout_rotation->addItem( label_Rotation );

    Plasma::IconWidget *pushButton_norm = new Plasma::IconWidget( groupBox );
    pushButton_norm->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    pushButton_norm->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_norm->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_norm->setDrawBackground( false );
    connect( pushButton_norm, SIGNAL( clicked() ), this, SLOT( rotateNorm() ) );
    layout_rotation->addItem( pushButton_norm );

    Plasma::IconWidget *pushButton_cw = new Plasma::IconWidget( groupBox );
    pushButton_cw->setIcon( KIcon( QLatin1String( "object-rotate-left" ) ) );
    pushButton_cw->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_cw->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_cw->setDrawBackground( false );
    connect( pushButton_cw, SIGNAL( clicked() ), this, SLOT( rotateCw() ) );
    layout_rotation->addItem( pushButton_cw );

    Plasma::IconWidget *pushButton_ccw = new Plasma::IconWidget( groupBox );
    pushButton_ccw->setIcon( KIcon( QLatin1String( "object-rotate-right" ) ) );
    pushButton_ccw->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_ccw->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_ccw->setDrawBackground( false );
    connect( pushButton_ccw, SIGNAL( clicked() ), this, SLOT( rotateCcw() ) );
    layout_rotation->addItem( pushButton_ccw );

    Plasma::IconWidget *pushButton_half = new Plasma::IconWidget( groupBox );
    pushButton_half->setIcon( KIcon( QLatin1String( "object-flip-vertical" ) ) );
    pushButton_half->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_half->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_half->setDrawBackground( false );
    connect( pushButton_half, SIGNAL( clicked() ), this, SLOT( rotateHalf() ) );
    layout_rotation->addItem( pushButton_half );

    //layout_groupbox->addStretch( 20 );
    layout_groupbox->addItem( layout_rotation );

    // create mode selection
    QGraphicsLinearLayout *layout_mode = new QGraphicsLinearLayout;
    layout_mode->setSpacing( 0 );
    layout_mode->setOrientation( Qt::Horizontal );

    Plasma::Label *label_mode = new Plasma::Label( groupBox );
    label_mode->nativeWidget()->setWordWrap( false );
    label_mode->setText( i18nc( "Toggle between absolute/relative penmode", "Mode:" ) );
    layout_mode->addItem( label_mode );
    layout_mode->addStretch();

    m_radioButtonAbsolute = new Plasma::RadioButton( groupBox );
    m_radioButtonAbsolute->setText( i18nc( "absolute pen movement (pen mode)", "Absolute" ) );
    m_radioButtonAbsolute->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    m_radioButtonAbsolute->nativeWidget()->setAutoExclusive( false );
    m_radioButtonAbsolute->setZValue( 10 );
    connect( m_radioButtonAbsolute->nativeWidget(), SIGNAL( clicked( bool ) ), this, SLOT( selectAbsoluteMode( bool ) ) );
    layout_mode->addItem( m_radioButtonAbsolute );

    m_radioButtonRelative = new Plasma::RadioButton( groupBox );
    m_radioButtonRelative->setText( i18nc( "relative pen movement (mouse mode)", "Relative" ) );
    m_radioButtonRelative->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    m_radioButtonRelative->nativeWidget()->setAutoExclusive( false );
    m_radioButtonRelative->setZValue( 10 );
    connect( m_radioButtonRelative->nativeWidget(), SIGNAL( clicked( bool ) ), this, SLOT( selectRelativeMode( bool ) ) );
    layout_mode->addItem( m_radioButtonRelative );

    layout_groupbox->addItem( layout_mode );
    //layout_groupbox->addStretch( 20 );

    layout_tabletWidget->addItem( groupBox );
}

void TabletApplet::buildErrorDialog()
{
    //container for the settings when the tablet is removed or an error occurred
    m_errorWidget = new QGraphicsWidget();
    m_errorWidget->setFocusPolicy( Qt::ClickFocus );

    QGraphicsLinearLayout *layout_error = new QGraphicsLinearLayout( Qt::Horizontal, m_errorWidget );
    layout_error->setSpacing( 10 );
    layout_error->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    Plasma::IconWidget *errIcon = new Plasma::IconWidget( m_errorWidget );
    errIcon->setIcon( KIcon( QLatin1String( "dialog-warning" ) ) );
    errIcon->setMaximumHeight( KIconLoader::SizeMedium );
    errIcon->setMinimumHeight( KIconLoader::SizeMedium );
    errIcon->setAcceptHoverEvents( false );
    m_errorMsg = new Plasma::Label( m_errorWidget );
    m_errorMsg->nativeWidget()->setWordWrap( true );
    m_errorMsg->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    layout_error->addItem( errIcon );
    layout_error->addItem( m_errorMsg );
}

void TabletApplet::showError( const QString &msg )
{
    m_deviceName->setText( i18nc( "Title of the applet when an error shows up", "Tablet Error" ) );
    m_errorMsg->setText( msg );

    m_configWidget->hide();
    m_layoutMain->removeItem( m_configWidget );
    m_errorWidget->show();
    m_layoutMain->addItem( m_errorWidget );
}

void TabletApplet::showApplet()
{
    m_errorWidget->hide();
    m_layoutMain->removeItem( m_errorWidget );
    m_configWidget->show();
    m_layoutMain->addItem( m_configWidget );

    updateWidget();
}

void TabletApplet::onTabletAdded()
{
    showApplet();
}

void TabletApplet::onTabletRemoved()
{
    showError( i18n( "No tablet device was found.\n\nPlease connect the device." ) );
}
