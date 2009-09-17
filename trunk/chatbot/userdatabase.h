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

#ifndef _USERDATABASE_H
#define	_USERDATABASE_H

#include "userinfoto.h"

#include <QSharedPointer>

#include <QList>
#include <QSettings>

typedef QSharedPointer<QSettings> SettingsPtr;

class UserDatabase
{
public:
    UserDatabase();
    virtual ~UserDatabase();

    UserInfoTOPtr getUserInfo(uin_t uin);
    void addUser(const UserInfoTOPtr user);
    void saveDatabase();
    QList<UserInfoTOPtr> getUserList() { return m_usersList; }

    bool isUserOnChannel(UserInfoTOPtr user);
    bool isUserInDatabase(uin_t user);
    bool isUserHaveNick(UserInfoTOPtr user);
    bool isUserHaveVoice(UserInfoTOPtr user);
    bool isUserHaveOp(UserInfoTOPtr user);
    bool isSuperUser(UserInfoTOPtr user);
   
    QString makeUserNick(UserInfoTOPtr);
 
private:
    void readUsersListConfig();
    void saveUsersListConfig();
    void cleanOnChannel();
    
    QList<UserInfoTOPtr> m_usersList;
    SettingsPtr settings;
};

#endif	/* _USERDATABASE_H */

