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
   File name    : regionparser.cpp
   Created      : 08 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Fixme
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "regionparser.h"
#include "region.h"

RegionParser::RegionParser()
{
	actions << "lock" << "unlock" << "refresh?";
}

bool RegionParser::editable(QObject *ref)
{
	GeoRegion *region;

	region = static_cast<GeoRegion*>(ref);
	if (region->lockedBy)
		return 1;
	else
		return 0;
}

bool RegionParser::parseObjects(QObject *ref, Client *client, QString type, QString id, QString tail)
{
	GeoRegion *region;
	bool found = 0;
	RegionYear *rY;

	region = static_cast<GeoRegion*>(ref);
	if (type == "year") {
		if (!region->regionYears.contains(id) && client->getUser()->getType() == UT_ADMIN) {
			rY = new RegionYear(REGION_DIR + region->name + "/" + id + ".xml");
			rY->setYear(id.toInt());
			region->regionYears[id] = rY;
		}
		if (region->regionYears.contains(id))
			found = regionYearParser.parseCmd(region->regionYears[id], client, tail);
	}

	return found;
}

bool RegionParser::parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32)
{
	GeoRegion *region;
	bool found = 0;

	region = static_cast<GeoRegion*>(ref);
	if (cmd == "lock") {
		found = region->lock(value.toInt(), client);
	}
	else if (cmd == "unlock") {
		found = region->unlock(client);
	}
	else if (cmd == "refresh?") {
		found = 1;
		client->setActiveRegion(region);
		QString response;
		region->dump(&response);
		client->write(response.toUtf8());
	}

	return found;
}
