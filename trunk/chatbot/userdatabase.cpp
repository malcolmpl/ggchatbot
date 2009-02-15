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

#include "userdatabase.h"
#include "common.h"

#include <QDebug>

namespace
{
    const QString USERS_CONFIG = "users.ini";
}

UserDatabase::UserDatabase()
{
    settings = SettingsPtr(new QSettings(USERS_CONFIG, QSettings::IniFormat));
    readUsersListConfig();
}

UserDatabase::~UserDatabase()
{
    cleanOnChannel();
    saveUsersListConfig();
}

void UserDatabase::cleanOnChannel()
{
    foreach(UserInfoTOPtr user, m_usersList)
    {
        user->setOnChannel(false);
    }
}

void UserDatabase::readUsersListConfig()
{
    qDebug() << "Odczytywanie bazy uzytkownikow...";
    int size = settings->beginReadArray("user");
    for (int i = 0; i < size; ++i)
    {
        UserInfoTOPtr user = UserInfoTOPtr(new UserInfoTO());
        settings->setArrayIndex(i);
        user->setNick(settings->value("nick").toString());
        user->setUin(settings->value("UIN").toUInt());
        user->setUserFlags(static_cast<GGChatBot::USER_FLAGS>(settings->value("Flags").toInt()));
        user->setLastSeen(settings->value("lastSeen").toDateTime());
        user->setChannelName(settings->value("channel").toString());
        user->setOnChannel(settings->value("onChannel").toBool());
        user->setBanned(settings->value("banned").toBool());
        user->setBanTime(settings->value("banTime").toDateTime());
        m_usersList.append(user);
    }
    settings->endArray();
}

void UserDatabase::saveUsersListConfig()
{
    qDebug() << "Zapisywanie bazy uzytkownikow...";
    settings->beginWriteArray("logins");
    for (int i = 0; i < m_usersList.size(); ++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("nick", m_usersList.at(i)->getNick());
        settings->setValue("UIN", m_usersList.at(i)->getUin());
        settings->setValue("Flags", m_usersList.at(i)->getUserFlags());
        settings->setValue("lastSeen", m_usersList.at(i)->getLastSeen());
        settings->setValue("channel", m_usersList.at(i)->getChannelName());
        settings->setValue("onChannel", m_usersList.at(i)->getOnChannel());
        settings->setValue("banned", m_usersList.at(i)->getBanned());
        settings->setValue("banTime", m_usersList.at(i)->getBanTime());
    }
    settings->endArray();
}

UserInfoTOPtr UserDatabase::getUserInfo(uin_t uin) const
{
    foreach(UserInfoTOPtr user, m_usersList)
    {
        if(user->getUin() == uin)
            return user;
    }

    return UserInfoTOPtr();
}

void UserDatabase::addUser(const UserInfoTOPtr u)
{
    qDebug() << "add user";
    if(u->getUin() == 0)
        return;
    
    foreach(UserInfoTOPtr user, m_usersList)
    {
        if(user->getUin()== u->getUin())
            return;
    }

    m_usersList.append(u);

    saveDatabase();
}

void UserDatabase::saveDatabase()
{
    saveUsersListConfig();
}

bool UserDatabase::isUserOnChannel(uin_t uin)
{
    if(!isUserHaveNick(uin))
        return false;

    UserInfoTOPtr user = getUserInfo(uin);
    if(!user->getOnChannel())
        return false;

    return true;
}

bool UserDatabase::isUserInDatabase(uin_t uin)
{
    if(getUserInfo(uin) == NULL)
    {
        UserInfoTOPtr user = UserInfoTOPtr(new UserInfoTO());
        user->setUin(uin);
        addUser(user);

        return false;
    }

    return true;
}

bool UserDatabase::isUserHaveNick(uin_t uin)
{
    if(!isUserInDatabase(uin))
        return false;

    UserInfoTOPtr user = getUserInfo(uin);
    if(user->getNick().isEmpty())
        return false;

    return true;
}


