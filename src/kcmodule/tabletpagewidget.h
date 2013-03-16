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

#ifndef PADTABWIDGET_H
#define PADTABWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QString>

namespace Wacom
{

class TabletPageWidgetPrivate;

/**
 * The "Tablet" tab of the main KCM widget.
 */
class TabletPageWidget : public QWidget
{
    Q_OBJECT

public:

    explicit TabletPageWidget( QWidget *parent = 0 );

    virtual ~TabletPageWidget();

    /**
     * Loads settings from the current profile and updates the widget accordingly.
     */
    void loadFromProfile();

    /**
     * Reinitializes the widget when a new tablet gets connected.
     */
    void reloadWidget();

    /**
     * Saves the current settings to the current profile.
     */
    void saveToProfile();


public slots:

    /**
     * Called when the user enables/disables the auto-rotation checkbox.
     */
    void onAutoRotateChanged (int state);

    /**
     * Called when any profile property value changes.
     */
    void onProfileChanged();

    /**
     * Called when the user presses the pad screen mapping button.
     */
    void onScreenMappingClicked();

    /**
     * Called when the user presses the pad tablet mapping button.
     */
    void onTabletMappingClicked();

    /**
     * Called when the state of the absolute tracking mode changes.
     */
    void onTrackingModeAbsolute(bool activated);

    /**
     * Called when the state of the relative tracking mode changes.
     */
    void onTrackingModeRelative(bool activated);


signals:

    /**
     * Emitted when the user changes configuration settings.
     */
    void changed();


protected:

    /**
     * Gets the current rotation in profile format.
     * These are the values returned by ScreenRotation.key().
     *
     * @return The current rotation in profile format.
     */
    const QString getRotation() const;

    /**
     * Gets the current screen area mapping in profile format.
     *
     * @return A screen mapping as returned by ScreenSpace::toString().
     */
    const QString& getScreenAreaMapping() const;

    /**
     * Gets the current tablet area mapping in profile format.
     * Possible return values are:
     *
     * "-1 -1 -1 -1" : The whole tablet area was selected.
     * "x1 y1 x2 y2" : An area was selected with the top left corner at (x1/y1) and
     *                 the bottom right corner at (x2/y2).
     *
     * @return The current tablet mapping in profile format.
     */
    const QString& getTabletAreaMapping() const;

    /**
     * Gets the current tracking mode selection.
     *
     * @return Either "absolute" or "relative".
     */
    const QString getTrackingMode() const;

    /**
     * Checks if auto-rotation or inverted auto-rotation is enabled.
     *
     * @return True if auto-rotation is enabled, else false.
     */
    bool isAutoRotationEnabled() const;

    /**
     * Check if inverted auto-rotation is enabled.
     *
     * @return True if inverted auto-rotation is active, else false.
     */
    bool isAutoRotateInversionEnabled() const;

    /**
     * Changes the auto-rotation configuration and updates the widgets.
     *
     * @param value True to enable auto-rotation, false to disable it.
     */
    void setAutoRotationEnabled (bool value);

    /**
     * Changes the inverte-auto rotation configuration. This will not
     * update the auto-rotation flag.
     *
     * @param value True to enable inverted auto-rotation, false to disable it.
     */
    void setAutoRotateInversionEnabled(bool value);

    /**
     * Sets the auto-rotation settings based on a string in profile format
     * and updates all widgets accordingly. Valid values for the parameter
     * are all values returned by ScreenRotation::key().
     *
     * @param value The new rotation in profile format.
     */
    void setRotation(const QString& value);

    /**
     * Sets a new screen area mapping and updates all widgets accordingly.
     * The given value has to be in profile format as returned by ScreenSpace::toString()
     *
     * @param value The new screen area mapping selection.
     */
    void setScreenAreaMapping(const QString& value);

    /**
     * Sets a new tablet area mapping and updates all widgets accordingly.
     * The given value has to be in profile format. Valid values are:
     *
     * "-1 -1 -1 -1" : The whole tablet area gets selected.
     * "x1 y1 x2 y2" : An area gets selected with the top left corner at (x1/y1) and
     *                 the bottom right corner at (x2/y2).
     *
     * @param value The new tablet area mapping in profile format.
     */
    void setTabletAreaMapping(const QString& value);


    /**
     * Sets the tracking mode and updates all widgets accordingly.
     *
     * @param value Either "absolute" or "relative"
     */
    void setTrackingMode(const QString& value);


private:

    /**
     * Initializes this widget. Must only be called once by the constructor.
     */
    void setupUi();

    Q_DECLARE_PRIVATE( TabletPageWidget )
    TabletPageWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
