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

#include "kickuserjob.h"
#include "userinfoto.h"
#include "userdatabase.h"
#include "sessionclient.h"

#include <QDebug>

// 10 minutes in seconds
const int INACTIVE_TIME = 60;

KickUserJob::KickUserJob()
{
    // one minute
    setTimerPeriod(1000*60);
}

void KickUserJob::makeJob()
{
    QList<UserInfoTOPtr> usersList = GetProfile()->getUserDatabase()->getUserList();
    QDateTime now = QDateTime::currentDateTime();

    foreach(UserInfoTOPtr user, usersList)
    {
        if(user->getOnChannel())
        {
            // it not working on special users
            if(user->getUserFlags() >= GGChatBot::OP_USER_FLAG)
                continue;

            qDebug() << "Checking time:" << user->getUin() << user->getLastSeen().secsTo(now) << INACTIVE_TIME;

            if(user->getLastSeen().secsTo(now) > INACTIVE_TIME)
            {
                // kick user
                QString msg = QString("%1 wylatuje z czatu. Nie spac, zwiedzac!").arg(GetProfile()->getUserDatabase()->makeUserNick(user));
                GetProfile()->getSession()->sendMessage(msg);
                qDebug() << msg;
                msg = QString("Zostales automatycznie wylogowany z powodu braku aktywnosci przez 10 minut. Aby powrocic wpisz: /start");
                GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
                user->setOnChannel(false);
            }
        }
    }
}
