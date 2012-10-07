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

#include "debug.h"
#include "tabletfinder.h"
#include "x11tabletfinder.h"

#include <QtCore/QList>

using namespace Wacom;

namespace Wacom
{
    class TabletFinderPrivate 
    {
        public:
            typedef QList<TabletInformation> TabletInformationList;

            TabletInformationList tabletList;

    }; // CLASS
} // NAMESPACE


TabletFinder::TabletFinder() : QObject(NULL), d_ptr(new TabletFinderPrivate)
{
}

TabletFinder::TabletFinder(const TabletFinder& finder) : QObject(NULL), d_ptr(new TabletFinderPrivate)
{
    // nothing to do - this class is a singleton and must not be copied

    // prevent compiler warning about unused parameter at least for debug builds.
    assert(&finder != NULL); 
}

TabletFinder::~TabletFinder()
{
    delete d_ptr;
}

TabletFinder& TabletFinder::operator=(const TabletFinder& finder)
{
    // nothing to do - this class is a singleton and must not be copied

    // prevent compiler warning about unused parameter at least for debug builds.
    assert(&finder != NULL);
    return *this;
}



TabletFinder& TabletFinder::instance()
{
    static TabletFinder instance;
    return instance;
}



bool TabletFinder::scan()
{
    Q_D(TabletFinder);

    X11TabletFinder x11tabletFinder;

    if (x11tabletFinder.scanDevices()) {
        d->tabletList = x11tabletFinder.getDevices();

        TabletFinderPrivate::TabletInformationList::ConstIterator iter;

        for (iter = d->tabletList.constBegin() ; iter != d->tabletList.constEnd() ; ++iter) {
            emit tabletAdded(*iter);
        }

        return true;
    }

    return false;
}



void TabletFinder::onX11TabletAdded(int deviceId)
{
    Q_D(TabletFinder);

    // check if we already know this tablet
    for (int i = 0 ; i < d->tabletList.size() ; ++i) {
        if (d->tabletList.at(i).hasDevice(deviceId)) {
            // we already know this tablet
            return;
        }
    }

    // scan for tablet devices
    X11TabletFinder x11TabletFinder;

    if (!x11TabletFinder.scanDevices()) {
        return;
    }

    // check if the device id can be found
    foreach (const TabletInformation& info, x11TabletFinder.getDevices()) {
        if (info.hasDevice(deviceId)) {
            d->tabletList.append(info);
            emit tabletAdded(info);
            return;
        }
    }
}



void TabletFinder::onX11TabletRemoved(int deviceId)
{
    Q_D(TabletFinder);

    // check if we know this tablet
    TabletFinderPrivate::TabletInformationList::iterator iter;

    for (iter = d->tabletList.begin() ; iter != d->tabletList.end() ; ++iter) {
        if (iter->hasDevice(deviceId)) {
            TabletInformation info = *iter;
            d->tabletList.erase(iter);
            emit tabletRemoved(info);
            return;
        }
    }
}

