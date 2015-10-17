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
   File name    : region.h
   Created      : 26 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Saves and loads a QMap of regionYears each which contain a list of statutory holidays
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGION_H_
#define REGION_H_

#include <QObject>
#include <QTcpSocket>
#include "parse.h"
#include "server.h"
#include "regionYear.h"
#include "regionparser.h"

#define	REGION_DIR	"regions/"

class Server;

class GeoRegion : public QObject
{
	Q_OBJECT

friend RegionParser;

public:
	GeoRegion(Server *srv);
	void load(QString dirToOpen);
	void save(void);
	void dump(QString *response);
	bool lock(quint32 clientVersion, Client *client);
	bool unlock(Client *client);
	inline QString getName(void) {return name;}
	inline void setName(QString value) {name = value;}

signals:
	void replicate(void);

private:
	QString name;
	QMap <QString, RegionYear *> regionYears;
	Server *server;
	Client *lockedBy;
	quint32 version;

private slots:
	void updateUsers(void);

};


#endif /* REGION_H_ */
