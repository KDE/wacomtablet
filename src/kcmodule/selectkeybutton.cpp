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

#include "selectkeybutton.h"
#include "ui_selectkeybutton.h"

using namespace Wacom;

namespace Wacom {
    class SelectKeyButtonPrivate {
        public:
            SelectKeyButtonPrivate() : ui(new Ui::SelectKeyButton) {
                button = 0;
            }

            ~SelectKeyButtonPrivate() {
                delete ui;
            }

            Ui::SelectKeyButton* ui;
            int                  button;
    };
}

SelectKeyButton::SelectKeyButton(QWidget *parent) :
        KDialog(parent),
        d_ptr(new SelectKeyButtonPrivate)
{
    setupUi();
}


SelectKeyButton::~SelectKeyButton()
{
    delete this->d_ptr;
}


int SelectKeyButton::getButton() const
{
    Q_D( const SelectKeyButton );
    return d->button;
}


void SelectKeyButton::setButton(int buttonNumber)
{
    Q_D( SelectKeyButton );

    int selection = d->ui->kcombobox->findData(buttonNumber);

    if (selection != -1) {
        d->button = buttonNumber;

        d->ui->kcombobox->blockSignals(true);
        d->ui->kcombobox->setCurrentIndex(selection);
        d->ui->kcombobox->blockSignals(false);
    }
}


void SelectKeyButton::onOkClicked()
{
    Q_D (SelectKeyButton);
    d->button = d->ui->kcombobox->itemData(d->ui->kcombobox->currentIndex()).toInt();
}


void SelectKeyButton::setupUi()
{
    Q_D(SelectKeyButton);

    QWidget *widget = new QWidget(this);
    d->ui->setupUi(widget);

    // populate dropdown
    d->ui->kcombobox->addItem(i18nc("Left mouse button click.",   "Left Click"),       1);
    d->ui->kcombobox->addItem(i18nc("Middle mouse button click.", "Middle Click"),     2);
    d->ui->kcombobox->addItem(i18nc("Right mouse button click.",  "Right Click"),      3);
    d->ui->kcombobox->addItem(i18nc("Mouse wheel up.",            "Mouse Wheel Up"),   4);
    d->ui->kcombobox->addItem(i18nc("Mouse wheel down.",          "Mouse Wheel Down"), 5);

    for (int i = 6 ; i < 33 ; ++i) {
        d->ui->kcombobox->addItem(i18nc("Extra mouse button click.", "Extra Button %1", i), i);
    }

    // initialize KDialog
    setCaption(i18n("Select a Mouse Button"));
    setMainWidget(widget);
    setButtons(KDialog::Ok | KDialog::Cancel);

    // connect signals
    connect(this, SIGNAL(okClicked()), this, SLOT(onOkClicked()));
}
