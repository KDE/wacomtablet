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

#ifndef GENERALWIDGET_H
#define GENERALWIDGET_H

#include <QtGui/QWidget>

namespace Ui
{
class GeneralWidget;
}

class QDBusInterface;

namespace Wacom
{
class ProfileManagement;

/**
  * This class shows some general information about the detected tablet device.
  *
  * Shows an image and the name plus all detected input devices (pad/stylus/eraser and so on).
  * Mainly used as debug output and to help the user to realize that his tablet
  * was detected correctly.
  */
class GeneralWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      * Default constructor
      *
      * @param deviceInterface Connection to the tablet daemon DBus /Device Interface
      * @param profileManager Handles the connection to the config files
      * @param parent the parent widget
      */
    explicit GeneralWidget(QDBusInterface *deviceInterface, ProfileManagement *profileManager, QWidget *parent = 0);

    /**
      * default destructor
      */
    ~GeneralWidget();

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
    Ui::GeneralWidget *m_ui;              /**< Handler to the generalwidget.ui file */
    QDBusInterface    *m_deviceInterface; /**< Connection to the tablet daemon DBus /Device Interface */
    ProfileManagement *m_profileManagement; /**< Handler for the profile config connection */
};

}

#endif // GENERALWIDGET_H
