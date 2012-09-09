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

// common
#include "dbustabletinterface.h"

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
        std::auto_ptr<Ui::TabletWidget>  m_ui;                  /**< Handler to the tabletwidget.ui file */
        QPointer<GeneralWidget>          m_generalPage;         /**< Widget that shows some basic information about the tablet */
        QPointer<PadButtonWidget>        m_padButtonPage;       /**< Widget for the pad button settings */
        QPointer<PadMapping>             m_padMappingPage;      /**< Widget for the pad rotation and working area */
        QPointer<PadMapping>             m_touchMappingPage;    /**< Widget for the touch rotation and working area */
        QPointer<PenWidget>              m_penPage;             /**< Widget for the pen settings (stylus/eraser) */
        QPointer<TouchWidget>            m_touchPage;           /**< Widget for the touch settings */
        QPointer<QWidget>                m_deviceError;         /**< Shows the error widget */
        bool                             m_profileChanged;      /**< True if the profile was changed and not saved yet */
}; // CLASS
}  // NAMESPACE



TabletWidget::TabletWidget( QWidget *parent )
    : QWidget( parent ), d_ptr(new TabletWidgetPrivate)
{
    init();
    loadTabletInformation();
}

TabletWidget::~TabletWidget()
{
    delete this->d_ptr;
}



void TabletWidget::init()
{
    Q_D( TabletWidget );

    DBusTabletInterface* dbusTabletInterface = &DBusTabletInterface::instance();

    if( !dbusTabletInterface->isValid() ) {
        kDebug() << "DBus interface not available";
    }

    d->m_profileChanged    = false;
    d->m_ui                = std::auto_ptr<Ui::TabletWidget>(new Ui::TabletWidget);

    d->m_generalPage       = new GeneralWidget();
    d->m_padButtonPage     = new PadButtonWidget();
    d->m_padMappingPage    = new PadMapping();
    d->m_touchMappingPage  = new PadMapping();
    d->m_penPage           = new PenWidget();
    d->m_touchPage         = new TouchWidget();

    d->m_ui->setupUi( this );
    d->m_ui->addProfileButton->setIcon( KIcon( QLatin1String( "document-new" ) ) );
    d->m_ui->delProfileButton->setIcon( KIcon( QLatin1String( "edit-delete-page" ) ) );

    // connect profile selector
    connect( d->m_ui->addProfileButton, SIGNAL( clicked( bool ) ), SLOT( addProfile() ) );
    connect( d->m_ui->delProfileButton, SIGNAL( clicked( bool ) ), SLOT( delProfile() ) );
    connect( d->m_ui->profileSelector, SIGNAL( currentIndexChanged( const QString ) ), SLOT( switchProfile( const QString ) ) );

    // connect configuration tabs
    connect( d->m_padButtonPage,    SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( d->m_padMappingPage,   SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( d->m_touchMappingPage, SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( d->m_penPage,          SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( d->m_touchPage,        SIGNAL( changed() ), SLOT( profileChanged() ) );
    connect( d->m_generalPage,      SIGNAL( changed() ), SLOT( profileChanged() ) );

    // connect DBus signals
    connect( dbusTabletInterface, SIGNAL( tabletAdded() ),   SLOT( loadTabletInformation() ) );
    connect( dbusTabletInterface, SIGNAL( tabletRemoved() ), SLOT( loadTabletInformation() ) );
}


void TabletWidget::loadTabletInformation()
{
    QDBusReply<bool> isAvailable = DBusTabletInterface::instance().isAvailable();

    if( !isAvailable.isValid() ) {
        QString errmsg = i18n( "D-Bus connection to the kded daemon not available.\n\n"
                               "Please start the Wacom tablet daemon and try again.\n"
                               "The daemon is responsible for tablet detection and profile support." );
        showError( errmsg );

    } else if( !isAvailable ) {
        QString errmsg = i18n( "No tablet device was found.\n\n"
                               "Please connect the device before you start this module.\n"
                               "If the device is already connected refer to the help file for any further information." );
        showError( errmsg );

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
    switchProfile( d->m_ui->profileSelector->currentText() );
}


void TabletWidget::saveProfile()
{
    Q_D( TabletWidget );

    d->m_generalPage->saveToProfile();
    d->m_padButtonPage->saveToProfile();
    d->m_padMappingPage->saveToProfile();
    d->m_touchMappingPage->saveToProfile();
    d->m_penPage->saveToProfile();
    d->m_touchPage->saveToProfile();

    d->m_profileChanged = false;
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

    d->m_generalPage->loadFromProfile();
    d->m_padButtonPage->loadFromProfile();
    d->m_padMappingPage->loadFromProfile();
    d->m_touchMappingPage->loadFromProfile();
    d->m_penPage->loadFromProfile();
    d->m_touchPage->loadFromProfile();

    d->m_profileChanged = false;
    emit changed( false );
}


void TabletWidget::applyProfile()
{
    DBusTabletInterface::instance().setProfile( ProfileManagement::instance().profileName() );
}


void TabletWidget::profileChanged()
{
    Q_D( TabletWidget );

    d->m_profileChanged = true;
    emit changed( true );
}


void TabletWidget::showError( const QString &errMsg )
{
    Q_D( TabletWidget );

    hideError();

    d->m_deviceError = new QWidget();
    Ui::ErrorWidget ew;
    ew.setupUi( d->m_deviceError );
    ew.errorImage->setPixmap( KIconLoader::global()->loadIcon( QLatin1String( "dialog-warning" ), KIconLoader::NoGroup, 128 ) );
    ew.errorText->setText( errMsg );

    hideConfig();

    d->m_ui->verticalLayout->addWidget( d->m_deviceError );
}


void TabletWidget::hideConfig()
{
    Q_D( TabletWidget );

    d->m_ui->profileSelector->setEnabled( false );
    d->m_ui->addProfileButton->setEnabled( false );
    d->m_ui->delProfileButton->setEnabled( false );
    d->m_ui->deviceTabWidget->setVisible( false );
}


void TabletWidget::hideError()
{
    Q_D( TabletWidget );

    if( d->m_deviceError ) {
        d->m_deviceError->setVisible( false );
        d->m_ui->verticalLayout->removeWidget( d->m_deviceError );
        delete d->m_deviceError;
        d->m_deviceError = NULL;
    }
}


bool TabletWidget::refreshProfileSelector ( const QString& profile )
{
    Q_D( TabletWidget );

    int         index    = -1;
    QStringList profiles = ProfileManagement::instance().availableProfiles();

    d->m_ui->profileSelector->blockSignals( true );
    d->m_ui->profileSelector->clear();
    d->m_ui->profileSelector->addItems( profiles );

    if (!profile.isEmpty()) {
        index = d->m_ui->profileSelector->findText( profile );
        d->m_ui->profileSelector->setCurrentIndex( index );
    }

    d->m_ui->profileSelector->blockSignals( false );

    return (index >= 0);
}


void TabletWidget::showConfig()
{
    Q_D( TabletWidget );

    // make sure no error message is active
    hideError();

    // reload profile and widget data
    ProfileManagement::instance().reload();
    
    d->m_generalPage->reloadWidget();
    d->m_padButtonPage->reloadWidget();
    d->m_padMappingPage->setTool(0);
    d->m_padMappingPage->reloadWidget();
    d->m_touchMappingPage->setTool(1);
    d->m_touchMappingPage->reloadWidget();
    d->m_penPage->reloadWidget();
    d->m_touchPage->reloadWidget();


    // initialize profile selector
    d->m_ui->profileSelector->setEnabled( true );
    d->m_ui->addProfileButton->setEnabled( true );
    d->m_ui->delProfileButton->setEnabled( true );

    if( ProfileManagement::instance().availableProfiles().isEmpty() ) {
        ProfileManagement::instance().createNewProfile();
        ProfileManagement::instance().setProfileName( QLatin1String( "default" ) );
        applyProfile();
    }

    refreshProfileSelector();


    // initialize configuration tabs
    d->m_ui->deviceTabWidget->clear();
    d->m_ui->deviceTabWidget->addTab( d->m_generalPage, i18nc( "Basic overview page for the tablet hardware", "General" ) );
    d->m_ui->deviceTabWidget->addTab( d->m_penPage, i18n( "Pen" ) );

    QDBusReply<bool> hasPadButtons = DBusTabletInterface::instance().hasPadButtons();
    if( hasPadButtons ) {
        d->m_ui->deviceTabWidget->addTab( d->m_padButtonPage, i18n( "Pad Buttons" ) );
    }

    QDBusReply<QString> touchAvailable = DBusTabletInterface::instance().getDeviceName(DeviceType::Touch);
    QString touchName = touchAvailable.value();
    if( !touchName.isEmpty() ) {
        d->m_ui->deviceTabWidget->addTab( d->m_touchPage, i18n( "Touch" ) );
    }

    d->m_ui->deviceTabWidget->addTab( d->m_padMappingPage, i18n( "Pad Mapping" ) );
    if( !touchName.isEmpty() ) {
        d->m_ui->deviceTabWidget->addTab( d->m_touchMappingPage, i18n( "Touch Mapping" ) );
    }

    d->m_ui->deviceTabWidget->setEnabled( true );
    d->m_ui->deviceTabWidget->setVisible( true );


    // switch to the currently active profile
    QDBusReply<QString> profile = DBusTabletInterface::instance().getProfile();
    if( profile.isValid() ) {
        d->m_ui->profileSelector->setCurrentItem( profile );
        switchProfile( profile );
    }
}


void TabletWidget::showSaveChanges()
{
    Q_D( TabletWidget );

    if( d->m_profileChanged ) {
        QPointer<KDialog> saveDialog = new KDialog();
        QWidget*          widget     = new QWidget( this );

        Ui::SaveProfile askToSave;
        askToSave.setupUi( widget );

        saveDialog->setMainWidget( widget );
        saveDialog->setButtons( KDialog::Apply | KDialog::Cancel );

        connect( saveDialog, SIGNAL( applyClicked() ), saveDialog, SLOT( accept() ) );

        if( saveDialog->exec() == KDialog::Accepted ) {
            saveProfile();
        }

        delete saveDialog;
    }
}
