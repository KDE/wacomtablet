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

            QWidget             *mainWidget; // does not need to be deleted as it is properly parented.
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

const ButtonShortcut& SelectKeyStroke::getShortcut() const
{
    Q_D (const SelectKeyStroke);
    return d->shortcut;
}


void SelectKeyStroke::setShortcut(const ButtonShortcut& shortcut)
{
    Q_D (SelectKeyStroke);

    d->shortcut = shortcut;

    updateModifierWidgets(shortcut);
    updateShortcutWidgets(shortcut);
    updateActionName(shortcut);
}


void SelectKeyStroke::onModifierChanged(int state)
{
    Q_D (const SelectKeyStroke);

    // prevent compiler warning about unused parameters at least in debug mode
    assert(state != 0 || state == 0);

    // build new shortcut sequence
    QString shortcutString;

    if (d->ui->ctrlModifierCheckBox->isChecked()) {
        shortcutString.append(QString::fromLatin1(" %1").arg(QLatin1String(" Ctrl")));
    }

    if (d->ui->altModifierCheckBox->isChecked()) {
        shortcutString.append(QString::fromLatin1(" %1").arg(QLatin1String(" Alt")));
    }

    if (d->ui->metaModifierCheckBox->isChecked()) {
        shortcutString.append(QString::fromLatin1(" %1").arg(QLatin1String(" Meta")));
    }

    if (d->ui->shiftModifierCheckBox->isChecked()) {
        shortcutString.append(QString::fromLatin1(" %1").arg(QLatin1String(" Shift")));
    }

    setShortcut(ButtonShortcut(shortcutString));
}


void SelectKeyStroke::onModifiersCleared(bool checked)
{
    // prevent compiler warning about unused parameters at least in debug mode
    assert(checked || !checked);

    setShortcut(ButtonShortcut());
}



void SelectKeyStroke::onShortcutChanged(QKeySequence sequence)
{
    setShortcut(ButtonShortcut(sequence.toString()));
}



void SelectKeyStroke::setupUi()
{
    Q_D (SelectKeyStroke);

    // setup main widget
    d->mainWidget = new QWidget( this );
    d->ui->setupUi( d->mainWidget );

    connect( d->ui->ctrlModifierCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifierChanged(int)) );
    connect( d->ui->altModifierCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifierChanged(int)) );
    connect( d->ui->metaModifierCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifierChanged(int)) );
    connect( d->ui->shiftModifierCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onModifierChanged(int)) );

    connect( d->ui->modifierClearButton, SIGNAL(clicked(bool)), this, SLOT(onModifiersCleared(bool)) );

    connect( d->ui->shortcutSelectorWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(onShortcutChanged(QKeySequence)) );

    // setup kdialog
    setMainWidget( d->mainWidget );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setCaption( i18n( "Select Keyboard Action" ) );

    setShortcut(ButtonShortcut());
}


void SelectKeyStroke::updateActionName(const ButtonShortcut& shortcut)
{
    Q_D(SelectKeyStroke);

    QString displayName;

    if (!shortcut.isSet()) {
        displayName = i18n("No action set.");

    } else if (shortcut.isKeystroke()) {
        // lookup keystrokes from the global list of shortcuts
        QList< KGlobalShortcutInfo > globalShortcutList = KGlobalAccel::getGlobalShortcutsByKey(QKeySequence(shortcut.toQKeySequenceString()));

        if(!globalShortcutList.isEmpty()) {
            displayName = globalShortcutList.at(0).uniqueName();
        }
    }

    // set default display name if we do not have one yet
    if (displayName.isEmpty()) {
        displayName = shortcut.toDisplayString();
    }

    d->ui->actionNameLabel->setText(displayName);
}


void SelectKeyStroke::updateModifierWidgets(const ButtonShortcut& shortcut)
{
    Q_D(SelectKeyStroke);

    if (shortcut.isModifier()) {
        // shortcut is a modifier sequence - set checkboxes accordingly
        QString shortcutString = shortcut.toString();
        bool    isChecked      = false;

        isChecked = shortcutString.contains(QLatin1String("ctrl"), Qt::CaseInsensitive);
        updateQCheckBox(*(d->ui->ctrlModifierCheckBox),  isChecked, true);

        isChecked = shortcutString.contains(QLatin1String("alt"), Qt::CaseInsensitive);
        updateQCheckBox(*(d->ui->altModifierCheckBox),   isChecked, true);

        isChecked = (shortcutString.contains(QLatin1String("super"), Qt::CaseInsensitive) || shortcutString.contains(QLatin1String("meta"), Qt::CaseInsensitive));
        updateQCheckBox(*(d->ui->metaModifierCheckBox),  isChecked, true);

        isChecked = shortcutString.contains(QLatin1String("shift"), Qt::CaseInsensitive);
        updateQCheckBox(*(d->ui->shiftModifierCheckBox), isChecked, true);

    } else {
        // shortcut is unknown or not set, all widgets are available but not set
        updateQCheckBox(*(d->ui->ctrlModifierCheckBox),  false, true);
        updateQCheckBox(*(d->ui->altModifierCheckBox),   false, true);
        updateQCheckBox(*(d->ui->metaModifierCheckBox),  false, true);
        updateQCheckBox(*(d->ui->shiftModifierCheckBox), false, true);
    }
}


void SelectKeyStroke::updateQCheckBox(QCheckBox& checkbox, bool isChecked, bool isEnabled) const
{
    checkbox.blockSignals(true);
    checkbox.setChecked(isChecked);
    checkbox.setEnabled(isEnabled);
    checkbox.blockSignals(false);
}


void SelectKeyStroke::updateShortcutWidgets(const ButtonShortcut& shortcut)
{
    Q_D(SelectKeyStroke);

    if (shortcut.isKeystroke()) {
        // shortcut is a key sequence - update it accordingly
        QKeySequence qkeySequence = QKeySequence::fromString(shortcut.toQKeySequenceString());

        d->ui->shortcutSelectorWidget->blockSignals(true);
        d->ui->shortcutSelectorWidget->setKeySequence(qkeySequence);
        d->ui->shortcutSelectorWidget->blockSignals(false);

    } else {
        // shortcut is unknown or not set - input is available
        d->ui->shortcutSelectorWidget->blockSignals(true);
        d->ui->shortcutSelectorWidget->clearKeySequence();
        d->ui->shortcutSelectorWidget->blockSignals(false);
    }
}
