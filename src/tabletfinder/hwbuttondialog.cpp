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

#include "hwbuttondialog.h"
#include "ui_hwbuttondialog.h"
#include <QX11Info>
#include <xcb/xcb.h>

using namespace Wacom;

HWButtonDialog::HWButtonDialog(int maxButtons, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HWButtonDialog)
    , m_maxButtons(maxButtons)
    , m_nextButton(1)
{
    ui->setupUi(this);

    ui->buttonBox->setEnabled(false);
    nextButton();
}

HWButtonDialog::~HWButtonDialog()
{
    delete ui;
}

bool HWButtonDialog::nativeEvent(const QByteArray& eventType, void* message, long int* result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    if (!QX11Info::isPlatformX11()) {
        return false;
    }

    xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);
    if ((event->response_type & ~0x80) != XCB_BUTTON_PRESS && m_nextButton <= m_maxButtons) {
        xcb_button_press_event_t* buttonEvent = static_cast<xcb_button_press_event_t *>(message);
        hwKey(buttonEvent->detail);
        return true;
    }

    return false;
}

void HWButtonDialog::nextButton()
{
    if(m_nextButton <= m_maxButtons) {
        QString text = i18n("Please press Button %1 on the tablet now.", m_nextButton);
        text.prepend(QLatin1String("<b>"));
        text.append(QLatin1String("</b>"));
        ui->textEdit->insertHtml(text);
        ui->textEdit->insertHtml(QLatin1String("<br>"));

        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->textEdit->setTextCursor(cursor);
    }
}

void HWButtonDialog::hwKey(unsigned int button)
{

    if(m_nextButton <= m_maxButtons) {
        QString text = i18n("Hardware button %1 detected.", button);
        ui->textEdit->insertHtml(text);
        ui->textEdit->insertHtml(QLatin1String("<br><br>"));
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->textEdit->setTextCursor(cursor);

        m_buttonMap << button;
        m_nextButton++;
    }

    if(m_nextButton > m_maxButtons) {
        ui->textEdit->insertHtml(i18n("All buttons detected. Please close dialog"));

        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->textEdit->setTextCursor(cursor);

        ui->buttonBox->setEnabled(true);
    }
    else {
        nextButton();
    }
}

QList<unsigned int> HWButtonDialog::buttonMap() const
{
    return m_buttonMap;
}
