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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QtCore/QString>

namespace Wacom
{
class StringUtils
{
public:
    /**
     * Trimms the input value and checks if it is one of '1', 'true', 'on' or 'yes'.
     *
     * @param value The value to returen as bool.
     *
     * @return True if the input string is one of "1/true/on/yes", else false.
     */
    static bool asBool (const QString& value);

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
