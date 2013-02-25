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

#include <QString>
#include <QMap>

#include "buttonshortcut.h"
#include "testbuttonshortcut.moc"

using namespace Wacom;

void TestButtonShortcut::assertButton(const ButtonShortcut& shortcut, int buttonNumber) const
{
    QVERIFY(shortcut.isSet());
    QVERIFY(shortcut.isButton());
    QCOMPARE(ButtonShortcut::BUTTON, shortcut.getType());

    QVERIFY(!shortcut.isKeystroke());
    QVERIFY(!shortcut.isModifier());

    QCOMPARE(buttonNumber, shortcut.getButton());
    QCOMPARE(QString::number(buttonNumber), shortcut.toString());
    QVERIFY(shortcut.toQKeySequenceString().isEmpty());
}


void TestButtonShortcut::assertEquals(const ButtonShortcut& shortcut1, const ButtonShortcut& shortcut2) const
{
    QVERIFY(shortcut1 == shortcut2);
    QVERIFY(!(shortcut1 != shortcut2));
    QVERIFY(shortcut1.getButton() == shortcut2.getButton());
    QVERIFY(shortcut1.getType() == shortcut2.getType());
    QVERIFY(shortcut1.isSet() == shortcut2.isSet());
    QVERIFY(shortcut1.isButton() == shortcut2.isButton());
    QVERIFY(shortcut1.isKeystroke() == shortcut2.isKeystroke());
    QVERIFY(shortcut1.isModifier() == shortcut2.isModifier());
    QCOMPARE(shortcut1.toString(), shortcut2.toString());
    QCOMPARE(shortcut2.toQKeySequenceString(), shortcut2.toQKeySequenceString());
}


void TestButtonShortcut::assertKeySequence(const ButtonShortcut& shortcut, const QString& sequence, bool sequenceIsQKeySeq, bool isModifier) const
{
    QVERIFY(shortcut.isSet());

    QVERIFY(!shortcut.isButton());

    QCOMPARE(0, shortcut.getButton());

    if (isModifier) {
        QVERIFY(shortcut.isModifier());
        QVERIFY(!shortcut.isKeystroke());
    } else {
        QVERIFY(shortcut.isKeystroke());
        QVERIFY(!shortcut.isModifier());
    }

    if (sequenceIsQKeySeq) {
        QCOMPARE(shortcut.toQKeySequenceString(), sequence);
    } else {
        QCOMPARE(shortcut.toString(), sequence);
    }
}


void TestButtonShortcut::assertKeyStroke(const ButtonShortcut& shortcut, const QString& sequence, bool isQKeySequence) const
{
    assertKeySequence(shortcut, sequence, isQKeySequence, false);
}


void TestButtonShortcut::assertModifier(const ButtonShortcut& shortcut, const QString& sequence) const
{
    assertKeySequence(shortcut, sequence, false, true);
}


void TestButtonShortcut::assertUnset(const ButtonShortcut& shortcut) const
{
    QVERIFY(!shortcut.isSet());
    QVERIFY(!shortcut.isButton());
    QVERIFY(!shortcut.isKeystroke());
    QVERIFY(!shortcut.isModifier());

    QCOMPARE(0, shortcut.getButton());
    QCOMPARE(QLatin1String("0"), shortcut.toString());
    QVERIFY(shortcut.toQKeySequenceString().isEmpty());
}


void TestButtonShortcut::testAssignment()
{
    ButtonShortcut buttonShortcut;
    ButtonShortcut modifierShortcut;
    ButtonShortcut strokeShortcut;

    buttonShortcut.setButton(2);
    modifierShortcut.set(QLatin1String("key ctrl shift"));
    strokeShortcut.set(QLatin1String("key ctrl a"));

    // Test Constructors.
    ButtonShortcut copyShortcut(buttonShortcut);
    assertEquals(copyShortcut, buttonShortcut);
    assertButton(copyShortcut, buttonShortcut.getButton());

    ButtonShortcut copyShortcut2 (modifierShortcut.toString());
    assertEquals(copyShortcut2, modifierShortcut);
    assertModifier(copyShortcut2, modifierShortcut.toString());

    ButtonShortcut copyShortcut3 (buttonShortcut.getButton());
    assertEquals(copyShortcut3, buttonShortcut);
    assertButton(copyShortcut3, buttonShortcut.getButton());

    // Test Assignment Operators
    copyShortcut = modifierShortcut;
    assertEquals(copyShortcut, modifierShortcut);
    assertModifier(copyShortcut, modifierShortcut.toString());

    copyShortcut = modifierShortcut.toString();
    assertEquals(copyShortcut, modifierShortcut);
    assertModifier(copyShortcut, modifierShortcut.toString());

    copyShortcut = strokeShortcut;
    assertEquals(copyShortcut, strokeShortcut);
    assertKeyStroke(copyShortcut, strokeShortcut.toString(), false);

    copyShortcut = strokeShortcut.toString();
    assertEquals(copyShortcut, strokeShortcut);
    assertKeyStroke(copyShortcut, strokeShortcut.toString(), false);

    copyShortcut = buttonShortcut;
    assertEquals(copyShortcut, buttonShortcut);
    assertButton(copyShortcut, buttonShortcut.getButton());

    copyShortcut = buttonShortcut.toString();
    assertEquals(copyShortcut, buttonShortcut);
    assertButton(copyShortcut, buttonShortcut.getButton());
}


void TestButtonShortcut::testButton()
{
    int minButtonNr = 1;
    int medButtonNr = 5;
    int maxButtonNr = 32;

    ButtonShortcut shortcut(medButtonNr);
    assertButton(shortcut, medButtonNr);

    shortcut.setButton(minButtonNr);
    assertButton(shortcut, minButtonNr);

    shortcut.setButton(medButtonNr);
    assertButton(shortcut, medButtonNr);

    shortcut.set(QString::number(minButtonNr));
    assertButton(shortcut, minButtonNr);

    shortcut.clear();
    assertUnset(shortcut);

    shortcut.set(QString::number(maxButtonNr));
    assertButton(shortcut, maxButtonNr);

    shortcut.setButton(maxButtonNr);
    assertButton(shortcut, maxButtonNr);

    shortcut.clear();
    assertUnset(shortcut);

    shortcut.set(QLatin1String("button 2"));
    assertButton(shortcut, 2);

    shortcut.set(QLatin1String("Button 3"));
    assertButton(shortcut, 3);

    shortcut.set(QLatin1String(" button +4 "));
    assertButton(shortcut, 4);
}


void TestButtonShortcut::testEmpty() {

    ButtonShortcut shortcut1;
    ButtonShortcut shortcut2;

    // make sure the shortcuts are equal in every aspect
    assertEquals (shortcut1, shortcut2);

    // make sure the shortcut is not set to anything
    assertUnset (shortcut1);
}


void TestButtonShortcut::testInvalidKeyStrokes()
{
    QList<QString> inputList;

    inputList.append(QLatin1String("A+B"));
    inputList.append(QLatin1String("Ctrl+b+a"));

    ButtonShortcut shortcut;

    for (QList<QString>::iterator inputIter = inputList.begin() ; inputIter != inputList.end() ; ++inputIter) {
        shortcut.set(*inputIter);
        assertUnset(shortcut);
    }
}


void TestButtonShortcut::testKeyStrokes()
{
    // key is input, value is expected output value
    QMap<QString, QString> inputMap;

    inputMap.insert(QLatin1String("A"), QLatin1String("key a"));
    inputMap.insert(QLatin1String("key A"), QLatin1String("key a"));
    inputMap.insert(QLatin1String("key +A"), QLatin1String("key a"));

    inputMap.insert(QLatin1String("Ctrl+X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("Ctrl X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("key Ctrl+X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("key +Ctrl +X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("key Ctrl X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("key +Ctrl +X -X"), QLatin1String("key ctrl x"));
    inputMap.insert(QLatin1String("key +Ctrl +X -x -a"), QLatin1String("key ctrl x"));

    inputMap.insert(QLatin1String("Meta+X"), QLatin1String("key super x"));
    inputMap.insert(QLatin1String("Meta X"), QLatin1String("key super x"));
    inputMap.insert(QLatin1String("key Meta+X"), QLatin1String("key super x"));
    inputMap.insert(QLatin1String("key +Meta +X"), QLatin1String("key super x"));
    inputMap.insert(QLatin1String("key Meta X"), QLatin1String("key super x"));

    inputMap.insert(QLatin1String("+"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("Ctrl++"), QLatin1String("key ctrl plus"));
    inputMap.insert(QLatin1String("key +"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("key ++"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("key Ctrl +"), QLatin1String("key ctrl plus"));
    inputMap.insert(QLatin1String("key +Ctrl ++"), QLatin1String("key ctrl plus"));

    inputMap.insert(QLatin1String("-"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("Ctrl+-"), QLatin1String("key ctrl minus"));
    inputMap.insert(QLatin1String("key -"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("key +-"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("key Ctrl -"), QLatin1String("key ctrl minus"));
    inputMap.insert(QLatin1String("key +Ctrl +-"), QLatin1String("key ctrl minus"));


    ButtonShortcut shortcut;

    for (QMap<QString,QString>::iterator inputIter = inputMap.begin() ; inputIter != inputMap.end() ; ++inputIter) {
        shortcut.set(inputIter.key());
        assertKeyStroke(shortcut, inputIter.value(), false);

        shortcut.clear();
        assertUnset(shortcut);

        shortcut.set(inputIter.key().toLower());
        assertKeyStroke(shortcut, inputIter.value().toLower(), false);
    }
}


void TestButtonShortcut::testModifier()
{
    // key is input, value is expected output value
    QMap<QString, QString> inputMap;
    inputMap.insert(QLatin1String("Ctrl+Alt+Shift+Super"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("Ctrl+Alt+Shift+Meta"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("Shift+Meta+Ctrl"), QLatin1String("key shift super ctrl"));
    inputMap.insert(QLatin1String("Alt+Ctrl"), QLatin1String("key alt ctrl"));
    inputMap.insert(QLatin1String("Shift"), QLatin1String("key shift"));
    inputMap.insert(QLatin1String("Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("Super"), QLatin1String("key super"));

    inputMap.insert(QLatin1String("key Ctrl+Alt+Shift+Super"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("key Ctrl+Alt+Shift+Meta"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("key Shift+Meta+Ctrl"), QLatin1String("key shift super ctrl"));
    inputMap.insert(QLatin1String("key Alt+Ctrl"), QLatin1String("key alt ctrl"));
    inputMap.insert(QLatin1String("key Shift"), QLatin1String("key shift"));
    inputMap.insert(QLatin1String("key Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("key Super"), QLatin1String("key super"));

    inputMap.insert(QLatin1String("key +Ctrl +Alt +Shift +Super"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("key +Ctrl +Alt +Shift +Meta"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("key +Shift +Meta +Ctrl"), QLatin1String("key shift super ctrl"));
    inputMap.insert(QLatin1String("key +Alt +Ctrl"), QLatin1String("key alt ctrl"));
    inputMap.insert(QLatin1String("key +Shift"), QLatin1String("key shift"));
    inputMap.insert(QLatin1String("key +Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("key +Super"), QLatin1String("key super"));

    inputMap.insert(QLatin1String("Ctrl Alt Shift Super"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("Ctrl Alt Shift Meta"), QLatin1String("key ctrl alt shift super"));
    inputMap.insert(QLatin1String("Shift Meta Ctrl"), QLatin1String("key shift super ctrl"));
    inputMap.insert(QLatin1String("Alt Ctrl"), QLatin1String("key alt ctrl"));
    inputMap.insert(QLatin1String("Shift"), QLatin1String("key shift"));
    inputMap.insert(QLatin1String("Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("Super"), QLatin1String("key super"));

    ButtonShortcut shortcut;

    for (QMap<QString,QString>::iterator inputIter = inputMap.begin() ; inputIter != inputMap.end() ; ++inputIter) {
        shortcut.set(inputIter.key());
        assertModifier(shortcut, inputIter.value());

        shortcut.clear();
        assertUnset(shortcut);

        shortcut.set(inputIter.key().toLower());
        assertModifier(shortcut, inputIter.value().toLower());
    }
}


void TestButtonShortcut::testQKeySequences()
{
    // key is input, value is expected output value
    QMap<QString, QString> inputMap;

    inputMap.insert(QLatin1String("A"), QLatin1String("A"));
    inputMap.insert(QLatin1String("key A"), QLatin1String("A"));
    inputMap.insert(QLatin1String("key +A"), QLatin1String("A"));

    inputMap.insert(QLatin1String("Ctrl+X"), QLatin1String("Ctrl+X"));
    inputMap.insert(QLatin1String("Ctrl X"), QLatin1String("Ctrl+X"));
    inputMap.insert(QLatin1String("key Ctrl+X"), QLatin1String("Ctrl+X"));
    inputMap.insert(QLatin1String("key +Ctrl +X"), QLatin1String("Ctrl+X"));
    inputMap.insert(QLatin1String("key Ctrl X"), QLatin1String("Ctrl+X"));

    inputMap.insert(QLatin1String("Meta+X"), QLatin1String("Meta+X"));
    inputMap.insert(QLatin1String("Meta X"), QLatin1String("Meta+X"));
    inputMap.insert(QLatin1String("key Meta+X"), QLatin1String("Meta+X"));
    inputMap.insert(QLatin1String("key +Meta +X"), QLatin1String("Meta+X"));
    inputMap.insert(QLatin1String("key Meta X"), QLatin1String("Meta+X"));

    ButtonShortcut shortcut;

    for (QMap<QString,QString>::iterator inputIter = inputMap.begin() ; inputIter != inputMap.end() ; ++inputIter) {
        shortcut.set(inputIter.key());
        assertKeyStroke(shortcut, inputIter.value(), true);

        shortcut.clear();
        assertUnset(shortcut);

        shortcut.set(inputIter.key().toLower());
        assertKeyStroke(shortcut, inputIter.value(), true);
    }
}


