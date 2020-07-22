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
#include <QRegExp>
#include <QString>
#include <QLocale>
#include <QtGlobal>

#include "connectionthread.h"
#include "logscheduler.h"
#include "common.h"

QTextStream *logOutput;

void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QString strTime = QString("[%1]").arg(GGChatBot::getDateTime().toString("dd-MM-yyyy hh:mm:ss:zzz"));
    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "%s %s\n", strTime.toLatin1().data(), msg.toUtf8().data());
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s %s\n", strTime.toLatin1().data(), msg.toUtf8().data());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s %s\n", strTime.toLatin1().data(), msg.toUtf8().data());
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s %s\n", strTime.toLatin1().data(), msg.toUtf8().data());
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s %s\n", strTime.toLatin1().data(), msg.toUtf8().data());
            break;
    }

    if(logOutput)
    {
        if(logOutput->device())
        {
            *logOutput << QString("%1 %2\n").arg(strTime).arg(msg.toUtf8().data());
            logOutput->flush();
        }
    }

    if(type == QtFatalMsg)
    {
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(&logHandler);
    QCoreApplication app(argc, argv);
    QTextCodec::setCodecForLocale (QTextCodec::codecForName ("UTF-8"));
    QLocale::setDefault(QLocale::Polish);

    LogScheduler *logSched = new LogScheduler();
    logSched->start();

    ConnectionThreadPtr connection = ConnectionThread::Create();
    QObject::connect(connection.data(), SIGNAL(finished()), &app, SLOT(quit()));
    connection->start();
    connection->startServer();

    int ret = app.exec();

    return ret;
}
