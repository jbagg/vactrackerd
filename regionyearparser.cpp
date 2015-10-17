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
   File name    : regionyearparser.cpp
   Created      : 08 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Command parser for the Region Year class
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "regionyearparser.h"
#include "regionYear.h"

RegionYearParser::RegionYearParser()
{
	actions << "stat";
}

bool RegionYearParser::parseAction(QObject *ref, Client *, QString, QString value, qint32 n)
{
	RegionYear *regionYear;

	regionYear = static_cast<RegionYear*>(ref);

	if (!n)
		regionYear->stats.clear();
	regionYear->stats.append(value.toInt());

	return 1;
}
