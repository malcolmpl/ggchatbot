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

#include <QtCore/QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QDate>
#include <QFile>

#include "connectionthread.h"
#include "logscheduler.h"

QTextStream *logOutput;

void logHandler(QtMsgType type, const char *msg)
{
    if(logOutput && logOutput->device())
    {
        *logOutput << "[" << QTime::currentTime().toString("hh:mm:ss:zzz") << "] " << msg << "\n";
        logOutput->flush();
    }

    switch (type)
    {
       case QtDebugMsg:
           fprintf(stderr, "Debug: %s\n", msg);
           break;
       case QtWarningMsg:
           fprintf(stderr, "Warning: %s\n", msg);
           break;
       case QtCriticalMsg:
           fprintf(stderr, "Critical: %s\n", msg);
           break;
       case QtFatalMsg:
           fprintf(stderr, "Fatal: %s\n", msg);
           abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(&logHandler);
    QCoreApplication app(argc, argv);

    LogScheduler logSched;
    logSched.start();

    ConnectionThread connection;
    QObject::connect(&connection, SIGNAL(finished()), &app, SLOT(quit()));
    connection.start();

    int ret = app.exec();
    fprintf(stderr, "exec end\n");

    fprintf(stderr, "exit\n");
    return ret;
}
