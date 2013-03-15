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
#include "screenareaselectioncontroller.h"
#include "screenareaselectionview.h"

#include <QtGui/QHBoxLayout>

using namespace Wacom;

namespace Wacom {
    class ScreenAreaSelectionWidgetPrivate {
        public:
            ScreenAreaSelectionController controller;
    };
}

ScreenAreaSelectionWidget::ScreenAreaSelectionWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new ScreenAreaSelectionWidgetPrivate)
{
    setupUi();
}


ScreenAreaSelectionWidget::~ScreenAreaSelectionWidget()
{
    delete this->d_ptr;
}


const QString ScreenAreaSelectionWidget::getSelection()
{
    Q_D(const ScreenAreaSelectionWidget);

    return d->controller.getSelection();
}


void ScreenAreaSelectionWidget::setSelection(const QString& selection)
{
    Q_D(ScreenAreaSelectionWidget);

    d->controller.setSelection(selection);
}


void ScreenAreaSelectionWidget::setupWidget(const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName)
{
    Q_D(ScreenAreaSelectionWidget);

    d->controller.setupController(screenSelection, tabletSelection, tabletCaption, deviceName);
}


void ScreenAreaSelectionWidget::setupUi()
{
    Q_D(ScreenAreaSelectionWidget);

    // setup widget
    ScreenAreaSelectionView* view = new ScreenAreaSelectionView(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(view);

    QWidget::setLayout(layout);

    // setup controller
    d->controller.setView(view);
    connect( view, SIGNAL(signalFullScreenSelected()), &(d->controller), SLOT(onFullScreenSelected()) );
    connect( view, SIGNAL(signalMonitorSelected(int)), &(d->controller), SLOT(onMonitorSelected(int)) );
}
