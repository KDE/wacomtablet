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

#include <KDE/KGlobalAccel>
#include <kglobalshortcutinfo.h>

#include <QtCore/QString>

using namespace Wacom;

SelectKeyStroke::SelectKeyStroke( QWidget *parent ) :
    KDialog( parent ),
    ui( new Ui::SelectKeyStroke )
{
    QWidget *widget = new QWidget( this );
    ui->setupUi( widget );
    setMainWidget( widget );

    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18n( "Select Key Function" ) );

    ui->kkeysequencewidget->setCheckForConflictsAgainst( KKeySequenceWidget::None );
    ui->kkeysequencewidget->setModifierlessAllowed( true );

    //fill the combobox with all values
    ui->comboBox->blockSignals( true );
    ui->comboBox->addItem( i18nc( "none means no special modifier is used. instead use the values from the Key Sequence Widget", "none" ), QString::fromLatin1( "none" ) );
    ui->comboBox->addItem( i18nc( "ALT key", "ALT" ), QString::fromLatin1( "ALT" ) );
    ui->comboBox->addItem( i18nc( "CTRL key", "CTRL" ), QString::fromLatin1( "CTRL" ) );
    ui->comboBox->addItem( i18nc( "SHIFT key", "SHIFT" ), QString::fromLatin1( "SHIFT" ) );
    ui->comboBox->addItem( i18nc( "META key", "META" ), QString::fromLatin1( "META" ) );
    ui->comboBox->addItem( i18nc( "ALT+CTRL key combination", "ALT+CTRL" ), QString::fromLatin1( "ALT+CTRL" ) );
    ui->comboBox->addItem( i18nc( "ALT+SHIFT key combination", "ALT+SHIFT" ), QString::fromLatin1( "ALT+SHIFT" ) );
    ui->comboBox->addItem( i18nc( "ALT+META key combination", "ALT+META" ), QString::fromLatin1( "ALT+META" ) );
    ui->comboBox->addItem( i18nc( "CTRL+SHIFT key combination", "CTRL+SHIFT" ), QString::fromLatin1( "CTRL+SHIFT" ) );
    ui->comboBox->addItem( i18nc( "CTRL+META key combination", "CTRL+META" ), QString::fromLatin1( "CTRL+META" ) );
    ui->comboBox->addItem( i18nc( "SHIFT+META key combination", "SHIFT+META" ), QString::fromLatin1( "SHIFT+META" ) );
    ui->comboBox->addItem( i18nc( "ALT+CTRL+META key combination", "ALT+CTRL+META" ), QString::fromLatin1( "ALT+CTRL+META" ) );
    ui->comboBox->addItem( i18nc( "ALT+CTRL+SHIFT key combination", "ALT+CTRL+SHIFT" ), QString::fromLatin1( "ALT+CTRL+SHIFT" ) );
    ui->comboBox->addItem( i18nc( "CTRL+META+SHIFT key combination", "CTRL+META+SHIFT" ), QString::fromLatin1( "CTRL+META+SHIFT" ) );
    ui->comboBox->blockSignals( false );

    connect( this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()) );
    connect( ui->kkeysequencewidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(findGlobalShortcut(QKeySequence)) );
}

SelectKeyStroke::~SelectKeyStroke()
{
    delete ui;
}

QString SelectKeyStroke::keyStroke() const
{
    return m_keyStroke;
}

void SelectKeyStroke::slotOkClicked()
{

    if( ui->comboBox->currentIndex() != 0 ) {
        int index = ui->comboBox->currentIndex();
        m_keyStroke = ui->comboBox->itemData( index ).toString();
    }
    else {
        m_keyStroke = ui->kkeysequencewidget->keySequence().toString();
    }

    m_keyStroke.replace( QRegExp( QLatin1String( "([^\\s])\\+" ) ), QLatin1String( "\\1 " ) );
    m_keyStroke = m_keyStroke.toLower();
}

void SelectKeyStroke::findGlobalShortcut( QKeySequence sequence )
{
    QList< KGlobalShortcutInfo > list = KGlobalAccel::getGlobalShortcutsByKey( sequence );

    if( !list.isEmpty() ) {
        ui->globalShortcut->setText( list.at( 0 ).uniqueName() );
    }
    else {
        ui->globalShortcut->clear();
    }
}
