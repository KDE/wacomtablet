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

#ifndef PRESSURECURVEDIALOG_H
#define PRESSURECURVEDIALOG_H

#include "devicetype.h"

#include <QtGui/QDialog>

namespace Ui
{
class PressureCurveDialog;
}

class QDBusInterface;

namespace Wacom
{
class PressureCurveWidget;

/**
  * This dialog shows the PressureCurveWidget and lets the uses change it on the fly
  * The pressure is animated when the suer uses his pen inside the widget
  */
class PressureCurveDialog : public QDialog
{
    Q_OBJECT
public:
    /**
      * default constructor
      *
      * @param parent parent widget
      */
    explicit PressureCurveDialog(QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PressureCurveDialog();

    /**
      * Sets the controllpoints to some starting values
      *
      * @param points starting values in the format as expected by the xssetwacom settings
      */
    void setControllPoints(const QString & points);

    /**
      * Sets the currently used WacomInterface instance to allow on the fly changing of the presscurve
      * Gives a better feeling when changing the beziercurve and directly see how this affects
      * the applied pressure with the pen
      *
      * @param deviceInterface Interface for the connection to the tablet device.
      * @param device devicename as used by the xsetwacomsetting for the stylus/eraser and named in the xorg.conf or by HAL
      */
    void setDeviceType (const DeviceType& deviceType);

    /**
      * Returns the changed presscurve control points in the format needed by the xsetwacom settings
      *
      * @return string version of both control points separated by spaces
      */
    QString getControllPoints();

public slots:
    /**
      * Updates the pen presscurve viathe xsetwacom settings and shows the values in the dialog
      *
      * @param points string version of both control points separated by spaces
      */
    void updateControlPoints(const QString & points);

    /**
      * called when the user accept his changes
      */
    void accept();

    /**
      * called when the user discards his changes
      */
    void reject();

private:
    Ui::PressureCurveDialog *m_ui;              /**< Handler to the presscurvedialog.ui file */
    QString                  m_device;
    PressureCurveWidget     *m_pressCurve;      /**< Widget that shows the presscurve and lets him change it */
};
}
#endif // PRESSURECURVEDIALOG_H
