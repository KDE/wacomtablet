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

#include "tabletareaselectiondialog.h"
#include "tabletareaselectionwidget.h"

#include "stringutils.h"
#include "x11info.h"

#include <QtCore/QRegExp>

#include <KDE/KLocalizedString>

using namespace Wacom;

namespace Wacom
{
    class TabletAreaSelectionDialogPrivate
    {
        public:
            TabletAreaSelectionWidget* selectionWidget; // no need to delete this widget as it is properly parented.
    }; // PRIVATE CLASS
} // NAMESPACE


TabletAreaSelectionDialog::TabletAreaSelectionDialog()
        : KDialog(NULL), d_ptr(new TabletAreaSelectionDialogPrivate)
{
    setupUi();
}


TabletAreaSelectionDialog::~TabletAreaSelectionDialog()
{
    delete this->d_ptr;
}


const QString TabletAreaSelectionDialog::getMappings() const
{
    Q_D(const TabletAreaSelectionDialog);

    return d->selectionWidget->getMappings();
}


const QString TabletAreaSelectionDialog::getScreenSpace() const
{
    Q_D(const TabletAreaSelectionDialog);

    return d->selectionWidget->getScreenSpace();
}



void TabletAreaSelectionDialog::select(int screenNumber)
{
    Q_D(TabletAreaSelectionDialog);

    d->selectionWidget->select(screenNumber);
}


void TabletAreaSelectionDialog::select(const QString& screenSpace)
{
    Q_D(TabletAreaSelectionDialog);

    d->selectionWidget->select(screenSpace);
}


void TabletAreaSelectionDialog::setupWidget(const QString& mappings, const QString& deviceName)
{
    Q_D(TabletAreaSelectionDialog);

    d->selectionWidget->setupWidget(mappings, deviceName);
}



void TabletAreaSelectionDialog::setupUi()
{
    Q_D(TabletAreaSelectionDialog);

    d->selectionWidget = new TabletAreaSelectionWidget(this);

    setMainWidget(d->selectionWidget);
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18nc( "Dialog title from a dialog which lets the user select an area of the tablet where the screen space will be mapped to.", "Select a Tablet Area" ) );
    setWindowIcon( KIcon( QLatin1String("input-tablet") ) );

    //connect( this, SIGNAL(okClicked()), this, SLOT(onOkClicked()) );
}
