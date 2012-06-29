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

#ifndef _COMMANDRESOLVER_H
#define	_COMMANDRESOLVER_H

#include <QObject>
#include <QPointer>
#include <libgadu.h>

#include "profilebase.h"
#include "userinfoto.h"

class Stats : public QObject
{
public:
    Stats() : adminUsers(0), opUsers(0), voiceUsers(0), totalUsers(0), ktosUsers(0), banUsers(0), totalBansTime(0), last24h(0), lastWeek(0), lastMonth(0) {}

    void init()
    {
        adminUsers = opUsers = voiceUsers = totalUsers = ktosUsers = banUsers = totalBansTime = last24h = lastWeek = lastMonth = 0;
    }

    int adminUsers;
    int opUsers;
    int voiceUsers;
    int totalUsers;
    int ktosUsers;
    int banUsers;
    uint64_t totalBansTime;
    int last24h;
    int lastWeek;
    int lastMonth;
};

class CommandResolver : public QObject, public ProfileBase
{
    Q_OBJECT
public:
    CommandResolver();
    virtual ~CommandResolver();

    bool checkCommand(gg_event *event);

private:
    gg_event *m_event;
    QString lastString;
    bool mChannelModerated;
    QString m_topic;
    bool mChannelClosed;
    bool m_topicIsBlocked;
    QDateTime mLastUserJoin;
    QPointer<Stats> mStatsPtr;

    QString removeCommand(QString message, QString command);

    void closedCommand();
    bool checkIfUserCanJoin();
    void nickCommand();
    void joinCommand();
    void leaveCommand();
    void whoCommand();
    void helpCommand();
    void kickCommand();
    void kickHelperCommand(UserInfoTOPtr, UserInfoTOPtr);
    void banCommand();
    void banHelperCommand(UserInfoTOPtr, uint, QString, UserInfoTOPtr);
    void unbanCommand();
    void unbanHelperCommand(UserInfoTOPtr);
    void topicCommand();
    void opCommand();
    void voiceCommand();
    void removeFlagsCommand();
    void moderateCommand();
    void unmoderateCommand();
    void setTopic(QString, bool showMessage = true);
    void imgStatusList();
    void setImgStatus();
    void privCommand();
    void kickAllCommand();
    void totalKickCommand();
    void statsCommand();
    void setNickCommand();
    void seenCommand();
    void blockTopicCommand();
};

#endif	/* _COMMANDRESOLVER_H */

