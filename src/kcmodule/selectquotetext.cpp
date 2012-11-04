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

#include "selectquotetext.h"
#include "ui_selectquotetext.h"

using namespace Wacom;

SelectQuoteText::SelectQuoteText(QWidget *parent) :
        KDialog(parent),
        ui(new Ui::SelectQuoteText)
{
    QWidget *widget = new QWidget(this);
    ui->setupUi(widget);
    setMainWidget(widget);

    setButtons(KDialog::Ok | KDialog::Cancel);
    setCaption(i18n("Select Quote Text"));

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()));
}

SelectQuoteText::~SelectQuoteText()
{
    delete ui;
}

QString SelectQuoteText::quoteText() const 
{
    return m_quoteText;
}

void SelectQuoteText::slotOkClicked()
{
    m_quoteText = ui->klineedit->text();
}
