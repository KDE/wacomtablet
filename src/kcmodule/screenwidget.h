/*
 * Copyright 2009,2010 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

//Qt includes
#include <QtGui/QWidget>

namespace Ui
{
class ScreenWidget;
}

namespace Wacom
{
class ProfileManagement;

/**
  * This widget allows the screen setup to be changed
  * Handling of multiple monitors as well as Twinview changes are possible
  */
class ScreenWidget : public QWidget
{
    Q_OBJECT
public:
    /**
      * default constructor
      * Initialize the widget.
      *
      * @param profileManager Handles the connection to the config files
      * @param parent parent Widget
      */
    explicit ScreenWidget(ProfileManagement *profileManager, QWidget *parent = 0);

    /**
      * default destructor
      */
    ~ScreenWidget();

    /**
      * Saves all values to the current profile
      */
    void saveToProfile();

    /**
      * Reloads the widget when the status of the tablet device changes (connects/disconnects)
      *
      */
    void reloadWidget();

public slots:
    /**
      * Called whenever the profile is switched or the widget needs to be reinitialized.
      *
      * Updates all values on the widget to the values from the profile.
      */
    void loadFromProfile();

    /**
      * Called whenever a value is changed.
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available
      */
    void profileChanged();

    /**
      * Called when the TwinView option is changed
      * Enables / disables the none TwinView options
      *
      * @param currentIndex twinview selection
      */
    void switchTwinView(int currentIndex);

signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available
      */
    void changed();

private:
    Ui::ScreenWidget  *m_ui;                /**< Handler to the screenwidget.ui file */
    ProfileManagement *m_profileManagement; /**< Handler for the profile config connection */
};

}

#endif // SCREENWIDGET_H
