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
   File name    : server.h
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   TCP Server for Vacation Tracker Clients to connect to.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef SERVER_H_
#define SERVER_H_

#include <QObject>
#include <QTcpServer>
#include <QMap>
#include <QTimer>
#include "region.h"
#include "user.h"
#include "client.h"
#include "userparser.h"
#include "regionparser.h"

class Client;
class User;
class GeoRegion;

class Server : public Parse
{
	Q_OBJECT

public:
	Server(qint32 port);
	inline bool hasRegion(QString regionID) {return regions.contains(regionID);}
	inline GeoRegion *getRegion(QString regionID) {return regions[regionID];}
	inline bool hasUser(QString userID) {return users.contains(userID);}
	inline User *getUser(QString userID) {return users[userID];}
	inline QMap<QString, User*>::iterator firstUser(void) {return users.begin();}
	inline QMap<QString, User*>::iterator endUser(void) {return users.end();}

private:
	void loadUsers(void);
	void loadRegions(void);
	bool parseObjects(QObject *ref, Client *client, QString type, QString id, QString tail);
	bool parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32 n);
	QTcpServer server;
	QTimer periodicWorkTimer;
	QMap <QString, GeoRegion *> regions;
	QMap <QString, User *> users;
	UserParser userParser;
	RegionParser regionParser;

private slots:
	void link(void);
	void periodicWork(void);

};

#endif /* SERVER_H_ */
