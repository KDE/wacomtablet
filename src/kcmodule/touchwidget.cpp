/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "touchwidget.h"
#include "ui_touchwidget.h"

#include "profilemanagement.h"

// common includes
#include "property.h"
#include "deviceprofile.h"

// stdlib
#include <memory>

//Qt includes
#include <QtCore/QStringList>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

using namespace Wacom;

namespace Wacom {
/**
  * Private class for the d-pointer.
  */
class TouchWidgetPrivate {
    public:
        std::auto_ptr<Ui::TouchWidget>  m_ui;                /**< Handler to the touchwidget.ui file */
}; // CLASS
}  // NAMESPACE


TouchWidget::TouchWidget( QWidget *parent )
    : QWidget( parent ), d_ptr(new TouchWidgetPrivate)
{
    Q_D( TouchWidget );

    d->m_ui = std::auto_ptr<Ui::TouchWidget>(new Ui::TouchWidget);
    d->m_ui->setupUi( this );

    reloadWidget();
}

TouchWidget::~TouchWidget()
{
    delete this->d_ptr;
}



void TouchWidget::saveToProfile()
{
    Q_D( TouchWidget );

    DeviceProfile touchProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Touch );

    if( d->m_ui->touchEventsCheckBox->isChecked() ) {
        touchProfile.setProperty( Property::Touch, QLatin1String("on") );
    }
    else {
        touchProfile.setProperty( Property::Touch, QLatin1String("off") );
    }

    if( d->m_ui->scrollDirection->isChecked() ) {
        touchProfile.setProperty( Property::InvertScroll, QLatin1String("on") );
        touchProfile.setProperty( Property::Button4, QLatin1String("5") );
        touchProfile.setProperty( Property::Button5, QLatin1String("4") );
    }
    else {
        touchProfile.setProperty( Property::InvertScroll, QLatin1String("off") );
        touchProfile.setProperty( Property::Button4, QLatin1String("4") );
        touchProfile.setProperty( Property::Button5, QLatin1String("5") );
    }
    if( d->m_ui->radioButton_Absolute->isChecked() ) {
        touchProfile.setProperty( Property::Mode, QLatin1String("absolute") );
    }
    else {
        touchProfile.setProperty( Property::Mode, QLatin1String("relative") );
    }

    if( d->m_ui->gesturesCheckBox->isChecked() ) {
        touchProfile.setProperty( Property::Gesture, QLatin1String("on") );
    }
    else {
        touchProfile.setProperty( Property::Gesture, QLatin1String("off") );
    }

    touchProfile.setProperty( Property::ZoomDistance, QString::number(d->m_ui->zoomDistanceBox->value()) );
    touchProfile.setProperty( Property::ScrollDistance, QString::number(d->m_ui->scrollDistanceBox->value()) );
    touchProfile.setProperty( Property::TapTime, QString::number(d->m_ui->tapTimeBox->value()) );

    ProfileManagement::instance().saveDeviceProfile(touchProfile);
}

void TouchWidget::loadFromProfile()
{
    Q_D( TouchWidget );

    DeviceProfile touchProfile = ProfileManagement::instance().loadDeviceProfile( DeviceType::Touch );

    QString touch = touchProfile.getProperty( Property::Touch );
    if( touch == QLatin1String( "on" ) ) {
        d->m_ui->touchEventsCheckBox->setChecked( true );
    }
    else {
        d->m_ui->touchEventsCheckBox->setChecked( false );
    }

    QString scrollDirection = touchProfile.getProperty( Property::InvertScroll );
    if( scrollDirection == QLatin1String( "on" ) ) {
        d->m_ui->scrollDirection->setChecked( true );
    }
    else {
        d->m_ui->scrollDirection->setChecked( false );
    }

    QString fingerMode = touchProfile.getProperty( Property::Mode );
    if( fingerMode == QLatin1String( "absolute" ) ) {
        d->m_ui->radioButton_Absolute->setChecked( true );
    }
    else {
        d->m_ui->radioButton_Relative->setChecked( true );
    }

    QString gesture = touchProfile.getProperty( Property::Gesture );
    if( gesture == QLatin1String( "on" ) ) {
        d->m_ui->gesturesCheckBox->setChecked( true );
    }
    else {
        d->m_ui->gesturesCheckBox->setChecked( false );
    }

    int zoomDistance = touchProfile.getProperty( Property::ZoomDistance ).toInt();
    d->m_ui->zoomDistanceBox->setValue( zoomDistance );

    int scrollDistance = touchProfile.getProperty( Property::ScrollDistance ).toInt();
    d->m_ui->scrollDistanceBox->setValue( scrollDistance );

    int tapTime = touchProfile.getProperty( Property::TapTime ).toInt();
    d->m_ui->tapTimeBox->setValue( tapTime );
}

void TouchWidget::profileChanged()
{
    emit changed();
}

void TouchWidget::reloadWidget() { }
