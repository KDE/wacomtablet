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

#ifndef PADBUTTONWIDGET_H
#define PADBUTTONWIDGET_H

//Qt includes
#include <QtGui/QWidget>

namespace Ui
{
class PadButtonWidget;
}

class KComboBox;

namespace Wacom
{
class ProfileManagement;

/**
  * The PadButton widget contains all settings to assign the buttons on the tablet pad
  *
  * The available number of buttons as well as the availability of vertical/horizontal strip
  * or TouchRing is defined by the wacom_devicelist settings file in the data folder.
  */
class PadButtonWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      * default constructor
      *
      * @param profileManager Handles the connection to the config files
      * @param parent parent Widget
      */
    explicit PadButtonWidget(ProfileManagement *profileManager, QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PadButtonWidget();

    /**
      * Initialize and setup the widget.
      *
      * gets its information for the detected device from the "wacom_devicelist" file
      */
    void init();

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
      * Opens a new dialogue for the buttons to select the button function.
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available
      *
      * @param selection button index
      * @see WacomInterface::PenButton Enumaration
      */
    void selectKeyFunction(int selection);

signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available
      */
    void changed();

private:
    /**
      * Fills the button selection combobox with all available values
      * Used in this way to get no redundant strings in the ui file for
      * any given translator. Thus reduce the strings necessary to translate.
      *
      * @param comboBox the combobox where the values are added to
      */
    void fillComboBox(KComboBox *comboBox);

    Ui::PadButtonWidget *m_ui;                /**< Handler to the padbuttonwidget.ui file */
    ProfileManagement   *m_profileManagement; /**< Handler for the profile config connection */
};

}

#endif /*PADBUTTONWIDGET_H*/
