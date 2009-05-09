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

#ifndef _USERINFOTO_H
#define	_USERINFOTO_H

#include "libgadu.h"
#include "common.h"

#include <boost/shared_ptr.hpp>

#include <QString>
#include <QDateTime>

class UserInfoTO
{
public:
    UserInfoTO();
    virtual ~UserInfoTO();

    void setUin(const uin_t v) { m_uin = v; }
    uin_t getUin() const { return m_uin; }

    void setNick(const QString v) { m_nick = v; }
    QString getNick() const { return m_nick; }

    void setUserFlags(const GGChatBot::USER_FLAGS v) { m_flags = v; }
    GGChatBot::USER_FLAGS getUserFlags() const { return m_flags; }

    void setOnChannel(bool v) { m_onChannel = v; }
    bool getOnChannel() const { return m_onChannel; }

    void setChannelName(QString v) { m_channelName = v; }
    QString getChannelName() const { return m_channelName; }

    void setLastSeen(QDateTime v) { m_lastSeen = v; }
    QDateTime getLastSeen() const { return m_lastSeen; }

    void setBanned(bool v) { m_banned = v; }
    bool getBanned() const { return m_banned; }

    void setBanTime(QDateTime v) { m_banTime = v; }
    QDateTime getBanTime() const { return m_banTime; }

    void setBanReason(QString v) { m_banReason = v; }
    QString getBanReason() const { return m_banReason; }

private:
    uin_t m_uin;
    QString m_nick;
    GGChatBot::USER_FLAGS m_flags;
    bool m_onChannel;
    QString m_channelName;
    QDateTime m_lastSeen;
    bool m_banned;
    QDateTime m_banTime;
    QString m_banReason;
};

typedef boost::shared_ptr<UserInfoTO> UserInfoTOPtr;

#endif	/* _USERINFOTO_H */

