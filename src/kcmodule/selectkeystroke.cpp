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

#include "selectkeystroke.h"
#include "ui_selectkeystroke.h"
#include "buttonshortcut.h"

#include <KDE/KGlobalAccel>
#include <kglobalshortcutinfo.h>

#include <QtCore/QString>

using namespace Wacom;


namespace Wacom {
    class SelectKeyStrokePrivate {
        public:
            SelectKeyStrokePrivate() : ui( new Ui::SelectKeyStroke ) {}
            ~SelectKeyStrokePrivate() {
                delete ui;
            }

            Ui::SelectKeyStroke *ui;
            ButtonShortcut      shortcut;
    };
}


SelectKeyStroke::SelectKeyStroke( QWidget *parent ) :
        KDialog( parent ),
        d_ptr(new SelectKeyStrokePrivate)
{
    setupUi();
}

SelectKeyStroke::~SelectKeyStroke()
{
    delete this->d_ptr;
}

QString SelectKeyStroke::keyStroke() const
{
    Q_D (const SelectKeyStroke);
    return d->shortcut.toString();
}

const ButtonShortcut& SelectKeyStroke::shortcut() const
{
    Q_D (const SelectKeyStroke);
    return d->shortcut;
}


void SelectKeyStroke::slotOkClicked()
{
    Q_D (SelectKeyStroke);

    if( d->ui->comboBox->currentIndex() != 0 ) {
        int index = d->ui->comboBox->currentIndex();
        d->shortcut = d->ui->comboBox->itemData( index ).toString();
    }
    else {
        d->shortcut = d->ui->kkeysequencewidget->keySequence().toString();
    }
}

void SelectKeyStroke::findGlobalShortcut( QKeySequence sequence )
{
    Q_D (SelectKeyStroke);

    QList< KGlobalShortcutInfo > list = KGlobalAccel::getGlobalShortcutsByKey( sequence );

    if( !list.isEmpty() ) {
        d->ui->globalShortcut->setText( list.at( 0 ).uniqueName() );
    }
    else {
        d->ui->globalShortcut->clear();
    }
}


void SelectKeyStroke::setupUi()
{
    Q_D (SelectKeyStroke);

    QWidget *widget = new QWidget( this );
    d->ui->setupUi( widget );

    d->ui->kkeysequencewidget->setCheckForConflictsAgainst( KKeySequenceWidget::None );
    d->ui->kkeysequencewidget->setModifierlessAllowed( true );

    //fill the combobox with all values
    d->ui->comboBox->blockSignals( true );
    d->ui->comboBox->addItem( i18nc( "none means no special modifier is used. instead use the values from the Key Sequence Widget", "none" ), QString::fromLatin1( "none" ) );
    d->ui->comboBox->addItem( i18nc( "ALT key", "ALT" ), QString::fromLatin1( "ALT" ) );
    d->ui->comboBox->addItem( i18nc( "CTRL key", "CTRL" ), QString::fromLatin1( "CTRL" ) );
    d->ui->comboBox->addItem( i18nc( "SHIFT key", "SHIFT" ), QString::fromLatin1( "SHIFT" ) );
    d->ui->comboBox->addItem( i18nc( "META key", "META" ), QString::fromLatin1( "META" ) );
    d->ui->comboBox->addItem( i18nc( "ALT+CTRL key combination", "ALT+CTRL" ), QString::fromLatin1( "ALT+CTRL" ) );
    d->ui->comboBox->addItem( i18nc( "ALT+SHIFT key combination", "ALT+SHIFT" ), QString::fromLatin1( "ALT+SHIFT" ) );
    d->ui->comboBox->addItem( i18nc( "ALT+META key combination", "ALT+META" ), QString::fromLatin1( "ALT+META" ) );
    d->ui->comboBox->addItem( i18nc( "CTRL+SHIFT key combination", "CTRL+SHIFT" ), QString::fromLatin1( "CTRL+SHIFT" ) );
    d->ui->comboBox->addItem( i18nc( "CTRL+META key combination", "CTRL+META" ), QString::fromLatin1( "CTRL+META" ) );
    d->ui->comboBox->addItem( i18nc( "SHIFT+META key combination", "SHIFT+META" ), QString::fromLatin1( "SHIFT+META" ) );
    d->ui->comboBox->addItem( i18nc( "ALT+CTRL+META key combination", "ALT+CTRL+META" ), QString::fromLatin1( "ALT+CTRL+META" ) );
    d->ui->comboBox->addItem( i18nc( "ALT+CTRL+SHIFT key combination", "ALT+CTRL+SHIFT" ), QString::fromLatin1( "ALT+CTRL+SHIFT" ) );
    d->ui->comboBox->addItem( i18nc( "CTRL+META+SHIFT key combination", "CTRL+META+SHIFT" ), QString::fromLatin1( "CTRL+META+SHIFT" ) );
    d->ui->comboBox->blockSignals( false );

    setMainWidget( widget );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18n( "Select Key Function" ) );

    connect( this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()) );
    connect( d->ui->kkeysequencewidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(findGlobalShortcut(QKeySequence)) );
}
