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

#include "kcmwacomtabletwidget.h"
#include "ui_kcmwacomtabletwidget.h"
#include "ui_saveprofile.h"
#include "ui_errorwidget.h"

#include "profilemanagement.h"
#include "generalpagewidget.h"
#include "styluspagewidget.h"
#include "buttonpagewidget.h"
#include "tabletpagewidget.h"
#include "touchpagewidget.h"

// common
#include "dbustabletinterface.h"
#include "devicetype.h"

//KDE includes
#include <KDE/KInputDialog>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

//Qt includes
#include <QtDBus/QDBusReply>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtGui/QPixmap>
#include <QtGui/QLineEdit>

using namespace Wacom;

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class KCMWacomTabletWidgetPrivate {
    public:
        Ui::KCMWacomTabletWidget ui;                 //!< This user interface.

        GeneralPageWidget generalPage;        //!< Widget that shows some basic information about the tablet.
        StylusPageWidget  stylusPage;         //!< Widget for the pen settings (stylus/eraser).
        ButtonPageWidget  buttonPage;         //!< Widget for the express button settings.
        TabletPageWidget  tabletPage;         //!< Widget for the tablet settings.
        TouchPageWidget   touchPage;          //!< Widget for the touch settings.
        QWidget           deviceErrorWidget;  //!< Device error widget.
        Ui::ErrorWidget   deviceErrorUi;      //!< Device error widget ui.
        bool              profileChanged;     //!< True if the profile was changed and not saved yet.
}; // CLASS
}  // NAMESPACE



KCMWacomTabletWidget::KCMWacomTabletWidget( QWidget *parent )
    : QWidget( parent ), d_ptr(new KCMWacomTabletWidgetPrivate)
{
    setupUi();
    loadTabletInformation();
}

KCMWacomTabletWidget::~KCMWacomTabletWidget()
{
    delete this->d_ptr;
}



void KCMWacomTabletWidget::setupUi()
{
    Q_D( KCMWacomTabletWidget );

    DBusTabletInterface* dbusTabletInterface = &DBusTabletInterface::instance();

    if( !dbusTabletInterface->isValid() ) {
        kDebug() << "DBus interface not available";
    }

    d->profileChanged    = false;

    // setup error widget
    d->deviceErrorUi.setupUi(&(d->deviceErrorWidget));
    d->deviceErrorUi.errorImage->setPixmap( KIconLoader::global()->loadIcon( QLatin1String( "dialog-warning" ), KIconLoader::NoGroup, 48 ) );

    // setup normal ui
    d->ui.setupUi( this );
    d->ui.addProfileButton->setIcon( KIcon( QLatin1String( "document-new" ) ) );
    d->ui.delProfileButton->setIcon( KIcon( QLatin1String( "edit-delete-page" ) ) );

    // connect profile selector
    connect( d->ui.addProfileButton, SIGNAL(clicked(bool)), SLOT(addProfile()) );
    connect( d->ui.delProfileButton, SIGNAL(clicked(bool)), SLOT(delProfile()) );
    connect( d->ui.profileSelector,  SIGNAL(currentIndexChanged(QString)), SLOT(switchProfile(QString)) );

    // connect configuration tabs
    connect( &(d->generalPage), SIGNAL(changed()), SLOT(profileChanged()) );
    connect( &(d->stylusPage),  SIGNAL(changed()), SLOT(profileChanged()) );
    connect( &(d->buttonPage),  SIGNAL(changed()), SLOT(profileChanged()) );
    connect( &(d->tabletPage),  SIGNAL(changed()), SLOT(profileChanged()) );
    connect( &(d->touchPage),   SIGNAL(changed()), SLOT(profileChanged()) );

    // connect rotation handling
    connect( &(d->tabletPage),  SIGNAL(rotationChanged(ScreenRotation)), &(d->touchPage), SLOT(onRotationChanged(ScreenRotation)));

    // connect DBus signals
    connect( dbusTabletInterface, SIGNAL(tabletAdded()),   SLOT(loadTabletInformation()) );
    connect( dbusTabletInterface, SIGNAL(tabletRemoved()), SLOT(loadTabletInformation()) );
}


void KCMWacomTabletWidget::loadTabletInformation()
{
    QDBusReply<bool> isAvailable = DBusTabletInterface::instance().isAvailable();

    if( !isAvailable.isValid() ) {
        QString errorTitle = i18n( "KDE tablet service not found" );
        QString errorMsg   = i18n( "Please start the KDE wacom tablet service to use this configuration dialog.\n"
                                   "The service is required for tablet detection and profile support." );
        showError( errorTitle, errorMsg );

    } else if( !isAvailable ) {
        QString errorTitle = i18n( "No tablet device detected" );
        QString errorMsg   = i18n( "Please connect a tablet device to continue.\n"
                                   "If your device is already connected it is currently unsupported." );
        showError( errorTitle, errorMsg );

    } else {
        showConfig();
    }
}


void KCMWacomTabletWidget::addProfile()
{
    bool ok;
    QString text = KInputDialog::getText( i18n( "Add new profile" ),
                                          i18n( "Profile name:" ), QString(), &ok, this );
    if( !ok || text.isEmpty() ) {
        return;
    }

    ProfileManagement::instance().createNewProfile( text );
    refreshProfileSelector(text);
    switchProfile( text );
}


void KCMWacomTabletWidget::delProfile()
{
    Q_D( KCMWacomTabletWidget );

    ProfileManagement::instance().deleteProfile();
    refreshProfileSelector();
    switchProfile( d->ui.profileSelector->currentText() );

    //update profile rotation selection
    d->generalPage.reloadWidget();
}


void KCMWacomTabletWidget::saveProfile()
{
    Q_D( KCMWacomTabletWidget );

    d->generalPage.saveToProfile();
    d->stylusPage.saveToProfile();
    d->buttonPage.saveToProfile();
    d->tabletPage.saveToProfile();
    d->touchPage.saveToProfile();

    d->profileChanged = false;
    emit changed( false );

    applyProfile();
}


void KCMWacomTabletWidget::switchProfile( const QString &profile )
{
    showSaveChanges();

    ProfileManagement::instance().setProfileName( profile );

    reloadProfile();
    applyProfile();
}


void KCMWacomTabletWidget::reloadProfile()
{
    Q_D( KCMWacomTabletWidget );

    d->generalPage.loadFromProfile();
    d->stylusPage.loadFromProfile();
    d->buttonPage.loadFromProfile();
    d->tabletPage.loadFromProfile();
    d->touchPage.loadFromProfile();

    d->profileChanged = false;
    emit changed( false );
}


void KCMWacomTabletWidget::applyProfile()
{
    DBusTabletInterface::instance().setProfile( ProfileManagement::instance().profileName() );
}


void KCMWacomTabletWidget::profileChanged()
{
    Q_D( KCMWacomTabletWidget );

    d->profileChanged = true;
    emit changed( true );
}


void KCMWacomTabletWidget::showError( const QString& errorTitle, const QString &errorMsg )
{
    Q_D( KCMWacomTabletWidget );

    hideError();
    hideConfig();

    d->deviceErrorUi.errorTitle->setText(errorTitle);
    d->deviceErrorUi.errorText->setText (errorMsg);
    d->ui.verticalLayout->addWidget (&(d->deviceErrorWidget));
    d->deviceErrorWidget.setVisible(true);
}


void KCMWacomTabletWidget::hideConfig()
{
    Q_D( KCMWacomTabletWidget );

    d->ui.profileSelector->setEnabled( false );
    d->ui.addProfileButton->setEnabled( false );
    d->ui.delProfileButton->setEnabled( false );
    d->ui.deviceTabWidget->setVisible( false );
}


void KCMWacomTabletWidget::hideError()
{
    Q_D( KCMWacomTabletWidget );

    d->deviceErrorWidget.setVisible(false);
    d->ui.verticalLayout->removeWidget (&(d->deviceErrorWidget));
}


bool KCMWacomTabletWidget::refreshProfileSelector ( const QString& profile )
{
    Q_D( KCMWacomTabletWidget );

    int         index    = -1;
    QStringList profiles = ProfileManagement::instance().availableProfiles();

    d->ui.profileSelector->blockSignals( true );
    d->ui.profileSelector->clear();
    d->ui.profileSelector->addItems( profiles );

    if (!profile.isEmpty()) {
        index = d->ui.profileSelector->findText( profile );
        d->ui.profileSelector->setCurrentIndex( index );

    } else if (!profiles.isEmpty()) {
        index = 0;
        d->ui.profileSelector->setCurrentIndex( index );
    }

    d->ui.profileSelector->blockSignals( false );

    return (index >= 0);
}


void KCMWacomTabletWidget::showConfig()
{
    Q_D( KCMWacomTabletWidget );

    // make sure no error message is active
    hideError();

    // reload profile and widget data
    ProfileManagement::instance().reload();

    d->generalPage.reloadWidget();
    d->stylusPage.reloadWidget();
    d->buttonPage.reloadWidget();
    d->tabletPage.reloadWidget();
    d->touchPage.reloadWidget();


    // initialize profile selector
    d->ui.profileSelector->setEnabled( true );
    d->ui.addProfileButton->setEnabled( true );
    d->ui.delProfileButton->setEnabled( true );

    if( ProfileManagement::instance().availableProfiles().isEmpty() ) {
        ProfileManagement::instance().createNewProfile(i18nc( "Name of the default profile that will be created if none exist.","Default" ));
        applyProfile();
    }

    refreshProfileSelector();


    // initialize configuration tabs
    d->ui.deviceTabWidget->clear();
    d->ui.deviceTabWidget->addTab( &(d->generalPage), i18nc( "Basic overview page for the tablet hardware", "General" ) );
    d->ui.deviceTabWidget->addTab( &(d->stylusPage), i18n( "Stylus" ) );

    QDBusReply<bool> hasPadButtons = DBusTabletInterface::instance().hasPadButtons();

    if( hasPadButtons.isValid() && hasPadButtons.value() ) {
        d->ui.deviceTabWidget->addTab( &(d->buttonPage), i18n( "Express Buttons" ) );
    }

    d->ui.deviceTabWidget->addTab( &(d->tabletPage), i18n ("Tablet") );

    QDBusReply<QString> touchDeviceName = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);
    bool                hasTouchDevice  = (touchDeviceName.isValid() && !touchDeviceName.value().isEmpty());

    if (hasTouchDevice) {
        d->ui.deviceTabWidget->addTab( &(d->touchPage), i18n ("Touch") );
    }

    d->ui.deviceTabWidget->setEnabled( true );
    d->ui.deviceTabWidget->setVisible( true );


    // switch to the currently active profile
    QDBusReply<QString> profile = DBusTabletInterface::instance().getProfile();
    if( profile.isValid() ) {
        d->ui.profileSelector->setCurrentItem( profile );
        switchProfile( profile );
    }
}


void KCMWacomTabletWidget::showSaveChanges()
{
    Q_D( KCMWacomTabletWidget );

    if( d->profileChanged ) {
        QPointer<KDialog> saveDialog = new KDialog();
        QWidget*          widget     = new QWidget( this );

        Ui::SaveProfile askToSave;
        askToSave.setupUi( widget );

        saveDialog->setMainWidget( widget );
        saveDialog->setButtons( KDialog::Apply | KDialog::Cancel );

        connect( saveDialog, SIGNAL(applyClicked()), saveDialog, SLOT(accept()) );

        if( saveDialog->exec() == KDialog::Accepted ) {
            saveProfile();
        }

        delete saveDialog;
    }
}
