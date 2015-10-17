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
   File name    : parse.h
   Created      : 29 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   This is an abstract class.  Properties like strings, ints and actions in subclasses can be
   registered.  When a registered property is detected, its value is updated in the subclass.  If
   the property is an action, the corresponding code is executed in the subclass's parseAction()
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef PARSER_H_
#define PARSER_H_

#include <QStringList>
#include <QString>
#include <QObject>

class Client;

class Parse : public QObject
{
	Q_OBJECT

public:
	bool parseCmd(QObject *ref, Client *client, QString cmd);

private:

private slots:
	
protected:
	virtual bool parseObjects(QObject *ref, Client *client, QString type, QString id, QString tail);
	virtual bool parseAction(QObject *ref, Client *client, QString cmd, QString value, qint32 n);
	virtual void parseInt(QObject *ref, QString id, qint32 value);
	virtual void parseFloat(QObject *ref, QString id, qreal value);
	virtual void parseString(QObject *ref, QString id, QString value);
	bool editable(QObject *);
	QStringList intProperties, floatProperties, stringProperties, actions;
};

#endif /* PARSER_H_ */
