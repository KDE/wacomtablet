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

#ifndef PENWIDGET_H
#define PENWIDGET_H

#include "devicetype.h"

#include <QtGui/QWidget>

namespace Ui
{
    class PenWidget;
}

class KComboBox;
class QLabel;

namespace Wacom
{

class PenWidgetPrivate;
class ButtonShortcut;

/**
  * The PenWidget class holds all settings for the stylus/eraser pen.
  * Through the xsetwacom interface the widget can manipulate the two buttons,
  * tip press curve, absolute/relative mode, double tab speed and across monitor support
  * via the Twinview options.
  */
class PenWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Enumeration for all available tablet pen button actions (stylus/eraser)
     */
    enum PenButtonAction {
        ActionDisabled,     //!< disables the button
        ActionSelected      //!< select an action
    };

    /**
      * default constructor
      * Initialize the widget.
      *
      * @param profileManager Handles the connection to the config files
      * @param parent parent Widget
      */
    explicit PenWidget(QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PenWidget();

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
      * Called whenever a value other than the pen buttons is changed.
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available
      */
    void profileChanged();

    /**
      * Opens a dialogue that allows the visual selection of the presscurve
      * If Qt detects the tablet (through the xorg.conf file) the changes can be tested
      * directly in the dialogue as well
      */
    void changeEraserPressCurve();

    /**
      * Opens a dialogue that allows the visual selection of the presscurve
      * If Qt detects the tablet (through the xorg.conf file) the changes can be tested
      * directly in the dialogue as well
      */
    void changeTipPressCurve();

signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available
      */
    void changed();


private slots:

    void onButtonActionChanged();


private:
    /**
     * The property name which is used to store a shortcut in a label.
     */
    static const char* LABEL_PROPERTY_KEYSEQUENCE;

    QString changePressCurve (const DeviceType& device, const QString& startValue);

    Q_DECLARE_PRIVATE( PenWidget )
    PenWidgetPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif /*PENWIDGET_H*/
