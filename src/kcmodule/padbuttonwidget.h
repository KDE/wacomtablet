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

#ifndef PADBUTTONWIDGET_H
#define PADBUTTONWIDGET_H

//Qt includes
#include <QtGui/QWidget>

namespace Ui
{
    class PadButtonWidget;
}

class KComboBox;
class QLabel;

namespace Wacom
{

class PadButtonWidgetPrivate;
class ButtonShortcut;

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
     * Enumeration for all available tablet pad button actions
     */
    enum PadButtonAction {
        ActionDisabled,
        ActionMouseClick,
        ActionKeyStroke
    };

    /**
      * default constructor
      *
      * @param profileManager Handles the connection to the config files
      * @param parent parent Widget
      */
    explicit PadButtonWidget(QWidget *parent = 0);

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
     * The property name which is used to store a shortcut in a label.
     */
    static const char* LABEL_PROPERTY_KEYSEQUENCE;

    /**
      * Fills the button selection combobox with all available values
      * Used in this way to get no redundant strings in the ui file for
      * any given translator. Thus reduce the strings necessary to translate.
      *
      * @param comboBox the combobox where the values are added to
      */
    void fillComboBox(KComboBox *comboBox);

    /**
     * Determines the button action the given shortcut represents.
     *
     * @param shortcut The shortcut to analyze.
     *
     * @return The associated button action.
     */
    PadButtonAction getButtonAction(const ButtonShortcut& shortcut) const;

    /**
     * Gets the shortcut sequence from the given action label in a format
     * that can be stored in the device profile.
     *
     * @param label The label to get the shortcut sequence from.
     *
     * @return The shortcut as string.
     */
    const QString getButtonActionShortcut (QLabel* label) const;

    /**
     * Determines the display name of a shortcut. This is either the name
     * of the global shortcut associated with this key sequence or the
     * display string returned by \a ButtonShortcut.
     *
     * @param shortcut The shortcut to get the display name from.
     *
     * @return The shortcut's display name or an empty string if the shortcut is not set.
     */
    const QString getShortcutDisplayName(const ButtonShortcut& shortcut) const;

    /**
     * This is called when the user selects a different entry from the
     * pen button actions dropdown menu.
     *
     * @param selection The item number which was selected from the combo box.
     * @param combo The action combo box which was changed.
     * @param label The action label which belongs to this combo box.
     */
    void onButtonActionSelectionChanged(int selection, KComboBox& combo, QLabel& label);

    /**
     * Sets a shortcut on the given action label and updates the selection
     * of the given combo box.
     *
     * @param combo The combo box to update.
     * @param label The action label to set the shortcut on.
     * @param shortcutSequence The shortcut to set.
     *
     * @return True if the shortcut was set and the combo box updated, else false.
     */
    bool setButtonActionShortcut(KComboBox* combo, QLabel* label, const QString& shortcutSequence) const;


    Q_DECLARE_PRIVATE( PadButtonWidget )
    PadButtonWidgetPrivate *const d_ptr; //!< The D-Pointer of this class.

}; // CLASS
}  // NAMESPACE
#endif /*PADBUTTONWIDGET_H*/
