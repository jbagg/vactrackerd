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
   File name    : withdrawal.cpp
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Used to take from the vacation balance (when a new vacation request is made another instance of
   this class is made)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "withdrawal.h"
#include "user.h"

Withdrawal::Withdrawal(User *parent, quint32 value)
{
	id = value;
	user = parent;
	approvedDate = 0;
}

void Withdrawal::load(QXmlStreamReader *xml)
{
	while (!xml->atEnd() && !xml->hasError()) {
		xml->readNext();
		if (xml->isEndElement() && xml->name() == "withdrawal")
			break;

		if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "start")
			start = User::getXmlInt(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "end")
			end = User::getXmlInt(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "length")
			length = User::getXmlFloat(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "request_date")
			requestDate = User::getXmlInt(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "approved_date")
			approvedDate = User::getXmlInt(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "approved_by")
			approvedBy = User::getXmlString(xml);
		else if (xml->tokenType() == QXmlStreamReader::StartElement && xml->name() == "state") {
			QString stateStr;
			stateStr = User::getXmlString(xml);
			if (stateStr == "request")
				state = WDRL_REQUEST;
			else if (stateStr == "approved")
				state = WDRL_APPROVED;
			else if (stateStr == "rejected")
				state = WDRL_REJECTED;
			else if (stateStr == "cancel_request")
				state = WDRL_CANCEL_REQUEST;
			else if (stateStr == "cancelled")
				state = WDRL_CANCELLED;
		}
	}
}

void Withdrawal::save(QXmlStreamWriter *xml)
{
	xml->writeStartElement("withdrawal");
	xml->writeStartElement("state");
	switch (state) {
		case WDRL_REQUEST: xml->writeCharacters("request"); break;
		case WDRL_APPROVED: xml->writeCharacters("approved");  break;
		case WDRL_REJECTED:	xml->writeCharacters("rejected"); break;
		case WDRL_CANCEL_REQUEST: xml->writeCharacters("cancel_request"); break;
		case WDRL_CANCELLED: xml->writeCharacters("cancelled"); break;
	}
	xml->writeEndElement();

	xml->writeStartElement("start");
	xml->writeCharacters(QString::number(start));
	xml->writeEndElement();
	xml->writeStartElement("end");
	xml->writeCharacters(QString::number(end));
	xml->writeEndElement();
	xml->writeStartElement("length");
	xml->writeCharacters(QString::number(length));
	xml->writeEndElement();
	xml->writeStartElement("request_date");
	xml->writeCharacters(QString::number(requestDate));
	xml->writeEndElement();
	xml->writeStartElement("approved_date");
	xml->writeCharacters(QString::number(approvedDate));
	xml->writeEndElement();
	xml->writeStartElement("approved_by");
	xml->writeCharacters(approvedBy.toUtf8());
	xml->writeEndElement();

	xml->writeEndElement();
}

void Withdrawal::dump(QString *response)
{
	switch (state) {
		case WDRL_REQUEST: *response+= "state=request&"; break;
		case WDRL_APPROVED: *response+= "state=approved&";  break;
		case WDRL_REJECTED:	*response+= "state=rejected&"; break;
		case WDRL_CANCEL_REQUEST: *response+= "state=cancel_request&"; break;
		case WDRL_CANCELLED: *response+= "state=cancelled&"; break;
	}
	*response+= "start=" + QString::number(start) + '&';
	*response+= "end=" + QString::number(end) + '&';
	*response+= "length=" + QString::number(length).replace('.', '#') + '&';
	*response+= "request_date=" + QString::number(requestDate) + '&';
	*response+= "approved_date=" + QString::number(approvedDate) + '&';
	*response+= "approved_by=" + approvedBy;
}
