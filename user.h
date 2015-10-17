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
   File name    : user.h
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   This is the user class which holds withdrawals, deposits and clients as well as general info
   about the user.  An instance of this class exists for each user.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef USER_H_
#define USER_H_

#include <QObject>
#include <QList>
#include "deposit.h"
#include "withdrawal.h"
#include "parse.h"
#include "client.h"
#include "region.h"
#include "server.h"
#include "userparser.h"

class Client;
class GeoRegion;

enum user_type {
	UT_NORMAL,
	UT_MANAGER,
	UT_ADMIN,
	UT_DISABLED,
};

class User : public QObject
{
	Q_OBJECT

friend UserParser;

public:
	User(Server *srv, QString fName);
	void load(void);
	void save(void);
	bool authLookUp(User *user);
	bool lock(quint32 clientVersion, Client *client);
	bool unlock(Client *client);
	void addAnnualAllotment(qint32 year);
	void checkForExpiredRequests(quint32 today);
	void forceManagementChainUpdate(void);

	inline QString getID(void) {return id;}
	inline void setID(QString value) {id = value;}
	inline QString getPwdHash(void) {return pwdHash;}
	inline void setPwdHash(QString value) {pwdHash = value;}
	inline QString getReportsTo(void) {return reportsTo;}
	inline void setBoss(User *newBoss)
	{
		boss = newBoss;
		boss->employees.append(this);
	}
	inline GeoRegion *getRegion(void) {return region;}
	inline user_type getType(void) {return type;}
	inline void setType(user_type value) {type = value;}
	inline void removeClient(Client *client) {clients.removeAt(clients.indexOf(client));}
	inline void addClient(Client *client) {clients.append(client);}
	inline Client *getClientAt(quint32 i) {return clients[i];}
	inline qint32 getClientTotal(void) {return clients.size();}

	static qint32 getXmlInt(QXmlStreamReader *xml)
	{
		qint32 x = 0;

		xml->readNext();
		if (xml->tokenType() == QXmlStreamReader::Characters)
#if QT_VERSION >= 0x050000
			x = xml->text().toInt();
#else
		{
			QString temp;
			temp.append(xml->text());
			x = temp.toInt();
		}
#endif



		return x;
	}
	static float getXmlFloat(QXmlStreamReader *xml)
	{
		float x = 0;

		xml->readNext();
		if (xml->tokenType() == QXmlStreamReader::Characters)
#if QT_VERSION >= 0x050000
			x = xml->text().toFloat();
#else
		{
			QString temp;
			temp.append(xml->text());
			x = temp.toFloat();
		}
#endif

		return x;
	}
	static QString getXmlString(QXmlStreamReader *xml)
	{
		QString text;

		xml->readNext();
		if (xml->tokenType() == QXmlStreamReader::Characters)
			text = xml->text().toString();

		return text;
	}

signals:
	void replicate(void);

private:
	void dump(Client *client);
	void dumpEmployees(Client *client);
	void buildEmployeeListResponce(QString *response);
	void buildEmployeeList(QString *response, qint32 level);
	void dumpPossibleManagers(Client *client);
	void cullPossibleManagers(QList <User *> *list);
	QString id, firstName, lastName, reportsTo, fileName, pwdHash, email;
	QList <Deposit *> deposits;
	QList <Withdrawal *> withdrawals;
	QList <User *> employees;		// for managers only
	QList <Client *> clients;
	Client *lockedBy;
	Server *server;
	User *oldBoss, *boss;
	GeoRegion *region;
	quint32 version, annualAllotment;
	user_type type;
	bool updateManagersFlag;

private slots:
	void updateManagerChain(void);
};

#endif /* USER_H_ */
