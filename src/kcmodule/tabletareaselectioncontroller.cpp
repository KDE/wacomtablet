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

#include "tabletareaselectioncontroller.h"
#include "tabletareaselectionview.h"
#include "calibrationdialog.h"

#include "stringutils.h"
#include "x11info.h"
#include "x11wacom.h"

using namespace Wacom;


namespace Wacom {
    class TabletAreaSelectionControllerPrivate
    {
        public:
            TabletAreaSelectionControllerPrivate() {
                view = NULL;
            }

            TabletAreaSelectionView* view;
            QRect                    tabletGeometry;
            QList<QRect>             screenGeometries;
            QRect                    screenSelection;
            QString                  deviceName;
    };
}



TabletAreaSelectionController::TabletAreaSelectionController()
        : QObject(), d_ptr(new TabletAreaSelectionControllerPrivate)
{
    // nothing to do except for initializing our private class
}


TabletAreaSelectionController::~TabletAreaSelectionController()
{
    delete this->d_ptr;
}


const QString TabletAreaSelectionController::getSelection() const
{
    Q_D(const TabletAreaSelectionController);

    QLatin1String fullTabletSelection(QLatin1String("-1 -1 -1 -1"));

    if (!hasView()) {
        return fullTabletSelection;
    }

    QRect selection = d->view->getSelection();

    if (!selection.isValid() || selection == d->tabletGeometry) {
        return fullTabletSelection;
    }

    return QString::fromLatin1("%1 %2 %3 %4").arg(selection.x())
                                             .arg(selection.y())
                                             .arg(selection.x() + selection.width())
                                             .arg(selection.y() + selection.height());
}


void TabletAreaSelectionController::setSelection(const QString& selection)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    if (selection.contains(QLatin1String("-1 -1 -1 -1"))) {
        // full screen selection
        d->view->selectAll();

    } else {
        // area or invalid selection
        setSelection(StringUtils::toQRectByCoordinates(selection));
    }
}



void TabletAreaSelectionController::setView(TabletAreaSelectionView* view)
{
    Q_D(TabletAreaSelectionController);

    // TODO cleanup signal/slot connections if we already have a view
    d->view = view;

    if (view != NULL) {
        connect(view, SIGNAL(signalCalibrateClicked()),     this, SLOT(onCalibrateClicked()));
        connect(view, SIGNAL(signalSetScreenProportions()), this, SLOT(onSetScreenProportions()));
    }
}


void TabletAreaSelectionController::setupController(const QString& tabletSelection, const QString& screenSelection, const QString& deviceName)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    d->deviceName       = deviceName;
    d->tabletGeometry   = X11Wacom::getMaximumTabletArea(deviceName);
    d->screenGeometries = X11Info::getScreenGeometries();
    d->screenSelection  = convertScreenAreaMappingToQRect(d->screenGeometries, screenSelection);

    d->view->setupScreens(d->screenGeometries, d->screenSelection, QSize(150,150));
    d->view->setupTablet(d->tabletGeometry, QSize(400,400));

    setSelection(tabletSelection);
}


void TabletAreaSelectionController::onCalibrateClicked()
{
    Q_D(TabletAreaSelectionController);

    CalibrationDialog calibDialog(d->deviceName);
    calibDialog.exec();

    setSelection(calibDialog.calibratedArea());
}


void TabletAreaSelectionController::onSetScreenProportions()
{
    Q_D(TabletAreaSelectionController);

    QRect tabletGeometry  = d->tabletGeometry;
    QRect screenSelection = d->screenSelection;

    if (screenSelection.isEmpty()) {
        return;
    }

    qreal screenAreaSelectionRatio = (float)screenSelection.width() / screenSelection.height();
    qreal newWidth, newHeight;

    if (screenSelection.width() > screenSelection.height()) {

        newWidth  = tabletGeometry.width();
        newHeight = newWidth / screenAreaSelectionRatio;

        if (newHeight > tabletGeometry.height()) {
            newHeight = tabletGeometry.height();
            newWidth  = newHeight * screenAreaSelectionRatio;
        }

    } else {

        newHeight = tabletGeometry.height();
        newWidth  = newHeight * screenAreaSelectionRatio;

        if (newWidth > tabletGeometry.width()) {
            newWidth  = tabletGeometry.width();
            newHeight = newWidth / screenAreaSelectionRatio;
        }
    }

    setSelection(QRect(0, 0, qRound(newWidth), qRound(newHeight)));
}


bool TabletAreaSelectionController::hasView() const
{
    Q_D(const TabletAreaSelectionController);

    return (d->view != NULL);
}



const QRect TabletAreaSelectionController::convertScreenAreaMappingToQRect(const QList< QRect >& screenAreas, const QString& selectedScreenArea) const
{
    QRect   result(0, 0, 0, 0);
    QRegExp monitorRegExp(QLatin1String("map(\\d+)"), Qt::CaseInsensitive);

    if (monitorRegExp.indexIn(selectedScreenArea, 0) != -1) {
        // monitor mapping
        int screenNum = monitorRegExp.cap(1).toInt();

        if ( 0 <= screenNum && screenNum < screenAreas.count() ) {
            result = screenAreas.at(screenNum);
        }
    }

    if (result.isEmpty()) {
        // full screen or invalid => full screen
        foreach (QRect screen, screenAreas) {
            result = result.united(screen);
        }
    }

    return result;
}



void TabletAreaSelectionController::setSelection(const QRect& selection)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    if (selection.isEmpty() || selection == d->tabletGeometry) {
        d->view->selectAll();
    } else {
        d->view->selectPart(selection);
    }
}
