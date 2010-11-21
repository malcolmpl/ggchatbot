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

#include <QPointer>
#include "sessionclientptr.h"
#include "profileptr.h"
#include "userinfoto.h"

class UserDatabase;
class BotSettings;
class BotSettingsTO;

typedef QPointer<UserDatabase> UserDatabasePtr;
typedef QPointer<BotSettings> BotSettingsPtr;

class Profile : public QObject
{
    Q_OBJECT
public:
    Profile();
    virtual ~Profile();

    BotSettingsTO getBotSettings() const;
    void setBotSettings(const BotSettingsTO);
    UserDatabasePtr getUserDatabase() const;
    SessionClientPtr getSession() const;
    void setSession(const SessionClientPtr v);
    bool messageIsSpam(UserInfoTOPtr, QString);
    QString replaceBadWords(QString content, bool &);
    void kickHelperCommand(UserInfoTOPtr u, UserInfoTOPtr sender, QString reason);

private:
    void Init();
    QStringList getSpamContent();
    QString replaceStar(QString content);
    QStringList getBadWordsList();

    BotSettingsPtr m_botSettings;
    UserDatabasePtr m_userDatabase;
    SessionClientPtr m_sessionClient;
};

#endif //PROFILE_H_
