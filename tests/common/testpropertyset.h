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

#ifndef TESTPROPERTYSET_H
#define TESTPROPERTYSET_H

#include <QtTest>
#include <QtCore>

namespace Wacom {

class TestPropertySet : public QObject
{
    Q_OBJECT

private slots:

    void testCompare();

    void testConstructor();

    void testFind();

    void testId();

    void testIds();

    void testIterator();

    void testKey();

    void testKeys();

    void testList();

    void testMap();

    void testOperator();

    void testSize();
};
}

QTEST_MAIN(Wacom::TestPropertySet)

#endif // HEADER PROTECTION
