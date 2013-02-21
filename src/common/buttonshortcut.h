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

#ifndef BUTTONSHORTCUT_H
#define BUTTONSHORTCUT_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMap>

namespace Wacom {

class ButtonShortcutPrivate;

class ButtonShortcut {
public:

    enum ShortcutType {
        NONE          = 0,
        BUTTON        = 1,
        KEYSTROKE     = 2,
        MODIFIER      = 3,

        TOGGLEMODE    = -1,
        TOGGLEDISPLAY = -2
    };

    static const QString TOGGLEDISPLAY_STRING;
    static const QString TOGGLEMODE_STRING;

    ButtonShortcut();
    ButtonShortcut(const ButtonShortcut& that);
    ~ButtonShortcut();

    ButtonShortcut& operator= (const ButtonShortcut& that);

    bool operator== (const ButtonShortcut& that) const;

    bool operator!= (const ButtonShortcut& that) const;

    void clear();

    int getButton() const;

    ShortcutType getType() const;

    bool isButton() const;

    bool isKeystroke() const;

    bool isModifier() const;

    bool isSet() const;

    bool isToggle() const;

    bool isToggleDisplay() const;

    bool isToggleMode() const;

    bool set(int buttonNumber);

    bool set(const QString& sequence);

    bool setToggle (ShortcutType toggle);

    QString toQKeySequenceString() const;

    QString toString() const;

private:

    /**
     * A two dimensional array which contains key conversions. The first value
     * is the key in storage format, the second value is the key in QKeySequence
     * format. The array has to be NULL terminated!
     *
     * DO NOT USE THIS ARRAY DIRECTLY! It is only required to build the static
     * conversion maps and should not be used for anything else.
     *
     * @sa getConvertFromStorageMap()
     * @sa getConvertToStorageMap()
     */
    static const char* CONVERT_KEY_MAP_DATA[][2];

    bool convertKey(QString& key, bool fromStorage) const;

    void convertToNormalizedKeySequence(QString& sequence, bool fromStorage) const;

    void convertKeySequenceToStorageFormat (QString& sequence) const;

    void convertKeySequenceToQKeySequenceFormat (QString& sequence) const;

    /**
     * Returns the map which is used to convert keys from storage format to
     * the QKeySequence format.
     */
    static const QMap<QString, QString>& getConvertFromStorageMap();

    /**
     * Returns the map which is used to convert keys from QKeySequence format to
     * the storage format.
     */
    static const QMap<QString, QString>& getConvertToStorageMap();

    /**
     * This is just a helper method to initialize the static conversion maps.
     * Do not use it for anything else!
     *
     * @sa getConvertFromStorageMap()
     * @sa getConvertToStorageMap()
     */
    static QMap<QString, QString> initConversionMap(bool fromStorageMap);

    /**
     * Normalizes the key sequence by removing all unnecessary '+' signs.
     * The result is a string of keys and modifiers seperated by a whitespace.
     * This method also does some basic checks to verify that the syntax of
     * the sequence is valid. If the sequence is invalid it will be cleared and
     * the result will therefore be an empty string. The result of this method
     * will be directly written into the parameter \a sequence.
     *
     * @param sequence The sequence to normalize.
     */
    void normalizeKeySequence(QString& sequence) const;

    bool setButtonSequence(const QString& buttonNumber);

    bool setKeySequence(QString sequence);

    bool setModifierSequence(QString sequence);

    bool setToggleSequence(const QString& toggleSequence);

    Q_DECLARE_PRIVATE( ButtonShortcut )
    ButtonShortcutPrivate *const d_ptr; /**< d-pointer for this class */
};

}      // NAMESPACE
#endif // HEADER PROTECTION
