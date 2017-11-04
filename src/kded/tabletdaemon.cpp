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

#include "tabletdaemon.h"
#include "debug.h"

#include "dbustabletservice.h"
#include "tabletfinder.h"
#include "tablethandler.h"
#include "wacomadaptor.h"
#include "x11eventnotifier.h"
#include "globalactions.h"
#include "../version.h"

// common includes
#include "aboutdata.h"

// stdlib includes
#include <memory>

// KDE includes
#include <KPluginFactory>
#include <KNotification>
#include <KLocalizedString>
#include <QGuiApplication>
#include <QScreen>

using namespace Wacom;

K_PLUGIN_FACTORY( WacomTabletFactory, registerPlugin<TabletDaemon>(); )
K_EXPORT_PLUGIN( WacomTabletFactory( "wacomtabletdaemon" ) )

namespace Wacom {
/**
  * Private class of the TabletDaemon for the d-pointer
  */
class TabletDaemonPrivate {
public:
    TabletDaemonPrivate()
        : tabletHandler(), dbusTabletService(tabletHandler) {}

    TabletHandler                     tabletHandler;    /**< tablet handler */
    DBusTabletService                 dbusTabletService;
    std::shared_ptr<GlobalActions>  actionCollection; /**< Collection of all global actions */

}; // CLASS
}  // NAMESPACE



TabletDaemon::TabletDaemon( QObject *parent, const QVariantList &args )
    : KDEDModule( parent ), d_ptr( new TabletDaemonPrivate )
{
    Q_UNUSED( args );
    Q_D( TabletDaemon );

    setupApplication();
    setupDBus();
    setupEventNotifier();
    setupActions();

    // scan for connected devices
    TabletFinder::instance().scan();

    // connect profile changed handler after searching for tablets as this is only used for the global shortcut workaround.
    connect(&(d->tabletHandler), &TabletHandler::profileChanged, this, &TabletDaemon::onProfileChanged);

    // Connecting this after the device has been set up ensures that no notification is send on startup.
    connect( &(d->tabletHandler), SIGNAL(notify(QString,QString,QString)), this, SLOT(onNotify(QString,QString,QString)) );
}



TabletDaemon::~TabletDaemon()
{
    X11EventNotifier::instance().stop();
    delete this->d_ptr;
}



void TabletDaemon::onNotify(const QString& eventId, const QString& title, const QString& message)
{
    Q_D( TabletDaemon );

    KNotification* notification = new KNotification(eventId);
    notification->setComponentName( QStringLiteral("wacomtablet") );
    notification->setTitle(title);
    notification->setText(message);
    notification->setIconName( QLatin1String( "input-tablet" ) );
    notification->sendEvent();
}



void TabletDaemon::onProfileChanged(const QString &tabletId, const QString& profile)
{
    Q_UNUSED(tabletId);
    Q_UNUSED(profile);

    // When closing the KCM module the KAction destructor disables all global shortcuts.
    // Make sure the global shortcuts are restored when a profile changes. This is not
    // optimal but at least it will enable the shortcuts again.
    dbgWacom << QLatin1String("Restoring global keyboard shortcuts...");
    setupActions();
}



void TabletDaemon::setupActions()
{
    Q_D( TabletDaemon );

    //if someone adds another action also add it to kcmodule/generalwidget.cpp

    // This method is called multiple times - make sure the action collection is only created once.
    if (d->actionCollection.get() == NULL) {
        d->actionCollection = std::shared_ptr<GlobalActions>(new GlobalActions(false, this));
        d->actionCollection->setConfigGlobal(true);
    }

    connect( d->actionCollection.get(), SIGNAL(toggleTouchTriggered()), &(d->tabletHandler), SLOT(onToggleTouch()) );
    connect( d->actionCollection.get(), SIGNAL(toggleStylusTriggered()), &(d->tabletHandler), SLOT(onTogglePenMode()) );
    connect( d->actionCollection.get(), SIGNAL(toggleScreenMapTriggered()), &(d->tabletHandler), SLOT(onToggleScreenMapping()) );
    connect( d->actionCollection.get(), SIGNAL(mapToFullScreenTriggered()), &(d->tabletHandler), SLOT(onMapToFullScreen()) );
    connect( d->actionCollection.get(), SIGNAL(mapToScreen1Triggered()), &(d->tabletHandler), SLOT(onMapToScreen1()) );
    connect( d->actionCollection.get(), SIGNAL(mapToScreen2Triggered()), &(d->tabletHandler), SLOT(onMapToScreen2()) );
    connect( d->actionCollection.get(), SIGNAL(nextProfileTriggered()), &(d->tabletHandler), SLOT(onNextProfile()) );
    connect( d->actionCollection.get(), SIGNAL(previousProfileTriggered()), &(d->tabletHandler), SLOT(onPreviousProfile()) );
}



void TabletDaemon::setupApplication()
{
    static AboutData about( QLatin1Literal("wacomtablet"),
                            i18n( "Graphic Tablet Configuration daemon" ),
                            QLatin1String(kded_version), i18n( "A Wacom tablet control daemon" ) );
}



void TabletDaemon::setupDBus()
{
    Q_D( TabletDaemon );

    // connect tablet handler events to D-Bus
    // this is done here and not in the D-Bus tablet service to facilitate unit testing
    connect(&(d->tabletHandler),     SIGNAL (profileChanged(QString, QString)),
            &(d->dbusTabletService), SLOT   (onProfileChanged(QString, QString)));

    connect(&(d->tabletHandler),     SIGNAL (tabletAdded(TabletInformation)),
            &(d->dbusTabletService), SLOT   (onTabletAdded(TabletInformation)));

    connect(&(d->tabletHandler),     SIGNAL (tabletRemoved(QString)),
            &(d->dbusTabletService), SLOT   (onTabletRemoved(QString)));
}



void TabletDaemon::setupEventNotifier()
{
    Q_D( TabletDaemon );

    // Set up monitoring for individual screen geometry changes
    monitorAllScreensGeometry();

    // Set up monitoring for screens being added, removed or reordered
    connect(qApp, &QGuiApplication::primaryScreenChanged, &(d->tabletHandler), &TabletHandler::onScreenAddedRemoved);
    connect(qApp, &QGuiApplication::screenAdded, &(d->tabletHandler), &TabletHandler::onScreenAddedRemoved);
    connect(qApp, &QGuiApplication::screenRemoved, &(d->tabletHandler), &TabletHandler::onScreenAddedRemoved);

    // Set up tablet connected/disconnected signals
    connect( &X11EventNotifier::instance(), &X11EventNotifier::tabletAdded,   &TabletFinder::instance(), &TabletFinder::onX11TabletAdded);
    connect( &X11EventNotifier::instance(), &X11EventNotifier::tabletRemoved, &TabletFinder::instance(), &TabletFinder::onX11TabletRemoved);

    connect( &TabletFinder::instance(),     &TabletFinder::tabletAdded,       &(d->tabletHandler),       &TabletHandler::onTabletAdded);
    connect( &TabletFinder::instance(),     &TabletFinder::tabletRemoved,     &(d->tabletHandler),       &TabletHandler::onTabletRemoved);

    X11EventNotifier::instance().start();
}

void TabletDaemon::monitorAllScreensGeometry()
{
    // FIXME: Should be duplicating old, buggy behaviour:
    // rotation changes monitored for all screens, which causes
    // unwanted rotation in a multi-screen setup

    // Add existing screens
    for (const auto &screen : QGuiApplication::screens())
    {
        monitorScreenGeometry(screen);
    }

    // Monitor future screens
    connect(qApp, &QGuiApplication::screenAdded, this, &TabletDaemon::monitorScreenGeometry);
}

void TabletDaemon::monitorScreenGeometry(QScreen *screen)
{
    Q_D( TabletDaemon );

    auto tabletHandler = &(d->tabletHandler);

    connect(screen, &QScreen::orientationChanged, tabletHandler, &TabletHandler::onScreenRotated);
    screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                     Qt::PortraitOrientation |
                                     Qt::InvertedLandscapeOrientation |
                                     Qt::InvertedPortraitOrientation);

    connect(screen, &QScreen::geometryChanged, &(d->tabletHandler), &TabletHandler::onScreenGeometryChanged);
}

#include "tabletdaemon.moc"
