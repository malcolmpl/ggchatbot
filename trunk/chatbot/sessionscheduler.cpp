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

#include <QTimer>
#include <QTime>
#include <QDebug>

SessionScheduler::SessionScheduler(gg_session *s)
{
    session = s;

    time = new QTime();
    time->start();

    timer = new QTimer(this);
    timer->start(1000);
    
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
}

SessionScheduler::~SessionScheduler()
{
}

void SessionScheduler::run()
{
    exec();
}

bool SessionScheduler::checkTime(int sec)
{
    int seconds = time->elapsed() / 1000;

    if(!(seconds % sec))
        return true;

    return false;
}

void SessionScheduler::timerEvent()
{
    if(!session)
        return;
    
    pingServer();
}

void SessionScheduler::pingServer()
{
    if(checkTime(10))
    {
        qDebug() << "Ping server";
        gg_ping(session);
    }
}

