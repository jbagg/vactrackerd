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
   File name    : regionyearparser.h
   Created      : 08 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Command parser for the Region Year class
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGIONYEARPARSER_H_
#define REGIONYEARPARSER_H_

#include "parse.h"

class RegionYearParser : public Parse
{
	Q_OBJECT

public:
	RegionYearParser();

private:
	bool parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32 n);
};


#endif /* REGIONYEARPARSER_H_ */
