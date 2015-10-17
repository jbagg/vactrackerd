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
   File name    : parse.cpp
   Created      : 29 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   This is an abstract class.  Properties like strings, ints and actions in subclasses can be
   registered.  When a registered property is detected, its value is updated in the subclass.  If
   the property is an action, the corresponding code is executed in the subclass's parseAction()
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "parse.h"
#include <QStringList>

bool Parse::parseCmd(QObject *ref, Client *client, QString cmd)
{
	QString type, tail, id;
	qint32 i, j;
	bool found = 0, missingPrpty = 0;

	if (cmd.contains('.'))	{
		type = cmd.section('.', 0, 0);
		tail = cmd.section('.', 1);

		id = type.section('[', 1);
		id = id.section(']', 0, 0);
		type = type.section('[', 0, 0);
		//qDebug() << type << id << tail;
		found = parseObjects(ref, client, type, id, tail);
	}
	else {
		QStringList prptList = cmd.split('&');
		for (i=0; i<prptList.size(); i++) {
			tail = prptList[i].section('=', 1);
			id = prptList[i].section('=', 0, 0);
			QStringList values = tail.split(',');
			for (j=0; j<values.size(); j++) {
				if (actions.contains(id)) {
					if (!parseAction(ref, client, id, values[j], j))
						missingPrpty = 1;
				}
				else if (!editable(ref)) {
					missingPrpty = 1;
					//qDebug() << "must lock object first";
				}
				else if (intProperties.contains(id))
					parseInt(ref, id, values[j].toInt());
				else if (stringProperties.contains(id))
					parseString(ref, id, values[j]);
				else if (floatProperties.contains(id)) {
					values[j].replace('#', '.');
					parseFloat(ref, id, values[j].toFloat());
				}
				else {
					missingPrpty = 1;
					//qDebug() << "missing" << id;
				}
			}
		}

		if (missingPrpty)
			found = 0;
		else
			found = 1;
	}

	return found;
}

bool Parse::parseObjects(QObject *, Client *, QString, QString, QString)
{
	return 0;
}

bool Parse::parseAction(QObject *, Client *, QString, QString, qint32)
{
	return 0;
}

void Parse::parseInt(QObject *, QString, qint32) {}
void Parse::parseFloat(QObject *, QString, qreal) {}
void Parse::parseString(QObject *, QString, QString) {}

bool Parse::editable(QObject *)
{
	return 1;
}

