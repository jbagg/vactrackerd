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
   File name    : deposit.cpp
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Used to add to the vacation balance (like an annual allotment)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "deposit.h"

void Deposit::load(QXmlStreamReader *xml)
{
	QXmlStreamAttributes attributes = xml->attributes();
	if (attributes.hasAttribute("year"))
#if QT_VERSION >= 0x050000
		year = attributes.value("year").toInt();
#else
	{
		QString temp;
		temp.append(attributes.value("year"));
		year = temp.toInt();
	}
#endif

	xml->readNext();
	if (xml->tokenType() == QXmlStreamReader::Characters)
#if QT_VERSION >= 0x050000
		amount = xml->text().toFloat();
#else
	{
		QString temp;
		temp.append(xml->text());
		amount = temp.toFloat();
	}
#endif

}

void Deposit::save(QXmlStreamWriter *xml)
{
	xml->writeStartElement("deposit");
	xml->writeAttribute("year", QString::number(year));
	xml->writeCharacters(QString::number(amount));
	xml->writeEndElement();
}

void Deposit::dump(QString *response)
{
	*response+= "year=" + QString::number(year) + '&';
	*response+= "amount=" + QString::number(amount).replace('.', '#');
}
