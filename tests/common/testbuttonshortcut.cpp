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
    QVERIFY(!shortcut.isToggle());
    QVERIFY(!shortcut.isToggleDisplay());
    QVERIFY(!shortcut.isToggleMode());

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
    QVERIFY(shortcut1.isToggle() == shortcut2.isToggle());
    QVERIFY(shortcut1.isToggleDisplay() == shortcut2.isToggleDisplay());
    QVERIFY(shortcut1.isToggleMode() == shortcut2.isToggleMode());
    QCOMPARE(shortcut1.toString(), shortcut2.toString());
    QCOMPARE(shortcut2.toQKeySequenceString(), shortcut2.toQKeySequenceString());
}


void TestButtonShortcut::assertKeySequence(const ButtonShortcut& shortcut, const QString& sequence, bool sequenceIsQKeySeq, bool isModifier) const
{
    QVERIFY(shortcut.isSet());

    QVERIFY(!shortcut.isButton());
    QVERIFY(!shortcut.isToggle());
    QVERIFY(!shortcut.isToggleDisplay());
    QVERIFY(!shortcut.isToggleMode());

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


void TestButtonShortcut::assertToggle(const ButtonShortcut& shortcut, ButtonShortcut::ShortcutType type) const
{
    QVERIFY(shortcut.isSet());
    QVERIFY(shortcut.isToggle());

    QVERIFY(!shortcut.isButton());
    QVERIFY(!shortcut.isKeystroke());
    QVERIFY(!shortcut.isModifier());

    QCOMPARE(0, shortcut.getButton());
    QVERIFY(shortcut.toQKeySequenceString().isEmpty());


    if (type == ButtonShortcut::TOGGLEDISPLAY) {
        QCOMPARE(ButtonShortcut::TOGGLEDISPLAY, shortcut.getType());
        QCOMPARE(ButtonShortcut::TOGGLEDISPLAY_STRING, shortcut.toString());

        QVERIFY(shortcut.isToggleDisplay());
        QVERIFY(!shortcut.isToggleMode());
    } else {
        QCOMPARE(ButtonShortcut::TOGGLEMODE, shortcut.getType());
        QCOMPARE(ButtonShortcut::TOGGLEMODE_STRING, shortcut.toString());

        QVERIFY(shortcut.isToggleMode());
        QVERIFY(!shortcut.isToggleDisplay());
    }
}


void TestButtonShortcut::assertUnset(const ButtonShortcut& shortcut) const
{
    QVERIFY(!shortcut.isSet());
    QVERIFY(!shortcut.isButton());
    QVERIFY(!shortcut.isKeystroke());
    QVERIFY(!shortcut.isModifier());
    QVERIFY(!shortcut.isToggle());
    QVERIFY(!shortcut.isToggleDisplay());
    QVERIFY(!shortcut.isToggleMode());

    QCOMPARE(0, shortcut.getButton());
    QCOMPARE(QLatin1String("0"), shortcut.toString());
    QVERIFY(shortcut.toQKeySequenceString().isEmpty());
}


void TestButtonShortcut::testAssignment()
{
    ButtonShortcut buttonShortcut;
    ButtonShortcut modifierShortcut;
    ButtonShortcut toggleShortcut;
    ButtonShortcut strokeShortcut;

    buttonShortcut.set(2);
    modifierShortcut.set(QLatin1String("key ctrl shift"));
    toggleShortcut.setToggle(ButtonShortcut::TOGGLEDISPLAY);
    strokeShortcut.set(QLatin1String("key ctrl a"));

    ButtonShortcut copyShortcut(buttonShortcut);
    assertEquals(copyShortcut, buttonShortcut);
    assertButton(copyShortcut, buttonShortcut.getButton());

    copyShortcut = modifierShortcut;
    assertEquals(copyShortcut, modifierShortcut);
    assertModifier(copyShortcut, modifierShortcut.toString());

    copyShortcut = toggleShortcut;
    assertEquals(copyShortcut, toggleShortcut);
    assertToggle(copyShortcut, copyShortcut.getType());

    copyShortcut = strokeShortcut;
    assertEquals(copyShortcut, strokeShortcut);
    assertKeyStroke(copyShortcut, strokeShortcut.toString(), false);

    copyShortcut = buttonShortcut;
    assertEquals(copyShortcut, buttonShortcut);
    assertButton(copyShortcut, buttonShortcut.getButton());
}


void TestButtonShortcut::testButton()
{
    int minButtonNr = 1;
    int maxButtonNr = 32;

    ButtonShortcut shortcut;

    shortcut.set(minButtonNr);
    assertButton(shortcut, minButtonNr);

    shortcut.set(QString::number(minButtonNr));
    assertButton(shortcut, minButtonNr);

    shortcut.clear();
    assertUnset(shortcut);

    shortcut.set(QString::number(maxButtonNr));
    assertButton(shortcut, maxButtonNr);

    shortcut.set(maxButtonNr);
    assertButton(shortcut, maxButtonNr);

    shortcut.clear();
    assertUnset(shortcut);
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

    inputList.append(QLatin1String("button 1"));
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

    inputMap.insert(QLatin1String("A"), QLatin1String("key A"));
    inputMap.insert(QLatin1String("key A"), QLatin1String("key A"));
    inputMap.insert(QLatin1String("key +A"), QLatin1String("key A"));

    inputMap.insert(QLatin1String("Ctrl+X"), QLatin1String("key Ctrl X"));
    inputMap.insert(QLatin1String("Ctrl X"), QLatin1String("key Ctrl X"));
    inputMap.insert(QLatin1String("key Ctrl+X"), QLatin1String("key Ctrl X"));
    inputMap.insert(QLatin1String("key +Ctrl +X"), QLatin1String("key Ctrl X"));
    inputMap.insert(QLatin1String("key Ctrl X"), QLatin1String("key Ctrl X"));

    inputMap.insert(QLatin1String("Meta+X"), QLatin1String("key super X"));
    inputMap.insert(QLatin1String("Meta X"), QLatin1String("key super X"));
    inputMap.insert(QLatin1String("key Meta+X"), QLatin1String("key super X"));
    inputMap.insert(QLatin1String("key +Meta +X"), QLatin1String("key super X"));
    inputMap.insert(QLatin1String("key Meta X"), QLatin1String("key super X"));

    inputMap.insert(QLatin1String("+"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("Ctrl++"), QLatin1String("key Ctrl plus"));
    inputMap.insert(QLatin1String("key +"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("key ++"), QLatin1String("key plus"));
    inputMap.insert(QLatin1String("key Ctrl +"), QLatin1String("key Ctrl plus"));
    inputMap.insert(QLatin1String("key +Ctrl ++"), QLatin1String("key Ctrl plus"));

    inputMap.insert(QLatin1String("-"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("Ctrl+-"), QLatin1String("key Ctrl minus"));
    inputMap.insert(QLatin1String("key -"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("key +-"), QLatin1String("key minus"));
    inputMap.insert(QLatin1String("key Ctrl -"), QLatin1String("key Ctrl minus"));
    inputMap.insert(QLatin1String("key +Ctrl +-"), QLatin1String("key Ctrl minus"));


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
    inputMap.insert(QLatin1String("Ctrl+Alt+Shift+Super"), QLatin1String("key Ctrl Alt Shift Super"));
    inputMap.insert(QLatin1String("Ctrl+Alt+Shift+Meta"), QLatin1String("key Ctrl Alt Shift super"));
    inputMap.insert(QLatin1String("Shift+Meta+Ctrl"), QLatin1String("key Shift super Ctrl"));
    inputMap.insert(QLatin1String("Alt+Ctrl"), QLatin1String("key Alt Ctrl"));
    inputMap.insert(QLatin1String("Shift"), QLatin1String("key Shift"));
    inputMap.insert(QLatin1String("Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("Super"), QLatin1String("key Super"));

    inputMap.insert(QLatin1String("key Ctrl+Alt+Shift+Super"), QLatin1String("key Ctrl Alt Shift Super"));
    inputMap.insert(QLatin1String("key Ctrl+Alt+Shift+Meta"), QLatin1String("key Ctrl Alt Shift super"));
    inputMap.insert(QLatin1String("key Shift+Meta+Ctrl"), QLatin1String("key Shift super Ctrl"));
    inputMap.insert(QLatin1String("key Alt+Ctrl"), QLatin1String("key Alt Ctrl"));
    inputMap.insert(QLatin1String("key Shift"), QLatin1String("key Shift"));
    inputMap.insert(QLatin1String("key Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("key Super"), QLatin1String("key Super"));

    inputMap.insert(QLatin1String("key +Ctrl +Alt +Shift +Super"), QLatin1String("key Ctrl Alt Shift Super"));
    inputMap.insert(QLatin1String("key +Ctrl +Alt +Shift +Meta"), QLatin1String("key Ctrl Alt Shift super"));
    inputMap.insert(QLatin1String("key +Shift +Meta +Ctrl"), QLatin1String("key Shift super Ctrl"));
    inputMap.insert(QLatin1String("key +Alt +Ctrl"), QLatin1String("key Alt Ctrl"));
    inputMap.insert(QLatin1String("key +Shift"), QLatin1String("key Shift"));
    inputMap.insert(QLatin1String("key +Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("key +Super"), QLatin1String("key Super"));

    inputMap.insert(QLatin1String("Ctrl Alt Shift Super"), QLatin1String("key Ctrl Alt Shift Super"));
    inputMap.insert(QLatin1String("Ctrl Alt Shift Meta"), QLatin1String("key Ctrl Alt Shift super"));
    inputMap.insert(QLatin1String("Shift Meta Ctrl"), QLatin1String("key Shift super Ctrl"));
    inputMap.insert(QLatin1String("Alt Ctrl"), QLatin1String("key Alt Ctrl"));
    inputMap.insert(QLatin1String("Shift"), QLatin1String("key Shift"));
    inputMap.insert(QLatin1String("Meta"), QLatin1String("key super"));
    inputMap.insert(QLatin1String("Super"), QLatin1String("key Super"));

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

    inputMap.insert(QLatin1String("Meta+X"), QLatin1String("meta+X"));
    inputMap.insert(QLatin1String("Meta X"), QLatin1String("meta+X"));
    inputMap.insert(QLatin1String("key Meta+X"), QLatin1String("meta+X"));
    inputMap.insert(QLatin1String("key +Meta +X"), QLatin1String("meta+X"));
    inputMap.insert(QLatin1String("key Meta X"), QLatin1String("meta+X"));

    ButtonShortcut shortcut;

    for (QMap<QString,QString>::iterator inputIter = inputMap.begin() ; inputIter != inputMap.end() ; ++inputIter) {
        shortcut.set(inputIter.key());
        assertKeyStroke(shortcut, inputIter.value(), true);

        shortcut.clear();
        assertUnset(shortcut);

        shortcut.set(inputIter.key().toLower());
        assertKeyStroke(shortcut, inputIter.value().toLower(), true);
    }
}


void TestButtonShortcut::testToggle()
{
    ButtonShortcut shortcut;

    shortcut.setToggle(ButtonShortcut::TOGGLEDISPLAY);
    assertToggle(shortcut, ButtonShortcut::TOGGLEDISPLAY);

    shortcut.clear();
    assertUnset(shortcut);

    shortcut.setToggle(ButtonShortcut::TOGGLEMODE);
    assertToggle(shortcut, ButtonShortcut::TOGGLEMODE);

    shortcut.set(ButtonShortcut::TOGGLEDISPLAY_STRING);
    assertToggle(shortcut, ButtonShortcut::TOGGLEDISPLAY);

    shortcut.clear();
    assertUnset(shortcut);

    shortcut.set(ButtonShortcut::TOGGLEMODE_STRING);
    assertToggle(shortcut, ButtonShortcut::TOGGLEMODE);
}

