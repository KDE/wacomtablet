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

#ifndef SCREENAREASELECTIONDIALOG_H
#define SCREENAREASELECTIONDIALOG_H

#include <KDE/KDialog>

#include <QtCore/QList>
#include <QtCore/QRect>
#include <QtCore/QString>

namespace Wacom
{

class ScreenAreaSelectionDialogPrivate;

class ScreenAreaSelectionDialog : public KDialog
{
    Q_OBJECT

public:

    explicit ScreenAreaSelectionDialog(QWidget* parent = 0);

    virtual ~ScreenAreaSelectionDialog();

    QString getSelection() const;

    void setSelection( const QString& selection);

    //! @deprecated use the other method
    void setupWidget( const QList<QRect>& screenAreas, const QRect& tabletArea, const QRect& selectedTabletArea, const QString& tabletAreaCaption = QString() );

    void setupWidget( const QRect& fullTabletArea, const QString& selectedTabletArea, const QString& tabletAreaCaption = QString() );


private:

    const QRect convertSelectedTabletAreaToQRect(const QString& selectedTabletArea, const QRect& fullTabletArea) const;

    void setupUi();

    Q_DECLARE_PRIVATE(ScreenAreaSelectionDialog)
    ScreenAreaSelectionDialogPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
