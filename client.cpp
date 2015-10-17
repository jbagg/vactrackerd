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
   File name    : client.cpp
   Created      : 01 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Object to handle connected clients.  Login & command validation
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QRegExp>
#include <QStringList>
#include "client.h"

Client::Client(Server *parent, QTcpSocket *newConnection)
{
	user = NULL;
	activeUser = NULL;
	activeRegion = NULL;
	auth = 0;
	server = parent;
	socket = newConnection;
}

void Client::rx(void)
{
	qint32	i;

	rx_stream+= socket->read(1024);
	QStringList rx_cmds = rx_stream.split(QRegExp("[\r\n]"));
	for (i=0; i<rx_cmds.size()-1; i++)
	{
		if (auth) {
			if (cmdAuthCheck(rx_cmds[i]))
				socket->write("ok\n");
			else {
				socket->write("error=" + errorMsg.toUtf8() + '\n');
				errorMsg.clear();
			}
		}
		else
			auth = logIn(rx_cmds[i]);
	}

	if (rx_cmds.size() > 1)
	{
		rx_stream.clear();
		if (rx_cmds.at(i).size())		// if string size != 0, there is a partial command leftover
			rx_stream+= rx_cmds.at(i);
	}
}

void Client::disconnect(void)
{
	if (user) {
		// if this client has it's active user locked, unlock it
		if (activeUser)
			activeUser->unlock(this);
		if (activeRegion)
			activeRegion->unlock(this);
		user->removeClient(this);
	}
	delete this;
}

bool Client::cmdAuthCheck(QString cmd)
{
	QString type, id;
	bool okToProceed = 0;

	type = cmd.section('.', 0, 0);
	id = type.section('[', 1);
	id = id.section(']', 0, 0);
	type = type.section('[', 0, 0);

	if (user->getType() == UT_ADMIN)								// admins can edit anything
		okToProceed = 1;
	else if (type == "user" && server->hasUser(id)) {
		if (user == server->getUser(id))					// users are allowed to edit their own data
			okToProceed = 1;
		else if(server->getUser(id)->authLookUp(user))		// check if [id] is in the user's manager chain
			okToProceed = 1;
	}

	if (okToProceed)
		return server->parseCmd(this, this, cmd);
	else
		return 0;
}

bool Client::logIn(QString cmd)
{
	QString type, tail, id;
	bool ok = 0;

	type = cmd.section('.', 0, 0);
	id = type.section('[', 1);
	id = id.section(']', 0, 0);
	type = type.section('[', 0, 0);
	if (type == "user" && server->hasUser(id)) {
		if (server->getUser(id)->getType() != UT_DISABLED) {
			tail = cmd.section('.', 1);
			if (tail.contains("login=")) {
				if (tail.section('=', 1) == server->getUser(id)->getPwdHash())
					ok = 1;
			}
		}
	}

	if (ok) {
		socket->write("ok\n");
		user = server->getUser(id);
		user->addClient(this);
	}
	else
		socket->write("error=\n");

	return ok;
}
