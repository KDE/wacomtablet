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

#ifndef PADMAPPING_H
#define PADMAPPING_H

//Qt includes
#include <QtGui/QWidget>

namespace Ui
{
class PadMapping;
}

class QDBusInterface;

namespace Wacom
{
class PadMappingPrivate;
class ProfileManagement;

/**
  * This class implements the widget to change the pad mapping
  * The pad mapping includes the rotation and the actual working area
  */
class PadMapping : public QWidget
{
    Q_OBJECT

public:
    /**
      * default constructor
      *
      * @param parent the parent widget
      */
    explicit PadMapping(QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PadMapping();

    /**
      * Sets the used tool for this mapping widget
      *
      * As the pad and touch tool are physically two different devices both should be calibrated on its own.
      * Here the used tool is selected.
      *
      * @param tool @arg @c 0 stylus/eraser and thus pad calibration
      *             @arg @c 1 touch tool calibration
      */
    void setTool(int tool);

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
      * Fires the changed() signal afterwards to inform the main widget that unsaved changes are available.
      */
    void profileChanged();
    
    /**
     * This opens a fullscreen dialog for the calibration of the tablet.
     */
    void showCalibrationDialog();

    /**
      * Updates the tablet area when the selected screen area is changed
      *
      * Only changes the tablet are if @c force @c proportions is activated.
      * This ensures the full screen area is mapped onto the tablet without loosing the proportions
      */
    void updateTabletArea();

signals:
    /**
      * Used to inform the main widget that unsaved changes in the current profile are available.
      */
    void changed();

private slots:
    /**
      * When the full tablet usage is selected this ensures the force proportions and tabletarea box is disabled
      */
    void setFullTabletUsage(bool useFullArea);

    /**
      * When force proportion is toggled and full tablet still set partOftablet and adjust the tabletarea widget
      */
    void setForceProportions(bool useProportionalArea);

    void setFullScreenUsage(bool fullScreen);
    void setPartOfScreenUsage(bool partOfScreen);
    void setMapToScreenUsage(bool mapToScreen);

private:

    Q_DECLARE_PRIVATE( PadMapping )
    PadMappingPrivate *const d_ptr; /**< d-pointer for this class */

}; // CLASS
}  // NAMESPACE
#endif // PADMAPPING_H
