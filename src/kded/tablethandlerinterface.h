/*
 * Copyright 2012 Alexander Maret-Huskinson <alex@maret.de>
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

#ifndef TABLETHANDLERINTERFACE_H
#define TABLETHANDLERINTERFACE_H

#include "property.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Wacom
{

/**
 * This interface only exists so we can create a mock implementation of it for
 * unit testing. See the main implementation for method documentation.
 *
 * @see TabletHandler
 */
class TabletHandlerInterface : public QObject
{
public:
    explicit TabletHandlerInterface(QObject* parent = 0) : QObject(parent) {};

    virtual QString getProperty(const QString& device, const Property& property) const = 0;

    virtual QStringList listProfiles() const = 0;

    virtual void setProfile(const QString& profile) = 0;

    virtual void setProperty(const QString& device, const Property & property, const QString& value) = 0;

}; // CLASS
}  // NAMESPACE
#endif // HEADER PROTECTION
