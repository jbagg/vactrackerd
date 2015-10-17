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
   File name    : regionparser.h
   Created      : 08 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Fixme
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGIONPARSER_H_
#define REGIONPARSER_H_

#include "parse.h"
#include "regionyearparser.h"

class RegionParser : public Parse
{
	Q_OBJECT

public:
	RegionParser();

private:
	bool parseObjects(QObject *ref, Client *client, QString type, QString id, QString tail);
	bool parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32 n);
	bool editable(QObject *);
	RegionYearParser regionYearParser;
};


#endif /* REGIONPARSER_H_ */
