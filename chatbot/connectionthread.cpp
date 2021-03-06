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

#include "connectionthread.h"
#include "profile.h"

#include <QtDebug>

ConnectionThread::ConnectionThread(QObject *parent)
    : QThread(parent)
{
    moveToThread(this);
}

ConnectionThread::~ConnectionThread()
{
}

void ConnectionThread::run()
{
    qDebug() << "run() called";
    exec();
}

void ConnectionThread::startServer()
{
    qDebug() << "startServer() called";
    ProfilePtr profile = new Profile();
    sessionClient = new SessionClient();
    profile->setSession(sessionClient);
    sessionClient->SetProfile(profile);

    QObject::connect(sessionClient, SIGNAL(endServer()), this, SLOT(quit()));

    sessionClient->MakeConnection();
}

