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
   File name    : deposit.h
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Used to add to the vacation balance (like user earned vacation)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef DEPOSIT_H_
#define DEPOSIT_H_

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "depositparser.h"

class Deposit : public QObject
{
	Q_OBJECT

friend DepositParser;

public:
	void load(QXmlStreamReader *xml);
	void save(QXmlStreamWriter *xml);
	void dump(QString *response);
	inline void setID(quint32 value) {id = value;}
	inline void setYear(quint32 value) {year = value;}
	inline void setAmount(qreal value) {amount = value;}

private:
	quint32	id;
	quint32	year;
	qreal	amount;
};


#endif /* DEPOSIT_H_ */
