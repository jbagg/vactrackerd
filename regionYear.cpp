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
#include <QFile>
#include "regionYear.h"

RegionYear::RegionYear(QString fName)
{
	fileName = fName;
}

void RegionYear::load()
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QXmlStreamReader xml(&file);

	while (!xml.atEnd() && !xml.hasError())	{
		xml.readNext();
		if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "stat") {
			xml.readNext();
			if (xml.tokenType() == QXmlStreamReader::Characters)
#if QT_VERSION >= 0x050000
				stats.append(xml.text().toInt());
#else
			{
				QString temp;
				temp.append(xml.text());
				stats.append(temp.toInt());
			}
#endif
		}
	}

	file.close();
}

void RegionYear::save(void)
{
	qint32 i;

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);
	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("year");

	for (i=0; i<stats.size(); i++) {
		xml.writeStartElement("stat");
		xml.writeCharacters(QString::number(stats[i]));
		xml.writeEndElement();
	}

	xml.writeEndElement(); // year
	xml.writeEndDocument();

	file.close();
}

void RegionYear::dump(QString *response)
{
	qint32 i;

	*response+= "year[" + QString::number(year) + "].stat=";
	for (i=0; i<stats.size(); i++) {
		*response+= QString::number(stats[i]) + ',';
	}
	if (stats.size())
		response->remove(response->size()-1, 1);  // remove last comma if > 0 items in stats
	*response+= '\n';
}
