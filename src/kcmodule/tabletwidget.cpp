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

#include "tabletwidget.h"
#include "ui_tabletwidget.h"
#include "ui_saveprofile.h"
#include "ui_errorwidget.h"

#include "profilemanagement.h"
#include "generalwidget.h"
#include "padbuttonwidget.h"
#include "padmapping.h"
#include "penwidget.h"
#include "touchwidget.h"

//KDE includes
#include <KDE/KInputDialog>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

//Qt includes
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtGui/QPixmap>
#include <QtGui/QLineEdit>

using namespace Wacom;

TabletWidget::TabletWidget( QWidget *parent )
    : QWidget( parent ),
      m_ui( new Ui::TabletWidget ),
      m_profileChanged( false )
{
    init();
    loadTabletInformation();
}

TabletWidget::~TabletWidget()
{
    delete m_ui;
    delete m_tabletInterface;
    delete m_deviceInterface;

    delete m_profileManagement;
    delete m_generalPage;
    delete m_padButtonPage;
    delete m_padMappingPage;
    delete m_touchMappingPage;
    delete m_penPage;
    delete m_touchPage;
}

void TabletWidget::init()
{
    m_tabletInterface = new QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Tablet" ), QLatin1String( "org.kde.Wacom" ) );
    m_deviceInterface = new QDBusInterface( QLatin1String( "org.kde.Wacom" ), QLatin1String( "/Device" ), QLatin1String( "org.kde.WacomDevice" ) );

    if( !m_tabletInterface->isValid() || !m_deviceInterface->isValid() ) {
        kDebug() << "DBus interface not available";
    }

    m_profileManagement = new ProfileManagement( m_deviceInterface );

    m_generalPage = new GeneralWidget( m_deviceInterface, m_profileManagement );
    m_padButtonPage = new PadButtonWidget( m_profileManagement );
    m_padMappingPage = new PadMapping( m_deviceInterface, m_profileManagement );
    m_touchMappingPage = new PadMapping( m_deviceInterface, m_profileManagement );
    m_penPage = new PenWidget( m_profileManagement );
    m_touchPage = new TouchWidget( m_profileManagement );
    m_ui->setupUi( this );
    m_ui->addProfileButton->setIcon( KIcon( QLatin1String( "document-new" ) ) );
    m_ui->delProfileButton->setIcon( KIcon( QLatin1String( "edit-delete-page" ) ) );

    connect( m_ui->addProfileButton, SIGNAL( clicked( bool ) ), SLOT( addProfile() ) );
    connect( m_ui->delProfileButton, SIGNAL( clicked( bool ) ), SLOT( delProfile() ) );
    connect( m_ui->profileSelector, SIGNAL( currentIndexChanged( const QString ) ), SLOT( switchProfile( const QString ) ) );
    connect( m_padButtonPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( m_padMappingPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( m_touchMappingPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( m_penPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( m_touchPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( m_generalPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    //DBus signals
    connect( m_tabletInterface, SIGNAL( tabletAdded() ), SLOT( loadTabletInformation() ) );
    connect( m_tabletInterface, SIGNAL( tabletRemoved() ), SLOT( loadTabletInformation() ) );

    m_profilesConfig = KSharedConfig::openConfig( QLatin1String( "tabletprofilesrc" ), KConfig::SimpleConfig );
}

void TabletWidget::loadTabletInformation()
{
    //check if a tablet is connected
    QDBusReply<bool> isAvailable = m_tabletInterface->call( QLatin1String( "tabletAvailable" ) );

    if( !isAvailable.isValid() ) {
        QString errmsg = i18n( "D-Bus connection to the kded daemon not available.\n\nPlease start the Wacom tablet daemon and try again.\nThe daemon is responsible for tablet detection and profile support." );
        showError( errmsg );
        kError() << "DBus reply tabletAvailable failed";
        return;
    }

    if( !isAvailable ) {
        QString errmsg = i18n( "No tablet device was found.\n\nPlease connect the device before you start this module.\nIf the device is already connected refer to the help file for any further information." );
        showError( errmsg );
        return;
    }

    if( m_deviceError ) {
        m_deviceError->setVisible( false );
        m_ui->verticalLayout->removeWidget( m_deviceError );
    }

    m_profileManagement->reload();
    m_generalPage->reloadWidget();
    m_padButtonPage->reloadWidget();
    m_padMappingPage->setTool(0);
    m_padMappingPage->reloadWidget();
    m_touchMappingPage->setTool(1);
    m_touchMappingPage->reloadWidget();
    m_penPage->reloadWidget();
    m_touchPage->reloadWidget();

    // ok we found a device, lets add all necessary information
    m_ui->profileSelector->setEnabled( true );
    m_ui->addProfileButton->setEnabled( true );
    m_ui->delProfileButton->setEnabled( true );
    m_ui->deviceTabWidget->setEnabled( true );
    m_ui->deviceTabWidget->setVisible( true );

    // load available profiles and create a default one if no profile exist;
    KConfigGroup deviceGroup = m_profileManagement->availableProfiles();

    if( deviceGroup.groupList().isEmpty() ) {
        m_profileManagement->createNewProfile();
        // set the state of the kcm and kded to the new default profile
        m_profileManagement->setProfileName( QLatin1String( "default" ) );
        applyProfile();
        //load the list of profiles again, will update to have the default profile
        deviceGroup = m_profileManagement->availableProfiles();
    }

    // fill combobox with all available profiles
    m_ui->profileSelector->blockSignals( true );
    m_ui->profileSelector->clear();
    m_ui->profileSelector->addItems( deviceGroup.groupList() );
    m_ui->profileSelector->blockSignals( false );

    // add all tab pages
    m_ui->deviceTabWidget->addTab( m_generalPage, i18nc( "Basic overview page for the tablet hardware", "General" ) );
    m_ui->deviceTabWidget->addTab( m_penPage, i18n( "Pen" ) );

    QDBusReply<bool> hasPadButtons = m_deviceInterface->call( QLatin1String( "hasPadButtons" ) );
    if( hasPadButtons ) {
        m_ui->deviceTabWidget->addTab( m_padButtonPage, i18n( "Pad Buttons" ) );
    }

    QDBusReply<QString> touchAvailable = m_deviceInterface->call( QLatin1String( "touchName" ) );
    QString touchName = touchAvailable.value();
    if( !touchName.isEmpty() ) {
        m_ui->deviceTabWidget->addTab( m_touchPage, i18n( "Touch" ) );
    }

    m_ui->deviceTabWidget->addTab( m_padMappingPage, i18n( "Pad Mapping" ) );
    if( !touchName.isEmpty() ) {
        m_ui->deviceTabWidget->addTab( m_touchMappingPage, i18n( "Touch Mapping" ) );
    }

    // switch to the current active profile
    QDBusReply<QString> profile = m_tabletInterface->call( QLatin1String( "profile" ) );
    if( profile.isValid() ) {
        m_ui->profileSelector->setCurrentItem( profile );
        switchProfile( profile );
    }
}

void TabletWidget::addProfile()
{
    bool ok;
    QString text = KInputDialog::getText( i18n( "Add new profile" ),
                                          i18n( "Profile name:" ), QString(), &ok, this );
    if( ok && !text.isEmpty() ) {
        m_profileManagement->createNewProfile( text );
    }
    else {
        return;
    }

    // refill combobox with all available profiles
    KConfigGroup deviceGroup = m_profileManagement->availableProfiles();
    m_ui->profileSelector->blockSignals( true );
    m_ui->profileSelector->clear();
    m_ui->profileSelector->addItems( deviceGroup.groupList() );
    int index = m_ui->profileSelector->findText( text );
    m_ui->profileSelector->setCurrentIndex( index );
    m_ui->profileSelector->blockSignals( false );

    switchProfile( text );
}

void TabletWidget::delProfile()
{
    // currently selected profile
    m_profileManagement->deleteProfile();
    KConfigGroup deviceGroup = m_profileManagement->availableProfiles();
    m_ui->profileSelector->blockSignals( true );
    m_ui->profileSelector->clear();
    m_ui->profileSelector->addItems( deviceGroup.groupList() );
    m_ui->profileSelector->blockSignals( false );

    switchProfile( m_ui->profileSelector->currentText() );
}

void TabletWidget::saveProfile()
{
    m_generalPage->saveToProfile();
    m_padButtonPage->saveToProfile();
    m_padMappingPage->saveToProfile();
    m_touchMappingPage->saveToProfile();
    m_penPage->saveToProfile();
    m_touchPage->saveToProfile();

    m_profileChanged = false;
    emit changed( false );

    applyProfile();
}

void TabletWidget::switchProfile( const QString &profile )
{
    if( m_profileChanged ) {
        QPointer<KDialog> saveDialog = new KDialog();
        Ui::SaveProfile askToSave;
        QWidget *widget = new QWidget( this );
        askToSave.setupUi( widget );
        saveDialog->setMainWidget( widget );
        saveDialog->setButtons( KDialog::Apply | KDialog::Cancel );
        connect( saveDialog, SIGNAL( applyClicked() ), saveDialog, SLOT( accept() ) );
        int ret;
        ret = saveDialog->exec();

        if( ret == KDialog::Accepted ) {
            saveProfile();
        }
    }

    m_profileManagement->setProfileName( profile );

    m_generalPage->loadFromProfile();
    m_padButtonPage->loadFromProfile();
    m_padMappingPage->loadFromProfile();
    m_touchMappingPage->loadFromProfile();
    m_penPage->loadFromProfile();
    m_touchPage->loadFromProfile();

    m_profileChanged = false;
    emit changed( false );

    applyProfile();
}

void TabletWidget::reloadProfile()
{
    m_generalPage->loadFromProfile();
    m_padButtonPage->loadFromProfile();
    m_padMappingPage->loadFromProfile();
    m_touchMappingPage->loadFromProfile();
    m_penPage->loadFromProfile();
    m_touchPage->loadFromProfile();

    m_profileChanged = false;
    emit changed( false );
}

void TabletWidget::applyProfile()
{
    m_tabletInterface->call( QLatin1String( "setProfile" ), m_profileManagement->profileName() );
}

void TabletWidget::profileChanged()
{
    m_profileChanged = true;
    emit changed( true );
}

void TabletWidget::showError( const QString &errMsg )
{
    delete m_deviceError;

    m_deviceError = new QWidget();
    Ui::ErrorWidget ew;
    ew.setupUi( m_deviceError );
    ew.errorImage->setPixmap( KIconLoader::global()->loadIcon( QLatin1String( "dialog-warning" ), KIconLoader::NoGroup, 128 ) );
    ew.errorText->setText( errMsg );
    m_ui->deviceTabWidget->setVisible( false );
    m_ui->verticalLayout->addWidget( m_deviceError );

    m_ui->profileSelector->setEnabled( false );
    m_ui->addProfileButton->setEnabled( false );
    m_ui->delProfileButton->setEnabled( false );
}
