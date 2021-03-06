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
#include "profile.h"
#include "common.h"

#include <QDebug>

// inactive time for users - 10 minutes in seconds
const int INACTIVE_TIME = 60*20;
// 1 minut in seconds
const int ONE_MINUTE_IN_SECONDS = 60;
// seconds for users without nick
const int INACTIVE_NONICK_TIME = 60;
// one hour
const int INACTIVE_VOICE_TIME = ONE_MINUTE_IN_SECONDS * 60;

KickUserJob::KickUserJob()
{
    // one minute
    setTimerPeriod(60);
}

KickUserJob::~KickUserJob()
{
}

void KickUserJob::makeJob()
{
    QList<UserInfoTOPtr> usersList = GetProfile()->getUserDatabase()->getUserList();
    QDateTime now = GGChatBot::getDateTime();

    int inactiveTime;

    foreach(UserInfoTOPtr user, usersList)
    {
        if(user->getOnChannel())
        {
            inactiveTime = INACTIVE_TIME;

            // it not working on special users
            if(user->getUserFlags() >= GGChatBot::OP_USER_FLAG)
                continue;

            if(user->getUserFlags() == GGChatBot::VOICE_USER_FLAG)
                inactiveTime = INACTIVE_VOICE_TIME;

            if(user->getNick().startsWith("Ktos", Qt::CaseInsensitive))
            {
                inactiveTime = INACTIVE_NONICK_TIME;
            }

            if(user->getLastSeen().secsTo(now) > inactiveTime)
            {
                // kick user
                GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
                QString msg = QString("%1 wylatuje z czatu. Nie spac, zwiedzac!").arg(userNick.nick);
                GetProfile()->getSession()->sendMessage(msg);
                qDebug() << msg;
                msg = QString("Zostales automatycznie wylogowany z powodu braku aktywnosci przez %1 minut. Aby powrocic wpisz: /start").arg(inactiveTime/ONE_MINUTE_IN_SECONDS);
                GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
                user->setOnChannel(false);
            }
        }
    }
}

