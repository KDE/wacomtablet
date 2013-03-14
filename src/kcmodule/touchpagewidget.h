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

#ifndef TOUCHPAGEWIDGET_H
#define TOUCHPAGEWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QString>

class QRect;

namespace Wacom
{

class TouchPageWidgetPrivate;

class TouchPageWidget : public QWidget
{
    Q_OBJECT

public:

    explicit TouchPageWidget(QWidget* parent = 0);
    virtual ~TouchPageWidget();

    void loadFromProfile();

    void reloadWidget();

    void saveToProfile();


public slots:

    /**
     * Called when the user enables/disables gesture support.
     */
    void onGesturesModeChanged(int state);

    /**
     * Called when any profile property value changes.
     */
    void onProfileChanged();

    /**
     * Called when the user presses the touch screen mapping button.
     */
    void onScreenMappingClicked();

    /**
     * Called when the user presses the touch tablet mapping button.
     */
    void onTabletMappingClicked();

    /**
     * Called when the user enables/disables touch support.
     */
    void onTouchModeChanged(int state);

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

    const QString getGestureSupportEnabled() const;

    const QString& getScreenAreaMapping() const;

    const QString getScrollDistance() const;

    const QString& getTabletAreaMapping() const;

    const QString getTapTime() const;

    const QString getTouchSupportEnabled() const;

    const QString getTrackingMode() const;

    const QString getZoomDistance() const;

    bool isGesturesSupportEnabled() const;

    bool isTouchSupportEnabled() const;

    void setGesturesSupportEnabled(bool value);

    void setScreenAreaMapping(const QString& value);

    void setScrollDistance(const QString& value);

    void setTabletAreaMapping(const QString& value);

    void setTapTime(const QString& value);

    void setTrackingMode(const QString& value);

    void setTouchSupportEnabled(bool value);

    void setZoomDistance(const QString& value);


private:

    void setupUi();

    Q_DECLARE_PRIVATE( TouchPageWidget )
    TouchPageWidgetPrivate *const d_ptr; //!< D-Pointer for this class.

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
