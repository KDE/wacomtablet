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

#include "generalpagewidget.h"
#include "ui_generalpagewidget.h"

#include "profilemanagement.h"

// common
#include "tabletinfo.h"
#include "dbustabletinterface.h"

// stdlib
#include <memory>

//KDE includes
#include <KShortcutsEditor>
#include <KActionCollection>
#include <KGlobalAccel>

//Qt includes
#include <QPointer>
#include <QStringList>
#include <QDBusInterface>
#include <QDBusReply>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>

#include <QDebug>

using namespace Wacom;

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class GeneralPageWidgetPrivate {
    public:
        ~GeneralPageWidgetPrivate() {
            delete m_actionCollection;
            delete m_shortcutEditor;
        }

        std::shared_ptr<Ui::GeneralPageWidget>  m_ui;                /**< Handler to the generalwidget.ui file */
        QPointer<KActionCollection>       m_actionCollection;
        QPointer<KShortcutsEditor>        m_shortcutEditor;
        QString                           tabletId;
}; // CLASS
}  // NAMESPACE


GeneralPageWidget::GeneralPageWidget(QWidget *parent)
        : QWidget(parent), d_ptr(new GeneralPageWidgetPrivate)
{
    Q_D( GeneralPageWidget );

    d->m_ui = std::shared_ptr<Ui::GeneralPageWidget>(new Ui::GeneralPageWidget);
    d->m_ui->setupUi(this);

    //if someone adds another action also add it to kded/tabletdeamon.cpp
    d->m_actionCollection = new KActionCollection(this, QLatin1Literal("wacomtablet") );
    d->m_actionCollection->setConfigGlobal(true);

    QAction *action = d->m_actionCollection->addAction(QLatin1String("Toggle touch tool"));
    action->setText( i18nc( "@action", "Enable/Disable the Touch Tool" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "input-tablet" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_T ) );

    action = d->m_actionCollection->addAction(QLatin1String("Toggle stylus mode"));
    action->setText( i18nc( "@action", "Toggle the Stylus Tool Relative/Absolute" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "draw-path" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_S ));

    action = d->m_actionCollection->addAction(QLatin1String("Toggle screen map selection"));
    action->setText( i18nc( "@action", "Toggle between all screens" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "draw-path" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_M ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to fullscreen"));
    action->setText( i18nc( "@action Maps the area of the tablet to all available screen space (space depends on connected monitors)", "Map to fullscreen" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "video-display" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_F ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to screen 1"));
    action->setText( i18nc( "@action", "Map to screen 1" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "video-display" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_1 ) );

    action = d->m_actionCollection->addAction(QLatin1String("Map to screen 2"));
    action->setText( i18nc( "@action", "Map to screen 2" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "video-display" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_2 ) );

    action = d->m_actionCollection->addAction(QLatin1String("Next Profile"));
    action->setText( i18nc( "@action Switch to the next profile in the rotation", "Next profile" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "go-next-use" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_N ) );

    action = d->m_actionCollection->addAction(QLatin1String("Previous Profile"));
    action->setText( i18nc( "@action Switch to the previous profile in the rotation", "Previous Profile" ) );
    action->setIcon( QIcon::fromTheme( QLatin1String( "go-previous-use" ) ) );
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << QKeySequence( Qt::CTRL + Qt::META + Qt::Key_P ) );

    d->m_shortcutEditor = new KShortcutsEditor(this, KShortcutsEditor::GlobalAction);
    d->m_shortcutEditor->addCollection(d->m_actionCollection, i18n("Wacom Tablet Settings"));

    d->m_ui->shortcutGroupBox->layout()->addWidget(d->m_shortcutEditor);

    //setup icons in the profilerotation list box
    d->m_ui->pbAddToRotationList->setIcon(QIcon::fromTheme( QLatin1String( "list-add" ) ));
    d->m_ui->pbRemoveFromRotationList->setIcon(QIcon::fromTheme( QLatin1String( "list-remove" ) ));
    d->m_ui->pbUp->setIcon(QIcon::fromTheme( QLatin1String( "arrow-up" ) ));
    d->m_ui->pbDown->setIcon(QIcon::fromTheme( QLatin1String( "arrow-down" ) ));


    connect(d->m_shortcutEditor, SIGNAL(keyChange()), this, SLOT(profileChanged()));
}


GeneralPageWidget::~GeneralPageWidget()
{
    delete this->d_ptr;
}

void GeneralPageWidget::setTabletId(const QString &tabletId)
{
    Q_D( GeneralPageWidget );
    d->tabletId = tabletId;
}

void GeneralPageWidget::saveToProfile()
{
    Q_D( GeneralPageWidget );

    QList<QListWidgetItem *> items = d->m_ui->lwRotationList->findItems(QLatin1String("*"),
                                                                        Qt::MatchWrap | Qt::MatchWildcard);
    QStringList newRotationList;
    foreach(QListWidgetItem *item, items) {
        newRotationList.append(item->text());
    }

    DBusTabletInterface::instance().setProfileRotationList(d->tabletId, newRotationList);

    d->m_shortcutEditor->save();
}


void GeneralPageWidget::loadFromProfile() {}


void GeneralPageWidget::profileChanged()
{
    emit changed();
}


void GeneralPageWidget::reloadWidget()
{
    Q_D( GeneralPageWidget );

    //get information via DBus
    QDBusReply<QString> deviceName       = DBusTabletInterface::instance().getInformation(d->tabletId, TabletInfo::TabletName.key());

    //load rotation profile list based on current tablet
    QDBusReply<QStringList> rotationList = DBusTabletInterface::instance().getProfileRotationList(d->tabletId);
    d->m_ui->lwRotationList->clear();
    d->m_ui->lwRotationList->addItems(rotationList);
}

void GeneralPageWidget::profileUp()
{
    Q_D( GeneralPageWidget );

    QListWidgetItem *curItem = d->m_ui->lwRotationList->currentItem();
    if(curItem) {
        int curRow = d->m_ui->lwRotationList->row(curItem);
        int nextRow = curRow-1;
        if(nextRow >= 0) {
            QListWidgetItem *curItem = d->m_ui->lwRotationList->takeItem(curRow);
            d->m_ui->lwRotationList->insertItem(nextRow, curItem->text());
            d->m_ui->lwRotationList->setCurrentRow(nextRow);
        }
        emit changed();
    }
}

void GeneralPageWidget::profileDown()
{
    Q_D( GeneralPageWidget );

    QListWidgetItem *curItem = d->m_ui->lwRotationList->currentItem();
    if(curItem) {
        int curRow = d->m_ui->lwRotationList->row(curItem);
        curItem = d->m_ui->lwRotationList->takeItem(curRow);
        d->m_ui->lwRotationList->insertItem(curRow+1, curItem->text());
        d->m_ui->lwRotationList->setCurrentRow(curRow+1);

        emit changed();
    }
}

void GeneralPageWidget::profileAdd()
{
    Q_D( GeneralPageWidget );

    bool ok;
    QString item = QInputDialog::getItem(this, i18n("Profile List"),
                                         i18n("Select the Profile you want to add:"), ProfileManagement::instance().availableProfiles(), 0, false, &ok);
    if (ok && !item.isEmpty()) {
        d->m_ui->lwRotationList->addItem(item);

        emit changed();
    }
}

void GeneralPageWidget::profileRemove()
{
    Q_D( GeneralPageWidget );

    QListWidgetItem *curItem = d->m_ui->lwRotationList->currentItem();
    if(curItem) {
        d->m_ui->lwRotationList->removeItemWidget(curItem);
        delete curItem;

        emit changed();
    }
}
