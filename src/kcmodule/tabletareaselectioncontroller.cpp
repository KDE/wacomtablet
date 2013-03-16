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

#include "screenspace.h"
#include "stringutils.h"
#include "x11info.h"
#include "x11wacom.h"

using namespace Wacom;


namespace Wacom {
    class TabletAreaSelectionControllerPrivate
    {
        public:
            TabletAreaSelectionControllerPrivate() {
                view          = NULL;
                currentScreen = -1;
            }

            TabletAreaSelectionView* view;
            QRect                    tabletGeometry;
            QList<QRect>             screenGeometries;
            int                      currentScreen;
            QString                  deviceName;
            QHash<int, QRect>        mappings; // stores all screen => tablet mappings
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


const QString TabletAreaSelectionController::getMappings()
{
    Q_D(const TabletAreaSelectionController);

    // make sure the current mapping is included
    setMapping(d->currentScreen, d->view->getSelection());

    // create mapping string
    QHash<int,QRect>::const_iterator mapping = d->mappings.constBegin();

    QString     separator = QLatin1String("|");
    ScreenSpace screen;
    QString     area;
    QString     mappings;

    for ( ; mapping != d->mappings.constEnd() ; ++mapping) {

        area = convertQRectToTabletArea(mapping.value(), d->tabletGeometry);

        if (mapping.key() >= 0) {
            screen = ScreenSpace::monitor(mapping.key());
        } else {
            screen = ScreenSpace::desktop();
        }

        if (!mappings.isEmpty()) {
            mappings.append(separator);
        }

        mappings.append(QString::fromLatin1("%1:%2").arg(screen.toString()).arg(area));
    }

    return mappings;
}


const QString TabletAreaSelectionController::getScreenSpace() const
{
    Q_D(const TabletAreaSelectionController);

    if (d->currentScreen >= 0) {
        return ScreenSpace::monitor(d->currentScreen).toString();
    }

    return ScreenSpace::desktop().toString();
}



void TabletAreaSelectionController::select(int screenNumber)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    int countScreens = d->screenGeometries.count();

    // if we have only one screen then use desktop instead
    // because desktop mode also supports relative mode
    if (screenNumber == 0 && countScreens == 1) {
        screenNumber = -1;
    }

    // also use desktop if the screen number points to an invalid screen
    if (-1 > screenNumber || screenNumber >= countScreens) {
        screenNumber = -1;
    }

    // save current data
    setMapping(d->currentScreen, d->view->getSelection());

    // update screen number and set selection
    d->currentScreen = screenNumber;
    d->view->select(screenNumber, getMapping(screenNumber));
}


void TabletAreaSelectionController::select(const QString& screenSpace)
{
    select(ScreenSpace(screenSpace).getScreenNumber());
}



void TabletAreaSelectionController::setView(TabletAreaSelectionView* view)
{
    Q_D(TabletAreaSelectionController);

    // cleanup signal/slot connections if we already have a view
    if (d->view != NULL) {
        disconnect(d->view, SIGNAL(signalCalibrateClicked()),     this, SLOT(onCalibrateClicked()));
        disconnect(d->view, SIGNAL(signalScreenToggle()),         this, SLOT(onScreenToggle()));
        disconnect(d->view, SIGNAL(signalSetScreenProportions()), this, SLOT(onSetScreenProportions()));
    }

    // save view and connect signals
    d->view = view;

    if (view != NULL) {
        connect(view, SIGNAL(signalCalibrateClicked()),     this, SLOT(onCalibrateClicked()));
        connect(view, SIGNAL(signalScreenToggle()),         this, SLOT(onScreenToggle()));
        connect(view, SIGNAL(signalSetScreenProportions()), this, SLOT(onSetScreenProportions()));
    }
}


void TabletAreaSelectionController::setupController(const QString& mappings, const QString& deviceName)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    d->deviceName       = deviceName;
    d->tabletGeometry   = X11Wacom::getMaximumTabletArea(deviceName);
    d->screenGeometries = X11Info::getScreenGeometries();
    d->currentScreen    = -1;
    setMappings(mappings);

    d->view->setupScreens(d->screenGeometries, QSize(150,150));
    d->view->setupTablet(d->tabletGeometry, QSize(400,400));

    // make sure we have valid data set
    d->view->select(d->currentScreen, getMapping(d->currentScreen));
}


void TabletAreaSelectionController::onCalibrateClicked()
{
    Q_D(TabletAreaSelectionController);

    CalibrationDialog calibDialog(d->deviceName);
    calibDialog.exec();

    setSelection(calibDialog.calibratedArea());
}


void TabletAreaSelectionController::onScreenToggle()
{
    Q_D(TabletAreaSelectionController);

    select(d->currentScreen + 1);
}



void TabletAreaSelectionController::onSetScreenProportions()
{
    Q_D(TabletAreaSelectionController);

    QRect tabletGeometry  = d->tabletGeometry;
    QRect screenSelection = getScreenGeometry(d->currentScreen);

    if (screenSelection.isEmpty()) {
        return;
    }

    // calculate new height and width of the selection
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

    // calculate x and y to center the selection
    int newX = (int)((tabletGeometry.width() - newWidth) / 2.);
    int newY = (int)((tabletGeometry.height() - newHeight) / 2.);

    setSelection(QRect(newX, newY, qRound(newWidth), qRound(newHeight)));
}


bool TabletAreaSelectionController::hasView() const
{
    Q_D(const TabletAreaSelectionController);

    return (d->view != NULL);
}



const QRect TabletAreaSelectionController::convertTabletAreaToQRect(const QString& tabletArea, const QRect& tabletGeometry) const
{
    QRect result;

    if (!tabletArea.contains(QLatin1String("-1 -1 -1 -1"))) {
        result = StringUtils::toQRectByCoordinates(tabletArea);
    }

    if (result.isEmpty()) {
        result = tabletGeometry;
    }

    return result;
}


const QString TabletAreaSelectionController::convertQRectToTabletArea(const QRect& tabletRect, const QRect& tabletGeometry) const
{
    QRect selection = (tabletRect.isEmpty() ? tabletGeometry : tabletRect);

    return QString::fromLatin1("%1 %2 %3 %4").arg(selection.x())
                                             .arg(selection.y())
                                             .arg(selection.x() + selection.width())
                                             .arg(selection.y() + selection.height());

}



const QRect TabletAreaSelectionController::getScreenGeometry(int screenNumber) const
{
    Q_D(const TabletAreaSelectionController);

    QRect result(0,0,0,0);

    if (0 <= screenNumber && screenNumber < d->screenGeometries.count()) {
        // screen geometry
        result = d->screenGeometries.at(screenNumber);

    } else {
        // full screen or invalid => full screen
        foreach (QRect screen, d->screenGeometries) {
            result = result.united(screen);
        }
    }

    return result;
}



const QRect& TabletAreaSelectionController::getMapping(int screenNumber) const
{
    Q_D(const TabletAreaSelectionController);

    // try to find selection for the current screen
    QHash<int,QRect>::const_iterator citer = d->mappings.constFind(screenNumber);

    if (citer != d->mappings.constEnd()) {
        return citer.value();
    }

    // return full selection if none is available
    return d->tabletGeometry;
}


void TabletAreaSelectionController::setMapping(int screenNumber, const QRect& mapping)
{
    Q_D(TabletAreaSelectionController);

    d->mappings.insert(screenNumber, mapping);
}


void TabletAreaSelectionController::setMappings(const QString& mappings)
{
    Q_D(TabletAreaSelectionController);

    QStringList screenMappings = mappings.split(QLatin1String("|"));
    QString     separator(QLatin1String(":"));
    QStringList mapping;
    ScreenSpace screen;
    QString     selection;
    int         screenNumber;
    QRect       selectionRect;


    foreach(QString screenMapping, screenMappings) {

        mapping = screenMapping.split(separator);

        if (mapping.count() != 2) {
            continue;
        }

        screen        = ScreenSpace(mapping.at(0).trimmed());
        selection     = mapping.at(1).trimmed();
        selectionRect = convertTabletAreaToQRect(selection, d->tabletGeometry);
        screenNumber  = screen.getScreenNumber();

        d->mappings.insert(screenNumber, selectionRect);
    }
}



void TabletAreaSelectionController::setSelection(const QRect& selection)
{
    Q_D(TabletAreaSelectionController);

    if (!hasView()) {
        return;
    }

    if (selection.isEmpty() || selection == d->tabletGeometry) {
        d->view->selectFullTablet();
    } else {
        d->view->selectPartOfTablet(selection);
    }
}
