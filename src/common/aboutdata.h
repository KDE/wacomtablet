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

#ifndef ABOUTDATA_H
#define ABOUTDATA_H

#include <KDE/KAboutData>

namespace Wacom
{
/**
 * This KAboutData implementatation should be used in every KDE component.
 * It is here so we have central place where to manage all authors of this
 * project.
 */
class AboutData : public KAboutData
{
public:
    /**
     * For the parameters of this constructor see the constructor of \a KAboutData.
     *
     * @see KAboutData
     */
    AboutData(const QByteArray& appName, const KLocalizedString& programName, const QByteArray& version,
              const KLocalizedString& shortDescription = KLocalizedString(), const KLocalizedString& otherText = KLocalizedString());
}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
