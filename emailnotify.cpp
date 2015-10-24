/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (C) 2015  Jonathan Bagg
	This file is part of Vactracker.

	Vactracker is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	Vactracker is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Vactracker; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
---------------------------------------------------------------------------------------------------
   Project name : Vacation Tracker daemon
   File name    : emailnotify.cpp
   Created      : 21 Oct 2015 - Back to the Future!
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Interface to libesmtp to send email notifications
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDate>
#include <QFile>
#include <QXmlStreamReader>
#include "emailnotify.h"
#include "libesmtp.h"
#include "user.h"

EmailNotify::EmailNotify()
{
	bool serverFound = 0, portFound = 0, replyFound = 0;
	QString port;

	configOk = 0;
	QFile cfgFile("email_config.xml");
	cfgFile.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!cfgFile.isOpen()) {
		qWarning() << tr("Unable to open Email Config File!");
		return;
	}

	QXmlStreamReader xml(&cfgFile);
	while (!xml.atEnd() && !xml.hasError())	{
		xml.readNext();
		if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "server") {
			server = User::getXmlString(&xml);
			if (server.size())
				serverFound = 1;
		}
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "port") {
			port = User::getXmlString(&xml);
			if (port.size()) {
				portFound = 1;
				server+= ':' + port;
			}
		}
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "reply") {
			reply = User::getXmlString(&xml);
			if (reply.size())
				replyFound = 1;
		}
	}

	if (!serverFound)
		qWarning() << tr("Email config has no server specified");
	if (!portFound)
		qWarning() << tr("Email config has no port specified");
	if (!replyFound)
		qWarning() << tr("Email config has no reply address specified");

	if (serverFound && portFound && replyFound)
		configOk = 1;

	cfgFile.close();
}

void EmailNotify::notify(Withdrawal *withdrawal)
{
	User *user;
	smtp_session_t smtp_session;
	smtp_message_t smtp_message;
	QString subject, txtState, To, ToBoss;
	QDate start, end;

	if (!configOk)
		return;

	user = withdrawal->getUser();

	switch (withdrawal->getState()) {
	case WDRL_REQUEST: txtState = tr("Request"); break;
	case WDRL_APPROVED: txtState = tr("Approved"); break;
	case WDRL_REJECTED: txtState = tr("Rejected"); break;
	case WDRL_CANCEL_REQUEST: txtState = tr("Cancellation Request"); break;
	case WDRL_CANCELLED: txtState = tr("Cancelled"); break;
	}
	start = QDate::fromJulianDay(withdrawal->getStart());
	end = QDate::fromJulianDay(withdrawal->getEnd());

	subject = tr("[Vacation - ") + user->getFirstName() + " " + user->getLastName() + "] ("
			+ txtState + ") " +
			QDate::shortMonthName(start.month()) + ' ' + QString::number(start.day()) + " - " +
			QDate::shortMonthName(end.month()) + ' ' + QString::number(end.day());

	To = user->getEmail();
	To.replace('#', '.');
	ToBoss = user->getBoss()->getEmail();
	ToBoss.replace('#', '.');

	smtp_session = smtp_create_session();
	smtp_message = smtp_add_message(smtp_session);

	smtp_set_header(smtp_message, "To", NULL, (const char *)To.toUtf8());

	smtp_set_server(smtp_session, server.toUtf8());

	smtp_set_reverse_path(smtp_message, reply.toUtf8());
	smtp_set_header(smtp_message, "Subject", (const char *)subject.toUtf8());
	smtp_set_header_option(smtp_message, "Subject", Hdr_OVERRIDE, 1);
	smtp_set_message_str(smtp_message, (void*)"Vacation Update");

	smtp_add_recipient(smtp_message, To.toUtf8());
	if (ToBoss.size())
		smtp_add_recipient(smtp_message, ToBoss.toUtf8());

	smtp_start_session(smtp_session);	// send messages
	smtp_destroy_session(smtp_session);
}
