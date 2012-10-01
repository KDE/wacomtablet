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

SelectKeyButton::SelectKeyButton(QWidget *parent) :
        KDialog(parent),
        ui(new Ui::SelectKeyButton)
{
    QWidget *widget = new QWidget(this);
    ui->setupUi(widget);
    setMainWidget(widget);

    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("Select Key Button"));

    for (int i = 1;i < 33;++i) {
        ui->kcombobox->addItem(i18nc("Pad Button action", "Button %1", i), i);
    }

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()));

}

SelectKeyButton::~SelectKeyButton()
{
    delete ui;
}

QString SelectKeyButton::keyButton() const
{
    return m_keyButton;
}

void SelectKeyButton::slotOkClicked()
{
    // we need "Button i" as text for the wacom driver
    // but the text is translated into something else
    // so the current index will be transferred back to the original text
    m_keyButton = QString::fromLatin1("Button %1").arg(ui->kcombobox->currentIndex() + 1);
}
