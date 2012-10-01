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

#ifndef KCMWACOMTABLET_H
#define KCMWACOMTABLET_H

// KDE includes
#include <KDE/KCModule>

//Qt includes
#include <QtCore/QMap>
#include <QtCore/QPointer>

class QVBoxLayout;

namespace Wacom
{
class AboutData;
class TabletWidget;

/**
  * This class is the starting point for the tablet KCModule
  * Here everything is initialized and set up in the right way
  */
class KCMWacomTablet : public KCModule
{
    Q_OBJECT

public:
    /**
      * default constructor
      *
      * @param parent parent widget
      */
    KCMWacomTablet(QWidget *parent, const QVariantList &);
    virtual ~KCMWacomTablet();

    /**
      * Called when the user hits the default button to reload the saved values from the config file
      */
    void load();

    /**
      * Called when the user hits apply/ok to save the current profile changes
      */
    void save();

private slots:
    /**
      * Initialize the module and set up all widgets
      * finds the connected tablet and fills in all the values
      */
    void initModule();

private:
    QVBoxLayout           *m_layout;        /**< Basic layout for the module */
    QPointer<TabletWidget> m_tabletWidget;  /**< Main widget that holds all other tabs */
    AboutData             *m_about;         /**< The about data returned by the module */
    bool                   m_changed;       /**< Saves if the profiles are changed or not */
};
}
#endif // KCMWACOMTABLET_H
