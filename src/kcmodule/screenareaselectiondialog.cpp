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

#include "screenareaselectiondialog.h"
#include "screenareaselectionpresenterwidget.h"

#include "stringutils.h"
#include "x11info.h"

#include <KDE/KLocalizedString>

using namespace Wacom;

namespace Wacom
{
    class ScreenAreaSelectionDialogPrivate
    {
        public:
            ScreenAreaSelectionPresenterWidget* selectionPresenter; // no need to delete this widget as it is properly parented.
    }; // CLASS
} // NAMESPACE


ScreenAreaSelectionDialog::ScreenAreaSelectionDialog(QWidget* parent)
        : KDialog(parent), d_ptr(new ScreenAreaSelectionDialogPrivate)
{
    setupUi();
}


ScreenAreaSelectionDialog::~ScreenAreaSelectionDialog()
{
    delete this->d_ptr;
}


QString ScreenAreaSelectionDialog::getSelection() const
{
    Q_D(const ScreenAreaSelectionDialog);

    return d->selectionPresenter->getSelection();
}


void ScreenAreaSelectionDialog::setSelection(const QString& selection)
{
    Q_D(ScreenAreaSelectionDialog);

    d->selectionPresenter->setSelection(selection);
}


void ScreenAreaSelectionDialog::setupWidget(const QString& screenSelection, const QString& tabletSelection, const QString& tabletCaption, const QString& deviceName)
{
    Q_D(ScreenAreaSelectionDialog);

    d->selectionPresenter->setupWidget(screenSelection, tabletSelection, tabletCaption, deviceName);
}


void ScreenAreaSelectionDialog::setupUi()
{
    Q_D(ScreenAreaSelectionDialog);

    d->selectionPresenter = new ScreenAreaSelectionPresenterWidget(this);

    setMainWidget(d->selectionPresenter);
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18nc( "Dialog title from a dialog which lets the user select an area of the screen where the tablet space will be mapped to.", "Select a Screen Area" ) );

    //connect( this, SIGNAL(okClicked()), this, SLOT(onOkClicked()) );
}
