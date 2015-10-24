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
   File name    : emailnotify.h
   Created      : 21 Oct 2015 - Back to the Future!
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Interface to libesmtp to send email notifications
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef EMAILNOTIFY_H_
#define EMAILNOTIFY_H_

#include <QObject>

class Withdrawal;

class EmailNotify : public QObject
{
	Q_OBJECT

public:
	EmailNotify();

public slots:
	void notify(Withdrawal *withdrawal);

private:
	bool configOk;
	QString server, reply;

};

#endif /* EMAILNOTIFY_H_ */
