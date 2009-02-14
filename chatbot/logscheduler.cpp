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

#include "logscheduler.h"

#include <QTimer>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QDebug>

extern QTextStream *logOutput;

LogScheduler::LogScheduler()
{
}

void LogScheduler::run()
{
    QDate date;
    date = QDate::currentDate();
    QString fileName = QString("logs/log_%1.txt").arg(date.toString("ddMMyyyy"));
    logFile.setFileName(fileName);
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        qDebug("Can't open log file!");
        return;
    }

    logOutput = new QTextStream(&logFile);

    lastTime = QTime::currentTime();
    logTimer = new QTimer();
    QObject::connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLogFile()));
    logTimer->start(1000);
    exec();
}

LogScheduler::~LogScheduler()
{
    logFile.close();
}

void LogScheduler::updateLogFile()
{
    if(QTime::currentTime() < lastTime)
    {
        lastTime = QTime::currentTime();
        if(!logOutput->device())
            return;

        delete logOutput;
        logOutput = 0;
        
        logFile.close();
        QDate date;
        date = QDate::currentDate();
        QString fileName = QString("logs/log_%1.txt").arg(date.toString("ddMMyyyy"));
        logFile.setFileName(fileName);
        if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            qDebug() << "Can't open log file!";
            return;
        }
        logOutput = new QTextStream(&logFile);
    }
}

