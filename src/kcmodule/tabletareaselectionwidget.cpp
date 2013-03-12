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

#include "tabletareaselectionwidget.h"
#include "ui_tabletareaselectionwidget.h"

#include "calibrationdialog.h"
#include "stringutils.h"

#include <QtCore/QList>
#include <QtCore/QRect>

using namespace Wacom;

namespace Wacom
{
    class TabletAreaSelectionWidgetPrivate
    {
        public:
            TabletAreaSelectionWidgetPrivate() : ui(new Ui::TabletAreaSelectionWidget) {}
            ~TabletAreaSelectionWidgetPrivate() {
                delete ui;
            }

            Ui::TabletAreaSelectionWidget* ui;
            QString                        deviceName;
    }; // PRIVATE CLASS
} // NAMESPACE


TabletAreaSelectionWidget::TabletAreaSelectionWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new TabletAreaSelectionWidgetPrivate)
{
    setupUi();
}


TabletAreaSelectionWidget::~TabletAreaSelectionWidget()
{
    delete this->d_ptr;
}


const QString TabletAreaSelectionWidget::getSelection() const
{
    Q_D(const TabletAreaSelectionWidget);

    if (d->ui->fullTabletRadioButton->isChecked()) {
        return QLatin1String("full");
    }

    return d->ui->areaWidget->getSelectionAsString();
}


void TabletAreaSelectionWidget::setSelection(const QString& selection)
{
    Q_D(const TabletAreaSelectionWidget);

    if (selection.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0) {

        // full screen selection
        setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);

    } else {
        // area or invalid selection
        QRect selectionRect = StringUtils::toQRect(selection);

        if (selectionRect.isEmpty()) {
            // invalid selection => full screen
            setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);

        } else {
            // area selection
            setTabletAreaType(TabletAreaSelectionWidget::PartialTabletArea);
            d->ui->areaWidget->setSelection(selectionRect);
        }
    }
}


void TabletAreaSelectionWidget::setupWidget(const QRect& tabletArea, const QList< QRect >& screenAreas, const QRect& screenAreaSelection, const QString& deviceName)
{
    Q_D(TabletAreaSelectionWidget);

    d->deviceName = deviceName;

    setupScreenArea(screenAreas, screenAreaSelection);
    setupTabletArea(tabletArea);
    setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);
}


void TabletAreaSelectionWidget::onCalibrateClicked()
{
    Q_D(TabletAreaSelectionWidget);

    CalibrationDialog calibDialog(d->deviceName);
    calibDialog.exec();

    QRect area = calibDialog.calibratedArea();
    setSelection(area);

    emit changed();
}


void TabletAreaSelectionWidget::onForceProportionsClicked()
{
    Q_D(TabletAreaSelectionWidget);

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

    setTabletAreaType(TabletAreaSelectionWidget::PartialTabletArea);
    setSelection(QRect(0, 0, qRound(newWidth), qRound(newHeight)));

    emit changed();
}


void TabletAreaSelectionWidget::onFullTabletSelected(bool checked)
{
    if (!checked) {
        return;
    }
    setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);
    emit changed();
}


void TabletAreaSelectionWidget::onTabletAreaChanged()
{
    emit changed();
}


void TabletAreaSelectionWidget::onTabletAreaSelected(bool checked)
{
    if (!checked) {
        return;
    }
    setTabletAreaType(TabletAreaSelectionWidget::PartialTabletArea);
    emit changed();
}


void TabletAreaSelectionWidget::setSelection(const QRect& selection)
{
    Q_D(TabletAreaSelectionWidget);

    d->ui->areaWidget->setSelection(selection);

    if (isFullAreaSelection(selection)) {
        setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);
    } else {
        setTabletAreaType(TabletAreaSelectionWidget::PartialTabletArea);
    }
}


void TabletAreaSelectionWidget::setTabletAreaType(TabletAreaSelectionWidget::TabletAreaType type)
{
    Q_D(TabletAreaSelectionWidget);

    d->ui->fullTabletRadioButton->blockSignals(true);
    d->ui->tabletAreaRadioButton->blockSignals(true);

    if (type == TabletAreaSelectionWidget::FullTabletArea) {
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



bool TabletAreaSelectionWidget::isFullAreaSelection(const QRect& selection) const
{
    Q_D (const TabletAreaSelectionWidget);

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


void TabletAreaSelectionWidget::setupScreenArea(const QList< QRect >& screenAreas, const QRect& screenAreaSelection)
{
    Q_D(TabletAreaSelectionWidget);

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


void TabletAreaSelectionWidget::setupTabletArea(const QRect& tabletArea)
{
    Q_D(TabletAreaSelectionWidget);

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


void TabletAreaSelectionWidget::setupUi()
{
    Q_D(TabletAreaSelectionWidget);

    d->ui->setupUi(this);

    setupTabletArea();
    setupScreenArea();

    connect( d->ui->areaWidget, SIGNAL(selectionChanged()), this, SLOT(onTabletAreaChanged()) );

    setTabletAreaType(TabletAreaSelectionWidget::FullTabletArea);
}


