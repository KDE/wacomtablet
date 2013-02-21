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

#ifndef TESTBUTTONSHORTCUT_H
#define TESTBUTTONSHORTCUT_H

#include <QtTest>
#include <QtCore>

#include "buttonshortcut.h"

namespace Wacom {

class TestButtonShortcut : public QObject
{
    Q_OBJECT

public:

    void assertButton (const ButtonShortcut& shortcut, int buttonNumber) const;

    void assertEquals (const ButtonShortcut& shortcut1, const ButtonShortcut& shortcut2) const;

    void assertKeySequence (const ButtonShortcut& shortcut, const QString& sequence, bool sequenceIsQKeySeq, bool isModifier) const;

    void assertKeyStroke (const Wacom::ButtonShortcut& shortcut, const QString& sequence, bool isQKeySequence) const;

    void assertModifier (const ButtonShortcut& shortcut, const QString& sequence) const;

    void assertToggle (const ButtonShortcut& shortcut, ButtonShortcut::ShortcutType type) const;

    void assertUnset (const ButtonShortcut& shortcut) const;

private slots:

    void testAssignment();
    
    void testButton();

    void testInvalidKeyStrokes();

    void testKeyStrokes();

    void testModifier();

    void testQKeySequences();

    void testToggle();

    void testEmpty();

}; // CLASS
}  // NAMESPACE

QTEST_MAIN(Wacom::TestButtonShortcut)

#endif // HEADER PROTECTION
