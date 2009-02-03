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

#ifndef PROFILE_H_
#define PROFILE_H_

#include <boost/shared_ptr.hpp>

class UserDatabase;
class BotSettings;
class BotSettingsTO;

typedef boost::shared_ptr<UserDatabase> UserDatabasePtr;
typedef boost::shared_ptr<BotSettings> BotSettingsPtr;

class Profile
{
public:
    Profile();
    virtual ~Profile();

    BotSettingsTO getBotSettings() const;
    UserDatabasePtr getUserDatabase() const;
    
private:
    void Init();

    BotSettingsPtr m_botSettings;
    UserDatabasePtr m_userDatabase;
};

typedef boost::shared_ptr<Profile> ProfilePtr;

#endif //PROFILE_H_
