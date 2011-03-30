/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef TOUCHWIDGET_H
#define TOUCHWIDGET_H

#include <QtGui/QWidget>

namespace Ui
{
class TouchWidget;
}

namespace Wacom
{
class ProfileManagement;

class TouchWidget : public QWidget
{
    Q_OBJECT
public:
    TouchWidget(ProfileManagement *profileManager, QWidget *parent = 0);
    /**
      * default destructor
      */
    ~TouchWidget();

    /**
      * Saves all values to the current profile
      */
    void saveToProfile();

public slots:
    /**
      * When called the widget information will be refreshed
      */
    void reloadWidget();

    /**
      * Called whenever the profile is switched or the widget needs to be reinitialized.
      *
      * Updates all values on the widget to the values from the profile.
      */
    void loadFromProfile();

    /**
      * Called whenever a value is changed.
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available.
      */
    void profileChanged();

signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available.
      */
    void changed();

private:
    Ui::TouchWidget *m_ui;              /**< Handler to the generalwidget.ui file */
    ProfileManagement *m_profileManagement; /**< Handler for the profile config connection */
};

}
#endif // TOUCHWIDGET_H
