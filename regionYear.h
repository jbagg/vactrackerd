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
   File name    : regionyear.h
   Created      : 26 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   A QList of statutory holidays for the year
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGIONYEAR_H_
#define REGIONYEAR_H_

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "parse.h"
#include "regionyearparser.h"

class RegionYear : public QObject
{
	Q_OBJECT

friend RegionYearParser;

public:
	RegionYear(QString fName);
	void load(void);
	void save(void);
	void dump(QString *response);
	inline void setYear(qint32 value) {year = value;}

private:
	qint32 year;
	QString fileName;
	QList <qint32> stats;

private slots:


};


#endif /* REGIONYEAR_H_ */
