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

#include "buttonactionselectiondialog.h"

#include "buttonshortcut.h"
#include "buttonactionselectionwidget.h"

#include <KDE/KLocalizedString>

using namespace Wacom;

namespace Wacom {
    class ButtonActionSelectionDialogPrivate {
        public:
            // no need to delete this as it is properly parented
            ButtonActionSelectionWidget* selectionWidget;
    };
}

ButtonActionSelectionDialog::ButtonActionSelectionDialog(QWidget* parent)
    : KDialog(parent), d_ptr(new ButtonActionSelectionDialogPrivate)
{
    Q_D (ButtonActionSelectionDialog);

    d->selectionWidget = new ButtonActionSelectionWidget(this);

    setMainWidget(d->selectionWidget);
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18nc( "The action that will be assigned to a tablet button.", "Select Button Action" ) );
}


ButtonActionSelectionDialog::~ButtonActionSelectionDialog()
{
    delete this->d_ptr;
}


const ButtonShortcut& ButtonActionSelectionDialog::getShortcut() const
{
    Q_D (const ButtonActionSelectionDialog);
    return d->selectionWidget->getShortcut();
}


void ButtonActionSelectionDialog::setShortcut(const ButtonShortcut& shortcut)
{
    Q_D (ButtonActionSelectionDialog);
    d->selectionWidget->setShortcut(shortcut);
}
