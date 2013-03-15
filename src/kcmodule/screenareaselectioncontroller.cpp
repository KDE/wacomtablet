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

#include "debug.h" // always needs to be first include

#include "screenareaselectioncontroller.h"
#include "screenareaselectionview.h"

#include "stringutils.h"
#include "x11info.h"
#include "x11wacom.h"

using namespace Wacom;

namespace Wacom
{
    class ScreenAreaSelectionControllerPrivate
    {
        public:
            ScreenAreaSelectionControllerPrivate() {
                widget = NULL;
            }
            ScreenAreaSelectionView*   widget;           // passed by the presenter
            QString                    deviceName;       // The Xinput device name we are handling
            QList< QRect >             screenGeometries; // The X11 screen geometries.
            QRect                      tabletGeometry;   // The tablet geometry.
    };
}


ScreenAreaSelectionController::ScreenAreaSelectionController()
        : d_ptr(new ScreenAreaSelectionControllerPrivate)
{
    // nothing to do, except for initializing our private class
}


ScreenAreaSelectionController::~ScreenAreaSelectionController()
{
    delete this->d_ptr;
}


const QString ScreenAreaSelectionController::getSelection() const
{
    Q_D(const ScreenAreaSelectionController);

    QString fullScreenSelection = QLatin1String("full");

    if (!hasWidget()) {
        return fullScreenSelection;
    }

    int selectedMonitor = d->widget->getSelectedMonitor();

    return (selectedMonitor < 0 ? fullScreenSelection : QString::fromLatin1("map%1").arg(selectedMonitor));
}


void ScreenAreaSelectionController::setSelection(const QString& selection)
{
    QRegExp monitorRegExp(QLatin1String("map(\\d+)"), Qt::CaseInsensitive);

    if (monitorRegExp.indexIn(selection, 0) != -1) {
        // monitor mapping
        int screenNum = monitorRegExp.cap(1).toInt();
        onMonitorSelected(screenNum);

    } else {
        // full screen mapping - unite all X11 screens to one big rectangle
        onFullScreenSelected();
    }
}


void ScreenAreaSelectionController::setupController(const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName)
{
    Q_D(ScreenAreaSelectionController);

    if (!hasWidget()) {
        return;
    }

    d->deviceName       = deviceName;
    d->screenGeometries = X11Info::getScreenGeometries();
    d->tabletGeometry   = X11Wacom::getMaximumTabletArea(deviceName);

    QRect tabletSelectionGeometry = convertTabletAreaMappingToQRect(tabletSelection, d->tabletGeometry);

    // TODO do tablet rotation calculations

    // setup widget
    d->widget->setupTablet(d->tabletGeometry, tabletSelectionGeometry, tabletCaption, QSize(150,150));
    d->widget->setupScreens(d->screenGeometries, QSize(400,400));

    // set selection
    setSelection(screenSelection);
}


void ScreenAreaSelectionController::setView(ScreenAreaSelectionView* widget)
{
    Q_D(ScreenAreaSelectionController);

    d->widget = widget;
}



void ScreenAreaSelectionController::onFullScreenSelected()
{
    Q_D(ScreenAreaSelectionController);

    if (!hasWidget()) {
        return;
    }

    d->widget->setFullScreenSelection();
}



void ScreenAreaSelectionController::onMonitorSelected(int screenNum)
{
    Q_D(ScreenAreaSelectionController);

    if (!hasWidget()) {
        return;
    }

    d->widget->setMonitorSelection(screenNum);
}



const QRect ScreenAreaSelectionController::convertTabletAreaMappingToQRect(const QString& selection, const QRect& geometry) const
{
    QRect result = StringUtils::toQRectByCoordinates(selection);

    if (!result.isValid() ||
        selection.contains(QLatin1String("-1 -1 -1 -1")) ||
        selection.contains(QLatin1String("full"), Qt::CaseInsensitive) ) {
        result = geometry;
    }

    return result;
}



bool ScreenAreaSelectionController::hasWidget() const
{
    Q_D(const ScreenAreaSelectionController);

    return (d->widget != NULL);
}
