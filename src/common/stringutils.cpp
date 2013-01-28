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

#include "stringutils.h"
#include "debug.h"

#include <QtCore/QRegExp>

using namespace Wacom;

bool StringUtils::asBool (const QString& value)
{
    QString trimmedValue = value.trimmed();

    return (trimmedValue.compare(QLatin1String("1")) == 0 ||
            trimmedValue.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0 ||
            trimmedValue.compare(QLatin1String("on"),   Qt::CaseInsensitive) == 0 ||
            trimmedValue.compare(QLatin1String("yes"),  Qt::CaseInsensitive) == 0);
}

