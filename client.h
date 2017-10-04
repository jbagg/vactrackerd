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
   File name    : client.h
   Created      : 01 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Object to handle connected clients.  Login & command validation
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef CLIENT_H_
#define CLIENT_H_

#include <QTcpSocket>
#include "user.h"
#include "region.h"
#include "server.h"

class Server;
class User;
class GeoRegion;

class Client : public QObject
{
	Q_OBJECT

public:
	Client(Server *parent, QTcpSocket *newConnection);
	inline void setActiveUser(User *value) {activeUser = value;}
	inline User *getActiveUser(void) {return activeUser;}
	inline User *getUser(void) {return user;}
	inline void setActiveRegion(GeoRegion *value) {activeRegion = value;}
	inline void setErrorMsg(QString value) {errorMsg = value;}
	inline void write(const char *value) {socket->write(value);}

private:
	bool cmdAuthCheck(QString cmd);
	bool logIn(QString cmd);
	Server *server;
	GeoRegion *activeRegion;
	User *user, *activeUser;
	QTcpSocket *socket;
	QString errorMsg;
	bool auth;


private slots:
	void rx(void);
	void disconnect(void);

};

#endif /* CLIENT_H_ */
