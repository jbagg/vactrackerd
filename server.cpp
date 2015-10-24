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
   File name    : server.cpp
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   TCP Server for Vacation Tracker Clients to connect to.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDir>
#include <QDate>
#include "server.h"

Server::Server(qint32 port)
{
	// create dir for users if it doesn't exist (on first run)
	QDir dir(QDir::current().path() + "/users");
	if (!dir.exists())
		dir.mkdir(dir.path());

	loadRegions();
	loadUsers();

	if (!server.listen(QHostAddress::Any, port)) {
		qCritical() << "Unable to start the TCP server";
		exit(1);
		return;
	}

	actions << "allManagers?" << "regionList?";

	connect(&server, SIGNAL(newConnection()), this, SLOT(link()));
	connect(&periodicWorkTimer, SIGNAL(timeout()), this, SLOT(periodicWork()));


	connect(&userParser.withdrawalParser, SIGNAL(notify(Withdrawal *)), &emailNotifier, SLOT(notify(Withdrawal *)), Qt::QueuedConnection);
	emailNotifier.moveToThread(&emailThread);
	emailThread.start();

	periodicWorkTimer.start(1200000);	// 20 min
}

bool Server::parseObjects(QObject *, Client *client, QString type, QString id, QString tail)
{
	bool found = 0;
	
	if (type == "user") {
		if (!users.contains(id) && client->getUser()->getType() == UT_ADMIN) {
			User *newUser = new User(this, "users/" + id + ".xml");
			newUser->setID(id);
			newUser->forceManagementChainUpdate();	// force to update the admin(s) with this new user if no manger was set on creation
			users[id] = newUser;
			newUser->lock(0, client);
		}
		if (users.contains(id))
			found = userParser.parseCmd(users[id], client, tail);
	}
	else if (type == "region") {
		if (!regions.contains(id) && client->getUser()->getType() == UT_ADMIN) {
			GeoRegion *newRegion = new GeoRegion(this);
			newRegion->setName(id);
			regions[id] = newRegion;
		}
		if (regions.contains(id))
			found = regionParser.parseCmd(regions[id], client, tail);
	}

	return found;
}

bool Server::parseAction(QObject *, Client *client, QString cmd, QString, qint32)
{
	QString response;
	bool found = 0;
	qint32 i = 0;

	if (cmd == "allManagers?") {
		found = 1;
		response = "managers=";
		QMap <QString, User *>::iterator u;
		for (u = users.begin(); u != users.end(); u++) {
			if ((*u)->getType() == UT_NORMAL)
				continue;
			i++;
			response+= (*u)->getID() + ',';
		}
		if (i)
			response.remove(response.size()-1, 1);	// remove last comma only if list isn't zero length
		response+= '\n';
		client->write(response.toUtf8());
	}
	else if (cmd == "regionList?") {
		found = 1;
		response = "regionList=";
		QMap <QString, GeoRegion *>::iterator r;
		for (r = regions.begin(); r != regions.end(); r++)
			response+= (*r)->getName() + ',';
		if (regions.size())
			response.remove(response.size()-1, 1);  // remove last comma if > 0 items in regions
		response+= '\n';
		client->write(response.toUtf8());
	}

	return found;
}

void Server::loadUsers(void)
{
	qint32 i;
	User *user;
	bool foundAdmin = 0;

	QDir userDir("users");
	userDir.setNameFilters(QStringList() << "*.xml");
	QFileInfoList userFiles = userDir.entryInfoList();

	// load xml files
	for (i=0; i<userFiles.size(); i++) {
		user = new User(this, userFiles[i].filePath());
		user->load();
		users[user->getID()] = user;
		if (user->getType() == UT_ADMIN)
			foundAdmin = 1;
	}

	// build up manager relations
	QMap <QString, User *>::iterator u;
	for (u = users.begin(); u != users.end(); u++) {
		if ((*u)->getReportsTo().size()) {
			if (users.contains((*u)->getReportsTo())) {
				(*u)->setBoss(users[(*u)->getReportsTo()]);
			}
		}
	}

	if (!foundAdmin) {
		// create "admin" user to start
		user = new User(this, userDir.path() + "/admin.xml");
		user->setID("admin");
		user->setPwdHash("21232f297a57a5a743894a0e4a801fc3");	// password = admin
		user->setType(UT_ADMIN);
		users[user->getID()] = user;
	}
}

void Server::loadRegions(void)
{
	qint32 i;
	GeoRegion *region;

	QDir regionRoot(REGION_DIR);
	regionRoot.setFilter(QDir::AllDirs | QDir::NoDot | QDir::NoDotDot);
	QFileInfoList regionDirs = regionRoot.entryInfoList();

	// load xml files
	for (i=0; i<regionDirs.size(); i++) {
		region = new GeoRegion(this);
		region->setName(regionDirs[i].baseName());
		region->load(regionDirs[i].filePath());
		regions[region->getName()] = region;
	}
}

void Server::link(void)
{
	QTcpSocket *clientSocket;
	Client *client;

	clientSocket = server.nextPendingConnection();
	client = new Client(this, clientSocket);
	connect(clientSocket, SIGNAL(readyRead()), client, SLOT(rx()));
	connect(clientSocket, SIGNAL(disconnected()), client, SLOT(disconnect()));
}

void Server::periodicWork(void)
{
	static qint32 lastYear, yesterday;
	QMap <QString, User *>::iterator u;
	QDate now = QDate::currentDate();

	// Update users annual allotment
	if (lastYear && lastYear != now.year()) {
		for (u = users.begin(); u != users.end(); u++) {
			if ((*u)->getType() != UT_DISABLED)
				(*u)->addAnnualAllotment(now.year());
		}
	}
	lastYear = now.year();

	// look for expired requests and reject them
	if (yesterday && yesterday != now.dayOfYear()) {
		for (u = users.begin(); u != users.end(); u++)
			(*u)->checkForExpiredRequests(now.toJulianDay());
	}
	yesterday = now.dayOfYear();
}
