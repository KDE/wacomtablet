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

#include <QtTest>
#include <QtCore>
#include <QString>

#include "deviceprofiletestutils.h"

using namespace Wacom;

void DeviceProfileTestUtils::assertValues(DeviceProfile& profile)
{
    QCOMPARE(profile.getAbsWheel2Down(), QLatin1String("AbsWheel2Down"));
    QCOMPARE(profile.getAbsWheel2Up(), QLatin1String("AbsWheel2Up"));
    QCOMPARE(profile.getAbsWheelDown(), QLatin1String("AbsWheelDown"));
    QCOMPARE(profile.getAbsWheelUp(), QLatin1String("AbsWheelUp"));
    QCOMPARE(profile.getArea(), QLatin1String("Area"));
    QCOMPARE(profile.getButton(1), QLatin1String("Button1"));
    QCOMPARE(profile.getButton(2), QLatin1String("Button2"));
    QCOMPARE(profile.getButton(3), QLatin1String("Button3"));
    QCOMPARE(profile.getButton(4), QLatin1String("Button4"));
    QCOMPARE(profile.getButton(5), QLatin1String("Button5"));
    QCOMPARE(profile.getButton(6), QLatin1String("Button6"));
    QCOMPARE(profile.getButton(7), QLatin1String("Button7"));
    QCOMPARE(profile.getButton(8), QLatin1String("Button8"));
    QCOMPARE(profile.getButton(9), QLatin1String("Button9"));
    QCOMPARE(profile.getButton(10), QLatin1String("Button10"));
    QCOMPARE(profile.getChangeArea(), QLatin1String("ChangeArea"));
    QCOMPARE(profile.getCursorProximity(), QLatin1String("CursorProximity"));
    QCOMPARE(profile.getForceProportions(), QLatin1String("ForceProportions"));
    QCOMPARE(profile.getGesture(), QLatin1String("Gesture"));
    QCOMPARE(profile.getInvertScroll(), QLatin1String("InvertScroll"));
    QCOMPARE(profile.getMapToOutput(), QLatin1String("MapToOutput"));
    QCOMPARE(profile.getMode(), QLatin1String("Mode"));
    QCOMPARE(profile.getName(), QLatin1String("Name"));
    QCOMPARE(profile.getPressureCurve(), QLatin1String("PressureCurve"));
    QCOMPARE(profile.getRawSample(), QLatin1String("RawSample"));
    QCOMPARE(profile.getRelWheelDown(), QLatin1String("RelWheelDown"));
    QCOMPARE(profile.getRelWheelUp(), QLatin1String("RelWheelUp"));
    QCOMPARE(profile.getRotate(), QLatin1String("Rotate"));
    QCOMPARE(profile.getRotateWithScreen(), QLatin1String("RotateWithScreen"));
    QCOMPARE(profile.getScreenMapping(), QLatin1String("ScreenMapping"));
    QCOMPARE(profile.getScreenSpace(), QLatin1String("ScreenSpace"));
    QCOMPARE(profile.getScrollDistance(), QLatin1String("ScrollDistance"));
    QCOMPARE(profile.getStripLeftDown(), QLatin1String("StripLeftDown"));
    QCOMPARE(profile.getStripLeftUp(), QLatin1String("StripLeftUp"));
    QCOMPARE(profile.getStripRightDown(), QLatin1String("StripRightDown"));
    QCOMPARE(profile.getStripRightUp(), QLatin1String("StripRightUp"));
    QCOMPARE(profile.getSuppress(), QLatin1String("Suppress"));
    QCOMPARE(profile.getTabletArea(), QLatin1String("TabletArea"));
    QCOMPARE(profile.getTabletPcButton(), QLatin1String("TabletPcButton"));
    QCOMPARE(profile.getTapTime(), QLatin1String("TapTime"));
    QCOMPARE(profile.getThreshold(), QLatin1String("Threshold"));
    QCOMPARE(profile.getTouch(), QLatin1String("Touch"));
    QCOMPARE(profile.getZoomDistance(), QLatin1String("ZoomDistance"));
}


void DeviceProfileTestUtils::setValues(DeviceProfile& profile)
{
    profile.setAbsWheel2Down(QLatin1String("AbsWheel2Down"));
    profile.setAbsWheel2Up(QLatin1String("AbsWheel2Up"));
    profile.setAbsWheelDown(QLatin1String("AbsWheelDown"));
    profile.setAbsWheelUp(QLatin1String("AbsWheelUp"));
    profile.setArea(QLatin1String("Area"));
    profile.setButton(1, QLatin1String("Button1"));
    profile.setButton(2, QLatin1String("Button2"));
    profile.setButton(3, QLatin1String("Button3"));
    profile.setButton(4, QLatin1String("Button4"));
    profile.setButton(5, QLatin1String("Button5"));
    profile.setButton(6, QLatin1String("Button6"));
    profile.setButton(7, QLatin1String("Button7"));
    profile.setButton(8, QLatin1String("Button8"));
    profile.setButton(9, QLatin1String("Button9"));
    profile.setButton(10, QLatin1String("Button10"));
    profile.setChangeArea(QLatin1String("ChangeArea"));
    profile.setCursorProximity(QLatin1String("CursorProximity"));
    profile.setForceProportions(QLatin1String("ForceProportions"));
    profile.setGesture(QLatin1String("Gesture"));
    profile.setInvertScroll(QLatin1String("InvertScroll"));
    profile.setMapToOutput(QLatin1String("MapToOutput"));
    profile.setMode(QLatin1String("Mode"));
    profile.setName(QLatin1String("Name"));
    profile.setPressureCurve(QLatin1String("PressureCurve"));
    profile.setRawSample(QLatin1String("RawSample"));
    profile.setRelWheelDown(QLatin1String("RelWheelDown"));
    profile.setRelWheelUp(QLatin1String("RelWheelUp"));
    profile.setRotate(QLatin1String("Rotate"));
    profile.setRotateWithScreen(QLatin1String("RotateWithScreen"));
    profile.setScreenMapping(QLatin1String("ScreenMapping"));
    profile.setScreenSpace(QLatin1String("ScreenSpace"));
    profile.setScrollDistance(QLatin1String("ScrollDistance"));
    profile.setStripLeftDown(QLatin1String("StripLeftDown"));
    profile.setStripLeftUp(QLatin1String("StripLeftUp"));
    profile.setStripRightDown(QLatin1String("StripRightDown"));
    profile.setStripRightUp(QLatin1String("StripRightUp"));
    profile.setSuppress(QLatin1String("Suppress"));
    profile.setTabletArea(QLatin1String("TabletArea"));
    profile.setTabletPcButton(QLatin1String("TabletPcButton"));
    profile.setTapTime(QLatin1String("TapTime"));
    profile.setThreshold(QLatin1String("Threshold"));
    profile.setTouch(QLatin1String("Touch"));
    profile.setZoomDistance(QLatin1String("ZoomDistance"));
}
