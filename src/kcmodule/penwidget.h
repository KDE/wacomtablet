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

#include <QtGui/QWidget>

namespace Wacom
{

class PenWidgetPrivate;

class ButtonShortcut;
class DeviceType;
class Property;

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
      * default constructor
      * Initialize the widget.
      *
      * @param parent parent Widget
      */
    explicit PenWidget(QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PenWidget();

    /**
      * Called whenever the profile is switched or the widget needs to be reinitialized.
      *
      * Updates all values on the widget to the values from the profile.
      */
    void loadFromProfile();

    /**
      * Reloads the widget when the status of the tablet device changes (connects/disconnects)
      */
    void reloadWidget();

    /**
      * Saves all values to the current profile
      */
    void saveToProfile();


public slots:
    /**
      * Opens a dialogue that allows the visual selection of the presscurve
      * If Qt detects the tablet (through the xorg.conf file) the changes can be tested
      * directly in the dialogue as well
      */
    void onChangeEraserPressCurve();

    /**
      * Opens a dialogue that allows the visual selection of the presscurve
      * If Qt detects the tablet (through the xorg.conf file) the changes can be tested
      * directly in the dialogue as well
      */
    void onChangeTipPressCurve();

    /**
      * Called whenever a value other than the pen buttons is changed.
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available
      */
    void onProfileChanged();


signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available
      */
    void changed();


protected:

    const QString getButtonShortcut (const Property& button) const;

    const QString getPressureCurve ( const DeviceType& device ) const;

    const QString getPressureFeel ( const DeviceType& device ) const;

    const QString getTabletPcButton() const;

    void setButtonShortcut (const Property& button, const QString& shortcut);

    void setPressureCurve (const DeviceType& device, const QString& value);

    void setPressureFeel (const DeviceType& device, const QString& value);

    void setTabletPcButton (const QString& value);


private:

    void changePressCurve (const DeviceType& deviceType);

    /**
     * Sets up the user interface widgets. Should only be called once by a constructor.
     */
    void setupUi();

    Q_DECLARE_PRIVATE( PenWidget )
    PenWidgetPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif /*PENWIDGET_H*/
