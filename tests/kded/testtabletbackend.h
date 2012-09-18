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

#ifndef TESTTABLETBACKEND_H
#define TESTTABLETBACKEND_H

#include <QtTest>
#include <QtCore>

#include "propertyadaptormock.h"
#include "tabletbackend.h"
#include "xinputproperty.h"
#include "xsetwacomproperty.h"


namespace Wacom
{
class TestTabletBackend : public QObject
{
    Q_OBJECT

public:


private slots:

    void initTestCase();

    void testGetInformation();

    void testSetDeviceProfile();

    void testSetProfile();
    
    void testSetProperty();

    void cleanupTestCase();


private:

    TabletInformation                       m_tabletInformation;
    TabletBackend*                          m_tabletBackend;

    PropertyAdaptorMock<XinputProperty>*    m_eraserXinputAdaptor;
    PropertyAdaptorMock<XsetwacomProperty>* m_eraserXsetwacomAdaptor;

    PropertyAdaptorMock<XinputProperty>*    m_padXinputAdaptor;
    PropertyAdaptorMock<XsetwacomProperty>* m_padXsetwacomAdaptor;

    PropertyAdaptorMock<XinputProperty>*    m_stylusXinputAdaptor;
    PropertyAdaptorMock<XsetwacomProperty>* m_stylusXsetwacomAdaptor;

}; // CLASS
}  // NAMESPACE

QTEST_MAIN(Wacom::TestTabletBackend)

#endif // HEADER PROTECTION
