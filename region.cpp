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
   File name    : region.cpp
   Created      : 26 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Saves and loads a QMap of regionYears each which contain a list of statutory holidays
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDir>
#include <QFile>
#include "region.h"
#include "client.h"

GeoRegion::GeoRegion(Server *srv)
{
	server = srv;
	version = 0;
	lockedBy = NULL;

	connect(this, SIGNAL(replicate()), this, SLOT(updateUsers()), Qt::QueuedConnection);
}

void GeoRegion::load(QString dirToOpen)
{
	qint32 i;
	RegionYear *rY;

	QDir regionDir(dirToOpen);
	regionDir.setNameFilters(QStringList() << "*.xml");
	QFileInfoList yearFiles = regionDir.entryInfoList();

	// load xml files
	for (i=0; i<yearFiles.size(); i++) {
		rY = new RegionYear(yearFiles[i].filePath());
		rY->load();
		rY->setYear(yearFiles[i].baseName().toInt());
		regionYears[yearFiles[i].baseName()] = rY;
	}
}

void GeoRegion::save(void)
{
	QMap <QString, RegionYear *>::iterator rY;

	// create dir for region if it doesn't exist
	QDir dir(QDir::current().path() + "/" + REGION_DIR + name);
	if (!dir.exists())
		dir.mkdir(dir.path());

	for (rY = regionYears.begin(); rY != regionYears.end(); rY++)
		(*rY)->save();
}

void GeoRegion::dump(QString *response)
{
	QMap <QString, RegionYear *>::iterator rY;

	*response+= "region[" + name + "].version=" + QString::number(version) + "\n";
	for (rY = regionYears.begin(); rY != regionYears.end(); rY++) {
		*response+= "region[" + name + "].";
		(*rY)->dump(response);
	}
}

bool GeoRegion::lock(quint32 clientVersion, Client *client)
{
	bool success = 0;

	if (!lockedBy && clientVersion == version) {
		lockedBy = client;
		success = 1;
	}

	return success;
}

bool GeoRegion::unlock(Client *client)
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

void GeoRegion::updateUsers(void)
{
	QString response;
	qint32 i;

	dump(&response);

	QMap <QString, User *>::iterator u;
	for (u = server->firstUser(); u != server->endUser(); u++) {
		if ((*u)->getRegion() != this)
			continue;
		for (i=0; i<(*u)->getClientTotal(); i++) {
			if ((*u)->getClientAt(i)->getActiveUser() == *u)
				(*u)->getClientAt(i)->write(response.toUtf8());
		}
	}
}
