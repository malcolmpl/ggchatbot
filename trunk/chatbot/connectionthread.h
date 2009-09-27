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

#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <QThread>
#include <QCoreApplication>
#include <boost/shared_ptr.hpp>
#include "sessionclient.h"

class ConnectionThread;

typedef boost::shared_ptr<ConnectionThread> ConnectionThreadPtr;

class ConnectionThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(ConnectionThread)

    template <typename T>
    class deleter
    {
        public:
        inline void operator()(T *p)
        {
            p->quit();
            while(p->isRunning())
            {
                QCoreApplication::processEvents();
                QCoreApplication::instance()->thread()->msleep(100);
            }
            delete p;
        }
    };
public:
    ConnectionThread(QObject *parent = 0);
    ~ConnectionThread();

    void run();

    static ConnectionThreadPtr Create()
    {
        return ConnectionThreadPtr(new ConnectionThread, ConnectionThread::deleter<ConnectionThread>());
    }
    
public slots:
    void startServer();

private:
    SessionClientPtr sessionClient;
};

#endif // CONNECTIONTHREAD_H
