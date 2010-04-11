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

#include "sessionscheduler.h"
#include "common.h"

#include <QTimer>
#include <QTime>
#include <QDebug>

SessionScheduler::SessionScheduler()
{
    time = new QTime();
    time->start();

    timer = new QTimer(this);
    timer->start(1000);
    
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
}

SessionScheduler::~SessionScheduler()
{
    if(time)
        delete time;

    if(timer)
    {
        timer->stop();
        delete timer;
    }

    jobsList.clear();
}

void SessionScheduler::run()
{
    exec();
}

bool SessionScheduler::checkTime(int sec)
{
    int seconds = time->secsTo(GGChatBot::getDateTime().time());
    if(!(seconds % sec))
        return true;

    return false;
}

void SessionScheduler::timerEvent()
{
    foreach(JobPtr job, jobsList)
    {
        if(checkTime(job->timerPeriod()))
            job->makeJob();
    }
}

void SessionScheduler::addJob(JobPtr j)
{
    foreach(JobPtr job, jobsList)
    {
        if(job == j)
            return;
    }

    jobsList.push_back(j);
}

void SessionScheduler::removeJob(JobPtr j)
{
    for(QList<JobPtr>::iterator i = jobsList.begin(); i != jobsList.end(); i++)
    {
        if(j == (*i))
        {
            jobsList.erase(i);
            return;
        }
    }
}
