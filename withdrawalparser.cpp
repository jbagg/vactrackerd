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
   File name    : withdrawalparser.cpp
   Created      : 07 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Command parser for the Withdrawal class
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDate>
#include "withdrawalparser.h"
#include "withdrawal.h"

WithdrawalParser::WithdrawalParser()
{
	intProperties << "start" << "end" << "request_date" << "approved_date";
	floatProperties << "length";
	stringProperties << "approved_by";
	actions << "state";
}

bool WithdrawalParser::parseAction(QObject *ref, Client *, QString cmd, QString value, qint32)
{
	Withdrawal *withdrawal;
	bool found = 0;

	withdrawal = static_cast<Withdrawal*>(ref);
	if (cmd == "state") {
		found = 1;
		if (value == "request")
			withdrawal->state = WDRL_REQUEST;
		else if (value == "approved")
			withdrawal->state = WDRL_APPROVED;
		else if (value == "rejected")
			withdrawal->state = WDRL_REJECTED;
		else if (value == "cancel_request")
			withdrawal->state = WDRL_CANCEL_REQUEST;
		else if (value == "cancelled")
			withdrawal->state = WDRL_CANCELLED;

		emit notify(withdrawal);
	}

	return found;
}

void WithdrawalParser::parseInt(QObject *ref, QString id, qint32 value)
{
	Withdrawal *withdrawal;

	withdrawal = static_cast<Withdrawal*>(ref);
	if (id == "start")
		withdrawal->start = value;
	else if (id == "end")
		withdrawal->end = value;
	else if (id == "request_date")
		withdrawal->requestDate = value;
	else if (id == "approved_date")
		withdrawal->approvedDate = value;
}

void WithdrawalParser::parseFloat(QObject *ref, QString, qreal value)
{
	Withdrawal *withdrawal;

	withdrawal = static_cast<Withdrawal*>(ref);
	withdrawal->length = value;
}

void WithdrawalParser::parseString(QObject *ref, QString, QString value)
{
	Withdrawal *withdrawal;

	withdrawal = static_cast<Withdrawal*>(ref);
	withdrawal->approvedBy = value;
}
