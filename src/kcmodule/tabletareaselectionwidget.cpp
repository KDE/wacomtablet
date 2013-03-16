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
#include "tabletareaselectioncontroller.h"
#include "tabletareaselectionview.h"

#include <QtGui/QHBoxLayout>


using namespace Wacom;

namespace Wacom
{
    class TabletAreaSelectionWidgetPrivate
    {
        public:
            TabletAreaSelectionController controller;
    };
}


TabletAreaSelectionWidget::TabletAreaSelectionWidget(QWidget* parent)
        : QWidget(parent), d_ptr(new TabletAreaSelectionWidgetPrivate)
{
    setupUi();
}


TabletAreaSelectionWidget::~TabletAreaSelectionWidget()
{
    delete this->d_ptr;
}


const QString TabletAreaSelectionWidget::getMappings() const
{
    Q_D(const TabletAreaSelectionWidget);

    return d->controller.getMappings();
}


const QString TabletAreaSelectionWidget::getScreenSpace() const
{
    Q_D(const TabletAreaSelectionWidget);

    return d->controller.getScreenSpace();
}


void TabletAreaSelectionWidget::select(int screenNumber)
{
    Q_D(TabletAreaSelectionWidget);

    d->controller.select(screenNumber);
}


void TabletAreaSelectionWidget::select(const QString& screenSpace)
{
    Q_D(TabletAreaSelectionWidget);

    d->controller.select(screenSpace);
}


void TabletAreaSelectionWidget::setupWidget(const QString& mappings, const QString& deviceName)
{
    Q_D(TabletAreaSelectionWidget);

    d->controller.setupController(mappings, deviceName);
}


void TabletAreaSelectionWidget::setupUi()
{
    Q_D(TabletAreaSelectionWidget);

    // setup view
    TabletAreaSelectionView* view = new TabletAreaSelectionView(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(view);

    QWidget::setLayout(layout);

    // setup controller
    d->controller.setView(view);

    // TODO connect signals
}
