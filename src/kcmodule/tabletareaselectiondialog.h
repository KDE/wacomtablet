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

#ifndef TABLETAREASELECTIONDIALOG_H
#define TABLETAREASELECTIONDIALOG_H

#include <KDE/KDialog>
#include <QtCore/QRect>
#include <QtCore/QList>

namespace Wacom
{

class TabletAreaSelectionDialogPrivate;

class TabletAreaSelectionDialog : public KDialog
{
    Q_OBJECT

public:

    explicit TabletAreaSelectionDialog();

    virtual ~TabletAreaSelectionDialog();

    const QString getSelection() const;

    void setSelection(const QString& selection);

    //! @deprecated Use the other method.
    void setupWidget( const QRect& tabletArea, const QList< QRect >& screenAreas, const QRect& screenAreaSelection, const QString& deviceName );

    void setupWidget( const QRect& fullTabletArea, const QString& selectedScreenArea, const QString& deviceName );


private:

    const QRect convertScreenAreaMappingToQRect( const QList< QRect >& screenAreas, const QString& selectedScreenArea ) const;

    void setupUi();

    Q_DECLARE_PRIVATE(TabletAreaSelectionDialog)
    TabletAreaSelectionDialogPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
} // NAMESPACE
#endif // HEADER PROTECTION
