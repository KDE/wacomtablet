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

#ifndef PRESSCURVEDIALOG_H
#define PRESSCURVEDIALOG_H

//Qt includes
#include <QtGui/QDialog>

namespace Ui
{
class PressCurveDialog;
}

class QDBusInterface;

namespace Wacom
{
class PressCurve;

/**
  * This dialog shows the PressCurve and lets the uses change it on the fly
  * The pressure is animated when the suer uses his pen inside the widget
  */
class PressCurveDialog : public QDialog
{
    Q_OBJECT
public:
    /**
      * default constructor
      *
      * @param parent parent widget
      */
    PressCurveDialog(QWidget *parent = 0);

    /**
      * default destructor
      */
    ~PressCurveDialog();

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
    void setDeviceHandler(QDBusInterface *deviceInterface, const QString & device);

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
    void recejct();

private:
    Ui::PressCurveDialog *m_ui;              /**< Handler to the presscurvedialog.ui file */
    QDBusInterface       *m_deviceInterface; /**< Connection to the tablet daemon DBus /Device Interface */
    QString               m_device;          /**< Devicename as used by the xsetwacomsetting for the stylus/eraser and named in the xorg.conf or by HAL */
    PressCurve           *m_pressCurve;      /**< Widget that shows the presscurve and lets him change it */
};

}

#endif // PRESSCURVEDIALOG_H
