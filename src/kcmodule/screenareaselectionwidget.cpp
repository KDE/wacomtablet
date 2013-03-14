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

#include "screenareaselectionwidget.h"
#include "ui_screenareaselectionwidget.h"

#include "stringutils.h"

#include <KDE/KLocalizedString>

#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QStringList>

using namespace Wacom;

namespace Wacom
{
    class ScreenAreaSelectionWidgetPrivate
    {
        public:
            ScreenAreaSelectionWidgetPrivate() : ui (new Ui::ScreenAreaSelectionWidget) {}
            ~ScreenAreaSelectionWidgetPrivate() {
                delete ui;
            }

            Ui::ScreenAreaSelectionWidget* ui;
            QList< QRect >                 screenAreas;

    }; // CLASS
} // NAMESPACE


ScreenAreaSelectionWidget::ScreenAreaSelectionWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new ScreenAreaSelectionWidgetPrivate)
{
    setupUi();
}


ScreenAreaSelectionWidget::~ScreenAreaSelectionWidget()
{
    delete this->d_ptr;
}


QString ScreenAreaSelectionWidget::getSelection() const
{
    Q_D(const ScreenAreaSelectionWidget);

    QString selection;

    if (d->ui->selectFullScreenRadio->isChecked()) {
        selection = QLatin1String("full");

    } else if (d->ui->selectMonitorRadio->isChecked()) {

        if (d->ui->selectMonitorCombo->count() == 0) {
            // combo box not initialized!
            selection = QLatin1String("full");

        } else {
            selection = d->ui->selectMonitorCombo->itemData(d->ui->selectMonitorCombo->currentIndex()).toString();
        }

    } else {
        selection = d->ui->screenArea->getSelectionAsString();
    }

    return selection;
}


void ScreenAreaSelectionWidget::setSelection(const QString& selection)
{
    Q_D(ScreenAreaSelectionWidget);

    QRegExp monitorRegExp(QLatin1String("map(\\d+)"), Qt::CaseInsensitive);

    if (selection.compare(QLatin1String("full"), Qt::CaseInsensitive) == 0 ) {
        // full screen selection
        setScreenAreaType(ScreenAreaSelectionWidget::FullScreenArea);

    } else if (monitorRegExp.indexIn(selection, 0) != -1) {
        // monitor selection
        setMonitorSelection(monitorRegExp.cap(1).toInt());

    } else {
        // partial or invalid selection
        QRect rectSelection = StringUtils::toQRect(selection, true);

        if (!rectSelection.isValid()) {
            setScreenAreaType(ScreenAreaSelectionWidget::FullScreenArea);
        } else {
            setScreenAreaType(ScreenAreaSelectionWidget::PartialScreenArea);
            d->ui->screenArea->setSelection(rectSelection);
        }
    }
}


void ScreenAreaSelectionWidget::setupWidget(const QList< QRect >& screenAreas, const QRect& tabletArea, const QRect& selectedTabletArea, const QString& tabletAreaCaption)
{
    setupTabletArea(tabletArea, selectedTabletArea, tabletAreaCaption);
    setupScreenArea(screenAreas);
    setupMonitorComboBox(screenAreas);
    setScreenAreaType(ScreenAreaSelectionWidget::FullScreenArea);
}


void ScreenAreaSelectionWidget::onFullScreenSelected(bool checked)
{
    if (checked) {
        setScreenAreaType(ScreenAreaSelectionWidget::FullScreenArea);
        emit changed();
    }
}


void ScreenAreaSelectionWidget::onMonitorScreenSelected()
{
    Q_D(const ScreenAreaSelectionWidget);
    setMonitorSelection(d->ui->selectMonitorCombo->currentIndex());
    emit changed();
}


void ScreenAreaSelectionWidget::onMonitorSelected(bool checked)
{
    if (checked) {
        setScreenAreaType(ScreenAreaSelectionWidget::MonitorArea);
        emit changed();
    }
}


void ScreenAreaSelectionWidget::onPartialScreenSelected(bool checked)
{
    if (checked) {
        setScreenAreaType(ScreenAreaSelectionWidget::PartialScreenArea);
        emit changed();
    }
}


void ScreenAreaSelectionWidget::setMonitorSelection(const int screenNum)
{
    Q_D (ScreenAreaSelectionWidget);

    if (d->screenAreas.isEmpty() || d->ui->selectMonitorCombo->count() == 0) {
        return; // no screens available
    }

    // setScreenAreaType() passes -1 to signal that the default/current selection should be set
    int selectScreen = (screenNum < 0) ? d->ui->selectMonitorCombo->currentIndex() : screenNum;

    if ( selectScreen < 0 || selectScreen >= d->ui->selectMonitorCombo->count() || selectScreen >= d->screenAreas.count() ) {
        selectScreen = 0; // invalid selection, use default
    }

    // select monitor
    d->ui->selectMonitorCombo->blockSignals(true);
    d->ui->selectMonitorCombo->setCurrentIndex(selectScreen);
    d->ui->selectMonitorCombo->blockSignals(false);

    d->ui->screenArea->setEnabled(false);
    d->ui->screenArea->setSelection(d->screenAreas.at(selectScreen));

    if (screenNum >= 0) {
        // only set area type if a valid monitor was selected or
        // else we will end up in an endless loop as setScreenAreaType()
        // passes -1 to this method.
        setScreenAreaType(ScreenAreaSelectionWidget::MonitorArea);
    }
}



void ScreenAreaSelectionWidget::setScreenAreaType(ScreenAreaSelectionWidget::ScreenAreaType areaType)
{
    Q_D (ScreenAreaSelectionWidget);

    d->ui->selectFullScreenRadio->blockSignals(true);
    d->ui->selectMonitorRadio->blockSignals(true);
    d->ui->selectAreaRadio->blockSignals(true);

    switch (areaType) {

        case ScreenAreaSelectionWidget::FullScreenArea:
            d->ui->selectMonitorCombo->setEnabled(false);
            d->ui->selectMonitorRadio->setChecked(false);
            d->ui->selectAreaRadio->setChecked(false);
            d->ui->selectFullScreenRadio->setChecked(true);

            d->ui->screenArea->setEnabled(false);
            d->ui->screenArea->clearSelection();
            break;

        case ScreenAreaSelectionWidget::MonitorArea:
            d->ui->selectFullScreenRadio->setChecked(false);
            d->ui->selectAreaRadio->setChecked(false);
            d->ui->selectMonitorRadio->setChecked(true);
            d->ui->selectMonitorCombo->setEnabled(true);
            setMonitorSelection(-1);
            break;

        case ScreenAreaSelectionWidget::PartialScreenArea:
            d->ui->selectFullScreenRadio->setChecked(false);
            d->ui->selectMonitorCombo->setEnabled(false);
            d->ui->selectMonitorRadio->setChecked(false);
            d->ui->selectAreaRadio->setChecked(true);

            d->ui->screenArea->setEnabled(true);
            break;
    }

    d->ui->selectFullScreenRadio->blockSignals(false);
    d->ui->selectMonitorRadio->blockSignals(false);
    d->ui->selectAreaRadio->blockSignals(false);
}


void ScreenAreaSelectionWidget::onScreenAreaChanged()
{
    emit changed();
}


void ScreenAreaSelectionWidget::setupMonitorComboBox(const QList< QRect >& screenAreas)
{
    Q_D(ScreenAreaSelectionWidget);

    d->screenAreas = screenAreas;
    d->ui->selectMonitorCombo->clear();

    QString itemName;
    QString itemData;

    for (int i = 0 ; i < screenAreas.size() ; ++i) {

        itemName = i18nc("Maps the tablet to the X11 screen #.", "Screen %1", i + 1);
        itemData = QString::fromLatin1("map%1").arg(i);

        d->ui->selectMonitorCombo->addItem(itemName, itemData);
    }
}



void ScreenAreaSelectionWidget::setupScreenArea(const QList< QRect >& screenAreas)
{
    Q_D(ScreenAreaSelectionWidget);

    d->ui->screenArea->setWidgetTargetSize(QSize(400,400));

    if (screenAreas.size() > 0) {
        d->ui->screenArea->setDrawAreaCaptions(true);
        d->ui->screenArea->setDrawSelectionCaption(true);

        QStringList captions;

        for (int i = 0 ; i < screenAreas.size() ; ++i) {
            captions.append(QString::number(i+1));
        }

        d->ui->screenArea->setAreas(screenAreas, captions);

    } else {
        d->ui->screenArea->setDrawAreaCaptions(true);
        d->ui->screenArea->setDrawSelectionCaption(false);

        d->ui->screenArea->setArea(QRect(0, 0, 1920, 1200), i18n("Internal Error"));
    }
}


void ScreenAreaSelectionWidget::setupTabletArea(const QRect& tabletArea, const QRect& selectedTabletArea, const QString& tabletAreaCaption)
{
    Q_D(ScreenAreaSelectionWidget);

    d->ui->tabletArea->setWidgetTargetSize(QSize(150,150));
    d->ui->tabletArea->setFont(QFont(QLatin1String("sans"), 8));
    d->ui->tabletArea->setOutOfBoundsMargin(.1);
    d->ui->tabletArea->setEnabled(false);

    if (!tabletArea.isValid()) {
        d->ui->tabletArea->setDrawAreaCaptions(true);
        d->ui->tabletArea->setDrawSelectionCaption(false);
        d->ui->tabletArea->setArea(QRect(0,0,1920,1200), i18n("Internal Error"));

    } else {
        d->ui->tabletArea->setDrawAreaCaptions(true);
        d->ui->tabletArea->setDrawSelectionCaption(true);
        d->ui->tabletArea->setArea(tabletArea, tabletAreaCaption);
        d->ui->tabletArea->setSelection(selectedTabletArea);
    }
}


void ScreenAreaSelectionWidget::setupUi()
{
    Q_D(ScreenAreaSelectionWidget);

    d->ui->setupUi(this);

    setupTabletArea();
    setupScreenArea();

    connect( d->ui->screenArea, SIGNAL(selectionChanged()), this, SLOT(onScreenAreaChanged()) );

    setScreenAreaType(ScreenAreaSelectionWidget::FullScreenArea);
}
