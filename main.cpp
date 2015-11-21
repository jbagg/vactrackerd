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
   File name    : main.cpp
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDebug>
#include <QApplication>
#include <QStringList>
#include "qtzeroconf/qzeroconf.h"
#include "server.h"

#define SRV_PORT	11437

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QStringList args = app.arguments();

	app.setApplicationName("Vacation Tracker");
	app.setApplicationVersion("0.8rc1");

	if (args.size() == 2 && args.at(1) == "--version")
	{
		qDebug() << app.applicationName().toUtf8() << app.applicationVersion().toUtf8();
		return 0;
	}

	QZeroConf zeroConf;
	zeroConf.startServicePublish("Vaction-Tracker-server", "_vactracker._tcp", "local", SRV_PORT);

	Server server(SRV_PORT);

	return app.exec();
}
