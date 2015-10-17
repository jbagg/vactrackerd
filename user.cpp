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
   File name    : user.cpp
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   This is the user class which holds withdrawals, deposits and clients as well as general info
   about the user.  An instance of this class exists for each user.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QFile>
#include <QDebug>
#include "user.h"
#include "region.h"

User::User(Server *srv, QString fName)
{
	server = srv;
	fileName = fName;
	lockedBy = NULL;
	boss = NULL;
	oldBoss = NULL;
	region = NULL;
	version = 0;
	annualAllotment = 0;
	updateManagersFlag = 0;

	connect(this, SIGNAL(replicate()), this, SLOT(updateManagerChain()), Qt::QueuedConnection);
}

void User::load(void)
{
	Deposit *deposit;
	Withdrawal *withdrawal;

	QFile userFile(fileName);
	userFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QXmlStreamReader xml(&userFile);

	while (!xml.atEnd() && !xml.hasError())	{
		xml.readNext();
		if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "id")
			id = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "first_name")
			firstName = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "last_name")
			lastName = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "email")
			email = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "pwd_hash")
			pwdHash = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "type") {
			QString typeStr;
			typeStr = User::getXmlString(&xml);
			if (typeStr == "normal")
				type = UT_NORMAL;
			else if (typeStr == "manager")
				type = UT_MANAGER;
			else if (typeStr == "admin")
				type = UT_ADMIN;
			else if (typeStr == "disabled")
				type = UT_DISABLED;
		}
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "reports_to")
			reportsTo = getXmlString(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "region") {
			QString regionID = getXmlString(&xml);
			if (server->hasRegion(regionID))
				region = server->getRegion(regionID);
		}
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "annualAllotment")
			annualAllotment = getXmlInt(&xml);
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "deposit") {
			deposit = new Deposit;
			deposits.append(deposit);
			deposit->setID(deposits.indexOf(deposit));
			deposit->load(&xml);
		}
		else if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "withdrawal") {
			withdrawal = new Withdrawal(withdrawals.size());
			withdrawals.append(withdrawal);
			withdrawal->load(&xml);
		}

	}
	userFile.close();
}

void User::save(void)
{
	qint32 i;

	QFile userFile(fileName);
	userFile.open(QIODevice::WriteOnly);
	QXmlStreamWriter xml(&userFile);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("user");

	xml.writeStartElement("id");
	xml.writeCharacters(id.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("first_name");
	xml.writeCharacters(firstName.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("last_name");
	xml.writeCharacters(lastName.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("email");
	xml.writeCharacters(email.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("pwd_hash");
	xml.writeCharacters(pwdHash.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("type");
	switch (type) {
		case UT_NORMAL: xml.writeCharacters("normal"); break;
		case UT_MANAGER: xml.writeCharacters("manager"); break;
		case UT_ADMIN: xml.writeCharacters("admin"); break;
		case UT_DISABLED: xml.writeCharacters("disabled"); break;
	}
	xml.writeEndElement();
	xml.writeStartElement("reports_to");
	xml.writeCharacters(reportsTo.toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("region");
	if (region)
		xml.writeCharacters(region->getName().toUtf8());
	xml.writeEndElement();
	xml.writeStartElement("annualAllotment");
	xml.writeCharacters(QString::number(annualAllotment));
	xml.writeEndElement();

	for (i=0; i<deposits.size(); i++)
		deposits.at(i)->save(&xml);

	for (i=0; i<withdrawals.size(); i++)
		withdrawals.at(i)->save(&xml);

	xml.writeEndElement(); // user
	xml.writeEndDocument();

	userFile.close();
}

bool User::authLookUp(User *manager)
{
	bool found = 0;

	if (boss) {
		if (boss == manager)
			found = 1;
		else
			found = boss->authLookUp(manager);
	}

	return found;
}

bool User::lock(quint32 clientVersion, Client *client)
{
	bool success = 0;

	if (!lockedBy && clientVersion == version) {
		lockedBy = client;
		success = 1;
	}
	else if (lockedBy)
		client->setErrorMsg(firstName + ' ' + lastName +
		tr("'s account is currently being edited by ") +
		lockedBy->getUser()->firstName + ' ' + lockedBy->getUser()->lastName);

	return success;
}

bool User::unlock(Client *client)
{
	bool success = 0;

	if (lockedBy == client) {
		lockedBy = NULL;
		version++;
		save();
		emit replicate();
		success = 1;
	}

	return success;
}

void User::updateManagerChain()
{
	User *tell;
	qint32 i;

	tell = this;
	while (tell) {
		// send to all clients associated with the user
		for (i=0; i<tell->clients.size(); i++) {
			if (tell->clients[i]->getActiveUser() == this)
				dump(tell->clients[i]);
			if (updateManagersFlag && tell != this)
				tell->dumpEmployees(tell->clients[i]);	// this is to update the new manager's drop down employee list
		}
		tell = tell->boss;
	}

	// this is to remove this user from the old manager's (+ old chain) drop down employee list
	if (updateManagersFlag) {
		tell = oldBoss;
		while (tell) {
			if (authLookUp(tell))	// We've made it up the chain to a common management point
				break;				//go no further, managers above are already updated from above loop
			if (tell->type == UT_ADMIN)		// admins are updated below, don't do twice
				break;
			// send to all clients associated with the manager
			for (i=0; i<tell->clients.size(); i++) {
				tell->dumpEmployees(tell->clients[i]);	// this is to update the manager's employee list
			}
			tell = tell->boss;
		}
	}

	// update all the admins too!
	QMap <QString, User *>::iterator u;
	for (u = server->firstUser(); u != server->endUser(); u++) {
		if ((*u)->type == UT_ADMIN) {
			for (i=0; i<(*u)->clients.size(); i++) {
				if ((*u)->clients[i]->getActiveUser() == this)
					dump((*u)->clients[i]);
				if (updateManagersFlag && (*u) != this)
					(*u)->dumpEmployees((*u)->clients[i]);	// this is to update the manager's employee list
			}
		}
	}
	updateManagersFlag = 0;
}

void User::dump(Client *client)
{
	QString response;
	qint32 i;

	response = "user[" + id + "].version=" + QString::number(version);
	response+= "&firstName=" + firstName;
	response+= "&lastName=" + lastName;
	response+= "&email=" + email;
	response+= "&type=";
	switch (type) {
		case UT_NORMAL: response+= "normal"; break;
		case UT_MANAGER: response+= "manager"; break;
		case UT_ADMIN: response+= "admin"; break;
		case UT_DISABLED: response+= "disabled"; break;
	}
	response+= "&reportsTo=" + reportsTo;
	response+= "&annualAllotment=" + QString::number(annualAllotment);
	if (region)
		response+= "&region=" + region->getName();
	response+= '\n';
	response+= "user[" + id + "].employees=";
	buildEmployeeListResponce(&response);
	for(i=0; i<deposits.size(); i++) {
		response+= "user[" + id + "].deposit[" + QString::number(i) + "].";
		deposits[i]->dump(&response);
		response+= '\n';
	}
	for(i=0; i<withdrawals.size(); i++) {
		response+= "user[" + id + "].withdrawal[" + QString::number(i) + "].";
		withdrawals[i]->dump(&response);
		response+= '\n';
	}
	response+= "user[" + id + "].update\n";
	client->write(response.toUtf8());
}

void User::dumpEmployees(Client *client)
{
	QString response;

	response = "user[" + id + "].employees=";
	buildEmployeeListResponce(&response);
	response+= "user[" + id + "].update\n";
	client->write(response.toUtf8());
}

void User::buildEmployeeList(QString *response, qint32 level)
{
	qint32 i;

	level--;
	for(i=0; i<employees.size(); i++) {
		*response+= employees[i]->id;
		*response+= ',';
		if (level)
			employees[i]->buildEmployeeList(response, level);
	}
	level++;
}

void User::buildEmployeeListResponce(QString *response)
{
	qint32 n;

	n = response->size();

	if (type == UT_ADMIN) {						// admins get the whole user list
		QMap <QString, User *>::iterator u;
		for (u = server->firstUser(); u != server->endUser(); u++) {
			if (*u == this)	// don't include this user/admin in the list (so it doesn't get listed twice)
				continue;
			*response+= (*u)->id;
			if (u != server->endUser())
				*response+= ',';
		}
	}
	else
		buildEmployeeList(response, 2);

	if (response->size() > n)
		response->remove(response->size()-1, 1);	// remove last comma only if list isn't zero length
	*response+= '\n';
}

void User::dumpPossibleManagers(Client *client)
{
	QString response;
	QList <User *> managers;
	QMap <QString, User *>::iterator u;
	qint32 i;

	// make a list of all managers
	for (u = server->firstUser(); u != server->endUser(); u++) {
		if ((*u)->type == UT_NORMAL)
			continue;
		managers.append(*u);
	}
	managers.removeAll(this);			// remove so user can't select them self as a manager
	cullPossibleManagers(&managers);	// remove managers below this user in the management chain

	response = "user[" + id + "].possibleManagers=";
	for(i=0; i<managers.size(); i++)
		response+= managers[i]->id + ',';
	if (i > 0)
		response.remove(response.size()-1, 1);	// remove last comma
	response+= '\n';
	client->write(response.toUtf8());
}

void User::cullPossibleManagers(QList <User *> *list)
{
	qint32 i;

	for(i=0; i<employees.size(); i++) {
		if (employees[i]->type == UT_NORMAL)
			continue;
		list->removeAll(employees[i]);
		employees[i]->cullPossibleManagers(list);
	}
}

void User::addAnnualAllotment(qint32 year)
{
	Deposit *deposit;

	deposit = new Deposit;
	deposit->setYear(year);
	deposit->setAmount(annualAllotment);
	deposits[deposits.size()] = deposit;
	save();
	emit replicate();
}

void User::checkForExpiredRequests(quint32 today)
{
	qint32 i;
	bool update = 0;

	for(i=0; i<withdrawals.size(); i++) {
		if (withdrawals[i]->getState() == WDRL_REQUEST && withdrawals[i]->getStart() <= today) {
			withdrawals[i]->setState(WDRL_REJECTED);
			update = 1;
		}
	}

	if (update) {
		save();
		emit replicate();
	}
}

void User::forceManagementChainUpdate(void)
{
	updateManagersFlag = 1;
}
