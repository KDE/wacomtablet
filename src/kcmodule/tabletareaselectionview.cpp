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

#include "tabletareaselectionview.h"
#include "ui_tabletareaselectionview.h"

#include "calibrationdialog.h"
#include "stringutils.h"

#include <QtCore/QList>

using namespace Wacom;

namespace Wacom
{
    class TabletAreaSelectionViewPrivate
    {
        public:
            TabletAreaSelectionViewPrivate() : ui(new Ui::TabletAreaSelectionView) {}
            ~TabletAreaSelectionViewPrivate() {
                delete ui;
            }

            Ui::TabletAreaSelectionView* ui;
            QString                        deviceName;
    }; // PRIVATE CLASS
} // NAMESPACE


TabletAreaSelectionView::TabletAreaSelectionView(QWidget* parent)
        : QWidget(parent), d_ptr(new TabletAreaSelectionViewPrivate)
{
    setupUi();
}


TabletAreaSelectionView::~TabletAreaSelectionView()
{
    delete this->d_ptr;
}


const QString TabletAreaSelectionView::getSelection() const
{
    Q_D(const TabletAreaSelectionView);

    if (d->ui->fullTabletRadioButton->isChecked()) {
        return QLatin1String("-1 -1 -1 -1");
    }

    // convert rectangle to "x1 y1 x2 y2" format.
    QRect selection = d->ui->areaWidget->getSelection();

    return QString::fromLatin1("%1 %2 %3 %4").arg(selection.x())
                                             .arg(selection.y())
                                             .arg(selection.x() + selection.width())
                                             .arg(selection.y() + selection.height());
}


void TabletAreaSelectionView::setSelection(const QString& selection)
{
    Q_D(const TabletAreaSelectionView);

    if (selection.compare(QLatin1String("-1 -1 -1 -1"), Qt::CaseInsensitive) == 0) {

        // full screen selection
        setTabletAreaType(TabletAreaSelectionView::FullTabletArea);

    } else {
        // area or invalid selection
        QRect selectionRect = StringUtils::toQRectByCoordinates(selection);

        if (selectionRect.isEmpty()) {
            // invalid selection => full screen
            setTabletAreaType(TabletAreaSelectionView::FullTabletArea);

        } else {
            // area selection
            setTabletAreaType(TabletAreaSelectionView::PartialTabletArea);
            d->ui->areaWidget->setSelection(selectionRect);
        }
    }
}


void TabletAreaSelectionView::setupWidget(const QRect& tabletArea, const QList< QRect >& screenAreas, const QRect& screenAreaSelection, const QString& deviceName)
{
    Q_D(TabletAreaSelectionView);

    d->deviceName = deviceName;

    setupScreenArea(screenAreas, screenAreaSelection);
    setupTabletArea(tabletArea);
    setTabletAreaType(TabletAreaSelectionView::FullTabletArea);
}


void TabletAreaSelectionView::onCalibrateClicked()
{
    Q_D(TabletAreaSelectionView);

    CalibrationDialog calibDialog(d->deviceName);
    calibDialog.exec();

    QRect area = calibDialog.calibratedArea();
    setSelection(area);

    emit changed();
}


void TabletAreaSelectionView::onForceProportionsClicked()
{
    Q_D(TabletAreaSelectionView);

    QRect tabletArea          = d->ui->areaWidget->getVirtualArea();
    QRect screenAreaSelection = d->ui->screenArea->getSelection();

    if (screenAreaSelection.isEmpty()) {
        return;
    }

    qreal screenAreaSelectionRatio = (float)screenAreaSelection.width() / screenAreaSelection.height();
    qreal newWidth, newHeight;

    if (screenAreaSelection.width() > screenAreaSelection.height()) {

        newWidth  = tabletArea.width();
        newHeight = newWidth / screenAreaSelectionRatio;

        if (newHeight > tabletArea.height()) {
            newHeight = tabletArea.height();
            newWidth  = newHeight * screenAreaSelectionRatio;
        }

    } else {

        newHeight = tabletArea.height();
        newWidth  = newHeight * screenAreaSelectionRatio;

        if (newWidth > tabletArea.width()) {
            newWidth  = tabletArea.width();
            newHeight = newWidth / screenAreaSelectionRatio;
        }
    }

    setTabletAreaType(TabletAreaSelectionView::PartialTabletArea);
    setSelection(QRect(0, 0, qRound(newWidth), qRound(newHeight)));

    emit changed();
}


void TabletAreaSelectionView::onFullTabletSelected(bool checked)
{
    if (!checked) {
        return;
    }
    setTabletAreaType(TabletAreaSelectionView::FullTabletArea);
    emit changed();
}


void TabletAreaSelectionView::onTabletAreaChanged()
{
    emit changed();
}


void TabletAreaSelectionView::onTabletAreaSelected(bool checked)
{
    if (!checked) {
        return;
    }
    setTabletAreaType(TabletAreaSelectionView::PartialTabletArea);
    emit changed();
}


void TabletAreaSelectionView::setSelection(const QRect& selection)
{
    Q_D(TabletAreaSelectionView);

    d->ui->areaWidget->setSelection(selection);

    if (isFullAreaSelection(selection)) {
        setTabletAreaType(TabletAreaSelectionView::FullTabletArea);
    } else {
        setTabletAreaType(TabletAreaSelectionView::PartialTabletArea);
    }
}


void TabletAreaSelectionView::setTabletAreaType(TabletAreaSelectionView::TabletAreaType type)
{
    Q_D(TabletAreaSelectionView);

    d->ui->fullTabletRadioButton->blockSignals(true);
    d->ui->tabletAreaRadioButton->blockSignals(true);

    if (type == TabletAreaSelectionView::FullTabletArea) {
        d->ui->fullTabletRadioButton->setChecked(true);
        d->ui->tabletAreaRadioButton->setChecked(false);

        d->ui->areaWidget->clearSelection();
        d->ui->areaWidget->setEnabled(false);

    } else {
        d->ui->tabletAreaRadioButton->setChecked(true);
        d->ui->fullTabletRadioButton->setChecked(false);

        d->ui->areaWidget->setEnabled(true);
    }

    d->ui->fullTabletRadioButton->blockSignals(false);
    d->ui->tabletAreaRadioButton->blockSignals(false);
}



bool TabletAreaSelectionView::isFullAreaSelection(const QRect& selection) const
{
    Q_D (const TabletAreaSelectionView);

    return (
        !selection.isValid() ||
        selection == d->ui->areaWidget->getVirtualArea() ||
        (
            selection.x()      == -1 &&
            selection.y()      == -1 &&
            selection.width()  == -1 &&
            selection.height() == -1
        )
    );
}


void TabletAreaSelectionView::setupScreenArea(const QList< QRect >& screenAreas, const QRect& screenAreaSelection)
{
    Q_D(TabletAreaSelectionView);

    d->ui->screenArea->setEnabled(false);
    d->ui->screenArea->setWidgetTargetSize(QSize(150,150));
    d->ui->screenArea->setFont(QFont(QLatin1String("sans"), 8));

    if (screenAreas.size() > 0) {

        d->ui->screenArea->setDrawAreaCaptions(true);
        d->ui->screenArea->setDrawSelectionCaption(true);

        QStringList captions;

        for (int i = 0 ; i < screenAreas.size() ; ++i) {
            captions.append(QString::number(i+1));
        }

        d->ui->screenArea->setAreas(screenAreas, captions);
        d->ui->screenArea->setSelection(screenAreaSelection);

    } else {
        d->ui->screenArea->setDrawAreaCaptions(true);
        d->ui->screenArea->setDrawSelectionCaption(false);
        d->ui->screenArea->setArea(QRect(0,0,1920,1200), i18n("Internal Error"));
    }
}


void TabletAreaSelectionView::setupTabletArea(const QRect& tabletArea)
{
    Q_D(TabletAreaSelectionView);

    d->ui->areaWidget->setWidgetTargetSize(QSize(400,400));
    d->ui->areaWidget->setOutOfBoundsMargin(.1);

    if (tabletArea.isValid()) {
        QString tabletCaption = QString::fromLatin1("%1x%2").arg(tabletArea.width()).arg(tabletArea.height());

        d->ui->areaWidget->setDrawAreaCaptions(true);
        d->ui->areaWidget->setDrawSelectionCaption(true);
        d->ui->areaWidget->setArea(tabletArea, tabletCaption);

    } else {
        d->ui->areaWidget->setDrawAreaCaptions(true);
        d->ui->areaWidget->setDrawSelectionCaption(false);
        d->ui->areaWidget->setArea(QRect(0,0,1920,1200), i18n("Internal Error"));
    }
}


void TabletAreaSelectionView::setupUi()
{
    Q_D(TabletAreaSelectionView);

    d->ui->setupUi(this);

    setupTabletArea();
    setupScreenArea();

    connect( d->ui->areaWidget, SIGNAL(selectionChanged()), this, SLOT(onTabletAreaChanged()) );

    setTabletAreaType(TabletAreaSelectionView::FullTabletArea);
}


