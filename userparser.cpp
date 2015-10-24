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
   File name    : userparser.cpp
   Created      : 07 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Command parser for the User class
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "userparser.h"
#include "user.h"

UserParser::UserParser()
{
	intProperties << "annualAllotment";
	stringProperties << "firstName" << "lastName" << "email" << "pwdHash";
	actions << "refresh?" << "possibleManagers?" << "reportsTo" << "type" << "region" << "lock" << "unlock";
}

bool UserParser::editable(QObject *ref)
{
	User *user;

	user = static_cast<User*>(ref);
	if (user->lockedBy)
		return 1;
	else
		return 0;
}

bool UserParser::parseObjects(QObject *ref, Client *client, QString type, QString id, QString tail)
{
	User *user;
	Deposit *deposit;
	Withdrawal *withdrawal;
	qint32 i;
	bool found = 0;

	user = static_cast<User*>(ref);
	i = id.toInt();
	if (type == "withdrawal") {
		if (i > user->withdrawals.size()) {
			return 0;
		}
		if (i == user->withdrawals.size()) {
			withdrawal = new Withdrawal(user, i);
			user->withdrawals.append(withdrawal);
		}
		if (i < user->withdrawals.size())
			found = withdrawalParser.parseCmd(user->withdrawals[i], client, tail);
	}
	else if (type == "deposit") {
		if (i > user->deposits.size()) {
			return 0;
		}
		if (i == user->deposits.size() && client->getUser()->type == UT_ADMIN) {		// only admins can add deposits
			deposit = new Deposit;
			deposit->setID(i);
			user->deposits.append(deposit);
		}
		if (i < user->deposits.size())
			found = depositParser.parseCmd(user->deposits[i], client, tail);
	}

	return found;
}

bool UserParser::parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32)
{
	User *user;
	bool found = 0;

	user = static_cast<User*>(ref);
	if (cmd == "refresh?") {
		found = 1;
		client->setActiveUser(user);
		if (user->region) {
			QString response;
			user->region->dump(&response);
			client->write(response.toUtf8());
		}
		user->dump(client);
	}
	else if (cmd == "possibleManagers?") {
		found = 1;
		user->dumpPossibleManagers(client);
	}
	else if (cmd == "lock") {
		found = user->lock(value.toInt(), client);
	}
	else if (cmd == "unlock") {
		found = user->unlock(client);
	}
	else if (!user->lockedBy) {
		found = 0;
	}
	else if (cmd == "reportsTo" && client->getUser()->type == UT_ADMIN) {	// only admins can change reportsTo
		if (value == user->reportsTo)
			found = 1;
		else if (!value.size()) {		// value is blank -> unsetting (clearing) boss field
			if (user->boss) {
				user->oldBoss = user->boss;
				user->boss->employees.removeAll(user);	// remove this user from old boss' employee list
				user->boss = NULL;
				user->reportsTo.clear();
				user->updateManagersFlag = 1;
				found = 1;
			}
		}
		else {
			if (user->server->hasUser(value)) {
				User *newBoss;
				newBoss = user->server->getUser(value);
				if (!newBoss->authLookUp(user)) {			// check newBoss is not lower in chain of command
					user->updateManagersFlag = 1;
					if (user->boss) {
						user->oldBoss = user->boss;
						user->boss->employees.removeAll(user);	// remove this user from old boss' employee list
					}
					newBoss->employees.append(user);
					user->boss = newBoss;
					user->reportsTo = value;
					found = 1;
				}
				else
					qWarning() << "error Manager loop detected";
			}
		}
	}
	else if (cmd == "type" && client->getUser()->type == UT_ADMIN) {	// only admins can change user type
		if (client->getUser() != user || value == "admin") {			// admins can not change themselves to non admin
			found = 1;
			if (value == "normal")
				user->type = UT_NORMAL;
			else if (value == "manager")
				user->type = UT_MANAGER;
			else if (value == "admin")
				user->type = UT_ADMIN;
			else if (value == "disabled")
				user->type = UT_DISABLED;
		}
	}
	else if (cmd == "region" && client->getUser()->type == UT_ADMIN)
	{
		if (user->server->hasRegion(value)) {
			found = 1;
			user->region = user->server->getRegion(value);
		}
	}

	return found;
}

void UserParser::parseInt(QObject *ref, QString, qint32 value)
{
	User *user;

	user = static_cast<User*>(ref);
	user->annualAllotment = value;
}

void UserParser::parseString(QObject *ref, QString id, QString value)
{
	User *user;

	user = static_cast<User*>(ref);
	//"firstName" << "lastName" << "email" << "pwdHash";
	if (id == "firstName")
		user->firstName = value;
	else if (id == "lastName")
		user->lastName = value;
	else if (id == "email")
		user->email = value;
	else if (id == "pwdHash")
		user->pwdHash = value;
}
