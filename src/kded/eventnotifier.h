/*
 * Copyright 2012 Alexander Maret-Huskinson <alex@maret.de>
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

#ifndef EVENTNOTIFIER_H
#define EVENTNOTIFIER_H

#include "tabletrotation.h"
#include "tabletinformation.h"

#include <QtGui/QWidget>

namespace Wacom
{
class EventNotifier : public QWidget
{
    Q_OBJECT

public:
    explicit EventNotifier(QWidget *parent = 0);
    virtual ~EventNotifier();

    //! register the notifier with the window system
    virtual void start() = 0;

    //! unregister the notifier from the window system
    virtual void stop() = 0;


Q_SIGNALS:

    void tabletAdded (const TabletInformation& info);

    void tabletRemoved (const TabletInformation& info);

    void screenRotated (TabletRotation tabletRotation);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
