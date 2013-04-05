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

#include "tabletapplet.h"
#include "property.h"
#include "wacomtabletsettings.h"

// common includes
#include "dbustabletinterface.h"
#include "devicetype.h"
#include "screenrotation.h"

//KDE inculdes
#include <KDE/KIconLoader>
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
#include <QtCore/QStringList>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>


namespace Wacom
{
    class TabletAppletPrivate
    {
        public:
            ~TabletAppletPrivate() {
                delete configWidget;
                delete errorWidget;
                delete dialogWidget;
            }

            bool                            hasTouch;
            QPointer<QGraphicsWidget>       dialogWidget;      //!< The graphics widget which displays the content.
            QPointer<QGraphicsWidget>       configWidget;      //!< Widget for the config content created by buildConfigDialog()
            QPointer<QGraphicsWidget>       errorWidget;       //!< Widget for the error content created by buildErrorDialog()

            // these do not need to be deleted as they are properly parented or are not managed by us
            QPointer<WacomTabletSettings>   tabletSettings;    //!< A backreference to the tablet popup applet containing this widget - do not delete this.
            QGraphicsLinearLayout*          layoutMain;        //!< Layout of the main widget that contains the title / config and error widgets
            Plasma::Label*                  deviceName;        //!< The name of the tablet.
            Plasma::Label*                  errorMsg;          //!< The error message if no tablet/kded available.
            Plasma::ComboBox*               profileSelector;   //!< The Combox for the profile selection.
            Plasma::RadioButton*            modeAbsolute;      //!< The Radiobutton to select the pen absolute mode
            Plasma::RadioButton*            modeRelative;      //!< The Radiobutton to select the pen absolute mode
            Plasma::RadioButton*            touchOn;           //!< The Radiobutton to toggle Touch on
            Plasma::RadioButton*            touchOff;          //!< The Radiobutton to toggle Touch off
    };
}


using namespace Wacom;

TabletApplet::TabletApplet (WacomTabletSettings *tabletSettings) :
    d_ptr (new TabletAppletPrivate)
{
    Q_D (TabletApplet);

    d->tabletSettings = tabletSettings;

    buildDialog();
}

TabletApplet::~TabletApplet()
{
    delete this->d_ptr;
}

QGraphicsWidget *TabletApplet::dialog()
{
    Q_D (TabletApplet);
    return d->dialogWidget;
}

void TabletApplet::onDBusConnected()
{
    DBusTabletInterface::resetInterface();

    if( !DBusTabletInterface::instance().isValid() ) {
        onDBusDisconnected();
        return;
    }

    connect( &DBusTabletInterface::instance(), SIGNAL (tabletAdded()),                  this, SLOT (onTabletAdded()) );
    connect( &DBusTabletInterface::instance(), SIGNAL (tabletRemoved()),                this, SLOT (onTabletRemoved()) );
    connect( &DBusTabletInterface::instance(), SIGNAL (profileChanged(QString)), this, SLOT (setProfile(QString)) );

    QDBusReply<bool> isAvailable = DBusTabletInterface::instance().isAvailable();

    if( isAvailable ) {
        onTabletAdded();
    } else {
        onTabletRemoved();
    }
}

void TabletApplet::onDBusDisconnected()
{
    QString errorTitle = i18n("Error - Tablet service not available.");
    QString errorMsg = i18n("Please start the KDE wacom tablet service.\n"
                            "The service is required for tablet detection and profile support.");
    showError( errorTitle, errorMsg );
}

void TabletApplet::updateWidget()
{
    Q_D (TabletApplet);

    QDBusReply<QString> dbusReply;

    dbusReply = DBusTabletInterface::instance().getInformation(TabletInfo::TabletName);

    if (dbusReply.isValid()) {
        d->deviceName->setText(dbusReply.value());
    }

    dbusReply = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);
    d->hasTouch = (dbusReply.isValid() && !dbusReply.value().isEmpty());

    updateProfile();
}

void TabletApplet::updateProfile()
{
    Q_D (TabletApplet);

    //get list of all profiles
    QDBusReply<QStringList> profileList = DBusTabletInterface::instance().listProfiles();

    //fill comboBox
    d->profileSelector->blockSignals( true );
    KComboBox *nativeBox = d->profileSelector->nativeWidget();
    nativeBox->clear();
    nativeBox->addItems( profileList );

    //set current profile
    QDBusReply<QString> profileName = DBusTabletInterface::instance().getProfile();

    int index = nativeBox->findText( profileName );
    nativeBox->setCurrentIndex( index );
    d->profileSelector->blockSignals( false );

    QDBusReply<QString> stylusMode = DBusTabletInterface::instance().getProperty(DeviceType::Stylus, Property::Mode);

    if( stylusMode.isValid() ) {
        if( QString( stylusMode ).contains( QLatin1String( "absolute" )) || QString( stylusMode ).contains( QLatin1String( "Absolute" )) ) {
            d->modeRelative->setChecked( false );
            d->modeAbsolute->setChecked( true );

        } else {
            d->modeRelative->setChecked( true );
            d->modeAbsolute->setChecked( false );
        }
    }

    if (!d->hasTouch) {
        d->touchOn->setEnabled(false);
        d->touchOff->setEnabled(false);

    } else {
        d->touchOn->setEnabled(true);
        d->touchOff->setEnabled(true);

        QDBusReply<QString> touchMode = DBusTabletInterface::instance().getProperty(DeviceType::Touch, Property::Touch);

        if( touchMode.isValid() ) {
            if( QString( touchMode ).contains( QLatin1String( "on" ) ) ) {
                d->touchOff->setChecked( false );
                d->touchOn->setChecked( true );
            }
            else {
                d->touchOff->setChecked( true );
                d->touchOn->setChecked( false );
            }
        }
    }
}

void TabletApplet::setProfile( const QString &name )
{
    Q_D (TabletApplet);

    updateProfile();

    d->profileSelector->blockSignals( true );
    KComboBox *nativeBox = d->profileSelector->nativeWidget();
    int index = nativeBox->findText( name );
    nativeBox->setCurrentIndex( index );
    d->profileSelector->blockSignals( false );
}

void TabletApplet::switchProfile( const QString &name )
{
    DBusTabletInterface::instance().setProfile(name);
}

void TabletApplet::rotateNorm()
{
    DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Rotate, ScreenRotation::NONE.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Rotate, ScreenRotation::NONE.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Rotate, ScreenRotation::NONE.key());
}

void TabletApplet::rotateCw()
{
    DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Rotate, ScreenRotation::CW.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Rotate, ScreenRotation::CW.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Rotate, ScreenRotation::CW.key());
}

void TabletApplet::rotateCcw()
{
    DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Rotate, ScreenRotation::CCW.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Rotate, ScreenRotation::CCW.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Rotate, ScreenRotation::CCW.key());
}

void TabletApplet::rotateHalf()
{
    DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Rotate, ScreenRotation::HALF.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Rotate, ScreenRotation::HALF.key());
    DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Rotate, ScreenRotation::HALF.key());
}

void TabletApplet::selectAbsoluteMode( bool state )
{
    Q_D (TabletApplet);

    if( state ) {
        d->modeRelative->setChecked( false );
        DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Mode, QLatin1String( "absolute" ));
        DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Mode, QLatin1String( "absolute" ));
    }
}

void TabletApplet::selectRelativeMode( bool state )
{
    Q_D (TabletApplet);

    if( state ) {
        d->modeAbsolute->setChecked( false );
        DBusTabletInterface::instance().setProperty(DeviceType::Stylus, Property::Mode, QLatin1String( "relative" ));
        DBusTabletInterface::instance().setProperty(DeviceType::Eraser, Property::Mode, QLatin1String( "relative" ));
    }
}

void TabletApplet::setTouchModeOn( bool state )
{
    Q_D (TabletApplet);

    if( state ) {
        d->touchOn->setChecked( true );
        d->touchOff->setChecked( false );
        DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Touch, QLatin1String( "on" ));
    }
}

void TabletApplet::setTouchModeOff( bool state )
{
    Q_D (TabletApplet);

    if( state ) {
        d->touchOn->setChecked( false );
        d->touchOff->setChecked( true );
        DBusTabletInterface::instance().setProperty(DeviceType::Touch, Property::Touch, QLatin1String( "off" ));
    }
}

void TabletApplet::buildDialog()
{
    Q_D (TabletApplet);

    //main widget
    d->dialogWidget = new QGraphicsWidget (d->tabletSettings);
    d->dialogWidget->setFocusPolicy( Qt::ClickFocus );
    d->dialogWidget->setMinimumSize( 350, 200 );
    d->dialogWidget->setPreferredSize( 350, 200 );

    d->layoutMain = new QGraphicsLinearLayout (Qt::Vertical, d->dialogWidget);
    d->layoutMain->setSpacing( 5 );
    d->layoutMain->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //create title block
    QGraphicsLinearLayout *layout_title = new QGraphicsLinearLayout;
    layout_title->setSpacing( 5 );
    layout_title->setOrientation( Qt::Horizontal );

    Plasma::IconWidget *icon = new Plasma::IconWidget( d->dialogWidget );
    icon->setIcon( KIcon( QLatin1String( "input-tablet" ) ) );
    icon->setMaximumHeight( KIconLoader::SizeMedium );
    icon->setMinimumHeight( KIconLoader::SizeMedium );
    icon->setAcceptHoverEvents( false );

    d->deviceName = new Plasma::Label( d->dialogWidget );
    d->deviceName->setMaximumHeight( KIconLoader::SizeMedium );
    d->deviceName->nativeWidget()->setWordWrap( false );
    d->deviceName->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    layout_title->addItem( icon );
    layout_title->addItem( d->deviceName );
    layout_title->addStretch();

    d->layoutMain->addItem( layout_title );

    Plasma::Separator *separator = new Plasma::Separator();
    separator->setOrientation( Qt::Horizontal );
    separator->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    d->layoutMain->addItem( separator );

    //build  the info dialog
    buildConfigDialog();

    //build the error dialog
    buildErrorDialog();
}

void TabletApplet::buildConfigDialog()
{
    Q_D (TabletApplet);

    //container for the settings when a tablet is available
    d->configWidget = new QGraphicsWidget();
    d->configWidget->setFocusPolicy( Qt::ClickFocus );
    d->configWidget->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    QGraphicsLinearLayout *layout_tabletWidget = new QGraphicsLinearLayout( Qt::Vertical, d->configWidget );
    layout_tabletWidget->setSpacing( 0 );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // create profile box
    QGraphicsLinearLayout *layout_profile = new QGraphicsLinearLayout;
    layout_profile->setSpacing( 0 );
    layout_profile->setOrientation( Qt::Horizontal );

    Plasma::Label *label_profile = new Plasma::Label( d->configWidget );
    label_profile->setMaximumHeight( KIconLoader::SizeMedium );
    label_profile->nativeWidget()->setWordWrap( false );
    label_profile->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    label_profile->setText( i18n( "Select Profile:" ) );

    d->profileSelector = new Plasma::ComboBox( d->configWidget );
    connect( d->profileSelector, SIGNAL(textChanged(QString)), this, SLOT(switchProfile(QString)) );

    QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 0 );
    sizePolicy.setHeightForWidth( d->profileSelector->sizePolicy().hasHeightForWidth() );
    d->profileSelector->setSizePolicy( sizePolicy );

    layout_profile->addItem( label_profile );
    layout_profile->addItem( d->profileSelector );
    layout_tabletWidget->addItem( layout_profile );

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // create settings groupbox
    Plasma::GroupBox *groupBox = new Plasma::GroupBox( d->configWidget );
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

    d->touchOn = new Plasma::RadioButton( groupBox );
    d->touchOn->setText( i18nc( "Touch tool enabled", "On" ) );
    d->touchOn->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    d->touchOn->nativeWidget()->setAutoExclusive( false );
    d->touchOn->setZValue( 10 );
    connect( d->touchOn->nativeWidget(), SIGNAL(clicked(bool)), this, SLOT(setTouchModeOn(bool)) );
    layout_touch->addItem( d->touchOn );

    d->touchOff = new Plasma::RadioButton( groupBox );
    d->touchOff->setText( i18nc( "Touch tool disabled", "Off" ) );
    d->touchOff->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    d->touchOff->nativeWidget()->setAutoExclusive( false );
    d->touchOff->setZValue( 10 );
    connect( d->touchOff->nativeWidget(), SIGNAL(clicked(bool)), this, SLOT(setTouchModeOff(bool)) );
    layout_touch->addItem( d->touchOff );

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
    connect( pushButton_norm, SIGNAL(clicked()), this, SLOT(rotateNorm()) );
    layout_rotation->addItem( pushButton_norm );

    Plasma::IconWidget *pushButton_cw = new Plasma::IconWidget( groupBox );
    pushButton_cw->setIcon( KIcon( QLatin1String( "object-rotate-left" ) ) );
    pushButton_cw->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_cw->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_cw->setDrawBackground( false );
    connect( pushButton_cw, SIGNAL(clicked()), this, SLOT(rotateCw()) );
    layout_rotation->addItem( pushButton_cw );

    Plasma::IconWidget *pushButton_ccw = new Plasma::IconWidget( groupBox );
    pushButton_ccw->setIcon( KIcon( QLatin1String( "object-rotate-right" ) ) );
    pushButton_ccw->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_ccw->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_ccw->setDrawBackground( false );
    connect( pushButton_ccw, SIGNAL(clicked()), this, SLOT(rotateCcw()) );
    layout_rotation->addItem( pushButton_ccw );

    Plasma::IconWidget *pushButton_half = new Plasma::IconWidget( groupBox );
    pushButton_half->setIcon( KIcon( QLatin1String( "object-flip-vertical" ) ) );
    pushButton_half->setMaximumHeight( KIconLoader::SizeSmallMedium );
    pushButton_half->setMinimumHeight( KIconLoader::SizeSmallMedium );
    pushButton_half->setDrawBackground( false );
    connect( pushButton_half, SIGNAL(clicked()), this, SLOT(rotateHalf()) );
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

    d->modeAbsolute = new Plasma::RadioButton( groupBox );
    d->modeAbsolute->setText( i18nc( "absolute pen movement (pen mode)", "Absolute" ) );
    d->modeAbsolute->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    d->modeAbsolute->nativeWidget()->setAutoExclusive( false );
    d->modeAbsolute->setZValue( 10 );
    connect( d->modeAbsolute->nativeWidget(), SIGNAL(clicked(bool)), this, SLOT(selectAbsoluteMode(bool)) );
    layout_mode->addItem( d->modeAbsolute );

    d->modeRelative = new Plasma::RadioButton( groupBox );
    d->modeRelative->setText( i18nc( "relative pen movement (mouse mode)", "Relative" ) );
    d->modeRelative->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    d->modeRelative->nativeWidget()->setAutoExclusive( false );
    d->modeRelative->setZValue( 10 );
    connect( d->modeRelative->nativeWidget(), SIGNAL(clicked(bool)), this, SLOT(selectRelativeMode(bool)) );
    layout_mode->addItem( d->modeRelative );

    layout_groupbox->addItem( layout_mode );
    //layout_groupbox->addStretch( 20 );

    layout_tabletWidget->addItem( groupBox );
}

void TabletApplet::buildErrorDialog()
{
    Q_D (TabletApplet);

    //container for the settings when the tablet is removed or an error occurred
    d->errorWidget = new QGraphicsWidget();
    d->errorWidget->setFocusPolicy( Qt::ClickFocus );

    QGraphicsLinearLayout *layout_error = new QGraphicsLinearLayout( Qt::Horizontal, d->errorWidget );
    layout_error->setSpacing( 10 );
    layout_error->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    Plasma::IconWidget *errIcon = new Plasma::IconWidget( d->errorWidget );
    errIcon->setIcon( KIcon( QLatin1String( "dialog-warning" ) ) );
    errIcon->setMaximumHeight( KIconLoader::SizeMedium );
    errIcon->setMinimumHeight( KIconLoader::SizeMedium );
    errIcon->setAcceptHoverEvents( false );
    d->errorMsg = new Plasma::Label( d->errorWidget );
    d->errorMsg->nativeWidget()->setWordWrap( true );
    d->errorMsg->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

    layout_error->addItem( errIcon );
    layout_error->addItem( d->errorMsg );
}

void TabletApplet::showError(const QString& title, const QString& msg )
{
    Q_D (TabletApplet);

    d->deviceName->setText( title );
    d->errorMsg->setText( msg );

    d->configWidget->hide();
    d->layoutMain->removeItem( d->configWidget );
    d->errorWidget->show();
    d->layoutMain->addItem( d->errorWidget );
}

void TabletApplet::showApplet()
{
    Q_D (TabletApplet);

    d->errorWidget->hide();
    d->layoutMain->removeItem( d->errorWidget );
    d->configWidget->show();
    d->layoutMain->addItem( d->configWidget );

    updateWidget();
}

void TabletApplet::onTabletAdded()
{
    showApplet();
}

void TabletApplet::onTabletRemoved()
{
    QString errorTitle = i18n("Graphic Tablet - Device not detected.");
    QString errorMsg = i18n("This widget is inactive because your tablet device is not connected or currently not supported.");
    showError(errorTitle, errorMsg);
}
