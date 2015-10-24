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
   File name    : withdrawal.h
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Used to take from the vacation balance (when a new vacation request is made another instance of
   this class is made)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef WITHDRAWAL_H_
#define WITHDRAWAL_H_

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "withdrawalparser.h"
#include "user.h"

class User;

enum withdrawal_state {
	WDRL_REQUEST,
	WDRL_APPROVED,
	WDRL_REJECTED,
	WDRL_CANCEL_REQUEST,
	WDRL_CANCELLED,
};

class Withdrawal : public QObject
{
	Q_OBJECT

friend WithdrawalParser;

public:
	Withdrawal(User *parent, quint32 value);
	void load(QXmlStreamReader *xml);
	void save(QXmlStreamWriter *xml);
	void dump(QString *response);
	inline User *getUser(void) {return user;}
	inline void setState(withdrawal_state value) {state = value;}
	inline withdrawal_state getState(void) {return state;}
	inline quint32 getStart(void) {return start;}
	inline quint32 getEnd(void) {return end;}

private:
	quint32 id;
	withdrawal_state state;
	quint32	start, end, requestDate, approvedDate;
	qreal length;
	QString approvedBy;
	User *user;
};

#endif /* WITHDRAWAL_H_ */
