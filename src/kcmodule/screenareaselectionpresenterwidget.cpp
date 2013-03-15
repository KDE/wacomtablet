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

#include "screenareaselectionpresenterwidget.h"
#include "screenareaselectioncontroller.h"
#include "screenareaselectionwidget.h"

#include <QtGui/QHBoxLayout>

using namespace Wacom;

namespace Wacom {
    class ScreenAreaSelectionPresenterWidgetPrivate {
        public:
            ScreenAreaSelectionController controller;
            ScreenAreaSelectionWidget*    widget;     // no need to delete as it is properly parented
    };
}

ScreenAreaSelectionPresenterWidget::ScreenAreaSelectionPresenterWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new ScreenAreaSelectionPresenterWidgetPrivate)
{
    Q_D(ScreenAreaSelectionPresenterWidget);

    setupUi();
    d->controller.setWidget(d->widget);
}


ScreenAreaSelectionPresenterWidget::~ScreenAreaSelectionPresenterWidget()
{
    delete this->d_ptr;
}


const QString ScreenAreaSelectionPresenterWidget::getSelection()
{
    Q_D(const ScreenAreaSelectionPresenterWidget);

    return d->controller.getSelection();
}


void ScreenAreaSelectionPresenterWidget::setSelection(const QString& selection)
{
    Q_D(ScreenAreaSelectionPresenterWidget);

    d->controller.setSelection(selection);
}


void ScreenAreaSelectionPresenterWidget::setupWidget(const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName)
{
    Q_D(ScreenAreaSelectionPresenterWidget);

    d->controller.setupController(screenSelection, tabletSelection, tabletCaption, deviceName);
}


void ScreenAreaSelectionPresenterWidget::setupUi()
{
    Q_D(ScreenAreaSelectionPresenterWidget);

    d->widget = new ScreenAreaSelectionWidget(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(d->widget);

    QWidget::setLayout(layout);

    connect( d->widget, SIGNAL(signalFullScreenSelected()), &(d->controller), SLOT(onFullScreenSelected()) );
    connect( d->widget, SIGNAL(signalMonitorSelected(int)), &(d->controller), SLOT(onMonitorSelected(int)) );
}
