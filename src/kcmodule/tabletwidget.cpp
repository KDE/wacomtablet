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

#include "tabletwidget.h"
#include "ui_tabletwidget.h"
#include "ui_saveprofile.h"
#include "ui_errorwidget.h"

#include "profilemanagement.h"
#include "generalwidget.h"
#include "padbuttonwidget.h"
#include "penwidget.h"
#include "tabletpagewidget.h"
#include "touchpagewidget.h"

// common
#include "dbustabletinterface.h"
#include "devicetype.h"

// stdlib
#include <memory>

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
class TabletWidgetPrivate {
    public:
        Ui::TabletWidget ui;                 //!< This user interface.

        GeneralWidget    generalPage;        //!< Widget that shows some basic information about the tablet.
        PenWidget        stylusPage;         //!< Widget for the pen settings (stylus/eraser).
        PadButtonWidget  buttonPage;         //!< Widget for the express button settings.
        TabletPageWidget tabletPage;         //!< Widget for the tablet settings.
        TouchPageWidget  touchPage;          //!< Widget for the touch settings.
        QWidget          deviceErrorWidget;  //!< Device error widget.
        Ui::ErrorWidget  deviceErrorUi;      //!< Device error widget ui.
        bool             profileChanged;     //!< True if the profile was changed and not saved yet.
}; // CLASS
}  // NAMESPACE



TabletWidget::TabletWidget( QWidget *parent )
    : QWidget( parent ), d_ptr(new TabletWidgetPrivate)
{
    setupUi();
    loadTabletInformation();
}

TabletWidget::~TabletWidget()
{
    delete this->d_ptr;
}



void TabletWidget::setupUi()
{
    Q_D( TabletWidget );

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

    // connect DBus signals
    connect( dbusTabletInterface, SIGNAL(tabletAdded()),   SLOT(loadTabletInformation()) );
    connect( dbusTabletInterface, SIGNAL(tabletRemoved()), SLOT(loadTabletInformation()) );
}


void TabletWidget::loadTabletInformation()
{
    QDBusReply<bool> isAvailable = DBusTabletInterface::instance().isAvailable();

    if( !isAvailable.isValid() ) {
        QString errorTitle = i18n( "KDE tablet service not found" );
        QString errorMsg   = i18n( "Please start the Wacom tablet service and try again.\n"
                                   "The daemon is required for tablet detection and profile support." );
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


void TabletWidget::addProfile()
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


void TabletWidget::delProfile()
{
    Q_D( TabletWidget );

    ProfileManagement::instance().deleteProfile();
    refreshProfileSelector();
    switchProfile( d->ui.profileSelector->currentText() );
}


void TabletWidget::saveProfile()
{
    Q_D( TabletWidget );

    d->generalPage.saveToProfile();
    d->stylusPage.saveToProfile();
    d->buttonPage.saveToProfile();
    d->tabletPage.saveToProfile();
    d->touchPage.saveToProfile();

    d->profileChanged = false;
    emit changed( false );

    applyProfile();
}


void TabletWidget::switchProfile( const QString &profile )
{
    showSaveChanges();

    ProfileManagement::instance().setProfileName( profile );

    reloadProfile();
    applyProfile();
}


void TabletWidget::reloadProfile()
{
    Q_D( TabletWidget );

    d->generalPage.loadFromProfile();
    d->stylusPage.loadFromProfile();
    d->buttonPage.loadFromProfile();
    d->tabletPage.loadFromProfile();
    d->touchPage.loadFromProfile();

    d->profileChanged = false;
    emit changed( false );
}


void TabletWidget::applyProfile()
{
    DBusTabletInterface::instance().setProfile( ProfileManagement::instance().profileName() );
}


void TabletWidget::profileChanged()
{
    Q_D( TabletWidget );

    d->profileChanged = true;
    emit changed( true );
}


void TabletWidget::showError( const QString& errorTitle, const QString &errorMsg )
{
    Q_D( TabletWidget );

    hideError();
    hideConfig();

    d->deviceErrorUi.errorTitle->setText(errorTitle);
    d->deviceErrorUi.errorText->setText (errorMsg);
    d->ui.verticalLayout->addWidget (&(d->deviceErrorWidget));
    d->deviceErrorWidget.setVisible(true);
}


void TabletWidget::hideConfig()
{
    Q_D( TabletWidget );

    d->ui.profileSelector->setEnabled( false );
    d->ui.addProfileButton->setEnabled( false );
    d->ui.delProfileButton->setEnabled( false );
    d->ui.deviceTabWidget->setVisible( false );
}


void TabletWidget::hideError()
{
    Q_D( TabletWidget );

    d->deviceErrorWidget.setVisible(false);
    d->ui.verticalLayout->removeWidget (&(d->deviceErrorWidget));
}


bool TabletWidget::refreshProfileSelector ( const QString& profile )
{
    Q_D( TabletWidget );

    int         index    = -1;
    QStringList profiles = ProfileManagement::instance().availableProfiles();

    d->ui.profileSelector->blockSignals( true );
    d->ui.profileSelector->clear();
    d->ui.profileSelector->addItems( profiles );

    if (!profile.isEmpty()) {
        index = d->ui.profileSelector->findText( profile );
        d->ui.profileSelector->setCurrentIndex( index );
    }

    d->ui.profileSelector->blockSignals( false );

    return (index >= 0);
}


void TabletWidget::showConfig()
{
    Q_D( TabletWidget );

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
        ProfileManagement::instance().createNewProfile();
        ProfileManagement::instance().setProfileName( QLatin1String( "Default" ) );
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


void TabletWidget::showSaveChanges()
{
    Q_D( TabletWidget );

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
