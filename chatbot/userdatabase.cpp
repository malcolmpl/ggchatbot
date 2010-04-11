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
    const int LAST_DAYS_SEEN = 30; // clear uin information after 30 days
}

UserDatabase::UserDatabase()
{
    settings = SettingsPtr(new QSettings(USERS_CONFIG, QSettings::IniFormat));
    readUsersListConfig();
}

UserDatabase::~UserDatabase()
{
//    cleanOnChannel();
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
    int size = settings->beginReadArray("users");

    QDateTime now = GGChatBot::getDateTime();

    for (int i = 0; i < size; ++i)
    {
        UserInfoTOPtr user = UserInfoTOPtr(new UserInfoTO());
        settings->setArrayIndex(i);
        user->setNick(settings->value("nick").toString());
        user->setUin(settings->value("UIN").toUInt());
	user->setUserFlags(settings->value("Flags").toUInt());
        user->setLastSeen(settings->value("lastSeen").toDateTime());
        user->setChannelName(settings->value("channel").toString());
	user->setOnChannel(settings->value("onChannel").toBool());
        user->setBanned(settings->value("banned").toBool());
        user->setBanTime(settings->value("banTime").toDateTime());
        user->setBanReason(settings->value("banReason").toString());
	if(user->getLastSeen().daysTo(now) >= LAST_DAYS_SEEN)
	{
	    qDebug() << "Usuwam z bazy informacje o" << user->getUin() << user->getNick();
	    continue;
	}

        m_usersList.push_back(user);
    }
    settings->endArray();
}

void UserDatabase::saveUsersListConfig()
{
    qDebug() << "Zapisywanie bazy uzytkownikow...";
    settings->beginWriteArray("users");

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
        settings->setValue("banReason", m_usersList.at(i)->getBanReason());
    }
    settings->endArray();
}

UserInfoTOPtr UserDatabase::getUserInfo(uin_t uin)
{
    foreach(UserInfoTOPtr user, m_usersList)
    {
        if(user->getUin() == uin)
            return user;
    }

    // This user not exist in database yet. We should add it.
    UserInfoTOPtr user = UserInfoTOPtr(new UserInfoTO());
    user->setUin(uin);
    addUser(user);

    return user;
}

void UserDatabase::addUser(const UserInfoTOPtr u)
{
    qDebug() << "Dodaje użytkownka" << u->getUin() << u->getNick() << "do bazy.";
    
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

bool UserDatabase::isUserOnChannel(UserInfoTOPtr user)
{
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

        return true;
    }

    return true;
}

bool UserDatabase::isUserHaveNick(UserInfoTOPtr user)
{
    if(user->getNick().isEmpty())
        return false;

    return true;
}

bool UserDatabase::isUserHaveVoice(UserInfoTOPtr user)
{
    if(user->getUserFlags() == GGChatBot::VOICE_USER_FLAG)
        return true;

    return false;
}

bool UserDatabase::isUserHaveOp(UserInfoTOPtr user)
{
    if(user->getUserFlags() == GGChatBot::OP_USER_FLAG)
        return true;

    return false;
}

bool UserDatabase::isSuperUser(UserInfoTOPtr user)
{
    if(user->getUserFlags() > GGChatBot::SUPER_USER_FLAG)
        return true;

    return false;
}

GGChatBot::UserNick UserDatabase::makeUserNick(UserInfoTOPtr u)
{
    GGChatBot::UserNick userNick;
    unsigned char *result;
    gg_msg_richtext header;
    gg_msg_richtext_format format;
    gg_msg_richtext_color color, color_black;
    unsigned int memoryPosition = sizeof(gg_msg_richtext);
    int resultlen = sizeof(gg_msg_richtext) + 2*(sizeof(format) + sizeof(color));
    result = new unsigned char[resultlen];
    format.font = 0;

    header.flag = 2;
    header.length = gg_fix16(resultlen - sizeof(gg_msg_richtext));
    memcpy(result, &header, sizeof(header));

    color_black.red = 0;
    color_black.green = 0;
    color_black.blue = 0;

    if(isUserHaveVoice(u))
    {
	color.red = 0;
	color.green = 150;
	color.blue = 0;
	format.position = gg_fix16(2);
	format.font |= GG_FONT_COLOR;
	memcpy(result + memoryPosition, &format, sizeof(format));
	memoryPosition += sizeof(format);
	memcpy(result + memoryPosition, &color, sizeof(color));
	memoryPosition += sizeof(color);
	userNick.nick = "<+" + u->getNick() + ">";
    }
    else if(isUserHaveOp(u))
    {
	color.red = 51;
	color.green = 0;
	color.blue = 204;
	format.position = gg_fix16(2);
	format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);
        userNick.nick = "<@" + u->getNick() + ">";
    }
    else if(isSuperUser(u))
    {
        color.red = 255;
        color.green = 0;
        color.blue = 0;
	format.position = gg_fix16(2);
	format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);
        userNick.nick = "<!" + u->getNick() + ">";
    }
    else
    {
        color.red = 100;
        color.green = 0;
        color.blue = 0;
	format.position = gg_fix16(1);
	format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);
	userNick.nick = "<" + u->getNick() + ">";	
    }

    format.font = 0;
    format.position = gg_fix16(format.position + u->getNick().length());
    memcpy(result + memoryPosition, &format, sizeof(format));
    memoryPosition += sizeof(format);
    memcpy(result + memoryPosition, &color_black, sizeof(color_black));

    userNick.format = result;
    userNick.formatlen = resultlen;

    return userNick;
}

