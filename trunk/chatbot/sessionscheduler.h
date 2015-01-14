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

#include <QObject>
#include <QList>

#include "job.h"

class QTimer;
class QTime;

class SessionScheduler : public QObject
{
    Q_OBJECT
public:
    SessionScheduler(QObject *parent = 0);
    virtual ~SessionScheduler();

    void addJob(JobPtr j);
    void removeJob(JobPtr j);
    void clearJobs();

private slots:
    void timerEvent();
    
private:
    void pingServer();
    bool checkTime(int sec);

    QTimer *timer;
    QTime *time;
    QList<JobPtr> jobsList;
};

#endif	/* _SESSIONSCHEDULER_H */

