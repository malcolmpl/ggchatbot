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

#include "profile.h"
#include "userdatabase.h"
#include "botsettings.h"
#include "sessionclient.h"
#include "debug.h"

#include <QStringList>

Profile::Profile()
{
    Init();
}

Profile::~Profile()
{

}

void Profile::Init()
{
    qDebug() << "Profile Init() called";
    m_botSettings = BotSettingsPtr(new BotSettings());
    m_userDatabase = UserDatabasePtr(new UserDatabase());
}

BotSettingsTO Profile::getBotSettings() const
{
    return m_botSettings->getBotSettings();
}

void Profile::setBotSettings(const BotSettingsTO bs)
{
    m_botSettings->setBotSettings(bs);
    m_botSettings->SaveBotSettings();
}

UserDatabasePtr Profile::getUserDatabase() const
{
    return m_userDatabase;
}

SessionClientPtr Profile::getSession() const
{
    return m_sessionClient;
}

void Profile::setSession(const SessionClientPtr v)
{
    m_sessionClient = v;
}

QStringList Profile::getSpamContent()
{
    QStringList spamList;
    spamList << "hydro" << "pijanyindyk" << "cotygadasz" << "5024276" << "grizlichat" << "21360"
        << "1915166" << "5433825" << "9488550";
    return spamList;
}

bool Profile::messageIsSpam(UserInfoTOPtr user, QString content)
{
    QStringList spamList = getSpamContent();
    QStringList contentList = content.split(" ", QString::SkipEmptyParts);
    foreach(QString str, contentList)
    {   
        foreach(QString spam, spamList)
        {   
            if(str.contains(spam, Qt::CaseInsensitive))
            {   
                QString msg = "SPAM!: " + content;
                showUserDebug(user, msg);
                return true;
            }
        }
    }

    return false;
}
