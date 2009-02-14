/*
    This file is part of GGChatBot.

    Copyright (C) 2009  Dariusz Mikulski <dariusz.mikulski@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#ifndef _SESSIONSCHEDULER_H
#define	_SESSIONSCHEDULER_H

#include <libgadu.h>

#include <QThread>

class QTimer;
class QTime;

class SessionScheduler : public QThread
{
    Q_OBJECT
public:
    SessionScheduler(gg_session *s);
    virtual ~SessionScheduler();

    void run();

private slots:
    void timerEvent();
    
private:
    void pingServer();
    bool checkTime(int sec);

    gg_session *session;
    QTimer *timer;
    QTime *time;
};

#endif	/* _SESSIONSCHEDULER_H */

