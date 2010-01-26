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

#include <QPointer>

#include <QString>
#include <QDateTime>

class UserInfoTO : public QObject
{
    Q_OBJECT
public:
    UserInfoTO();
    virtual ~UserInfoTO();

    void setUin(const uin_t v) { m_uin = v; }
    uin_t getUin() const { return m_uin; }

    void setNick(const QString v) { m_nick = v; }
    QString getNick() const { return m_nick; }

    void setUserFlags(const int v) { m_flags = v; }
    int getUserFlags() const { return m_flags; }

    void setOnChannel(bool v) { m_onChannel = v; }
    bool getOnChannel() const { return m_onChannel; }

    void setChannelName(QString v) { m_channelName = v; }
    QString getChannelName() const { return m_channelName; }

    void setLastSeen(QDateTime v) { m_lastSeen = v; }
    QDateTime getLastSeen() const { return m_lastSeen; }

    void setBanned(bool v) { m_banned = v; }
    bool getBanned() const { return m_banned; }
    bool isBanned() const { return m_banned; }

    void setBanTime(QDateTime v) { m_banTime = v; }
    QDateTime getBanTime() const { return m_banTime; }

    void setBanReason(QString v) { m_banReason = v; }
    QString getBanReason() const { return m_banReason; }

    void setLastMessage(QString v) { m_lastMessage = v; }

    QString getLastMessage() const { return m_lastMessage; }
private:
    QString m_channelName;
    bool m_onChannel;
    int m_flags;
    QDateTime m_lastSeen;
    uin_t m_uin;
    bool m_banned;

    QString m_nick;
    QDateTime m_banTime;
    QString m_banReason;
    QString m_lastMessage;
};

typedef QPointer<UserInfoTO> UserInfoTOPtr;

#endif	/* _USERINFOTO_H */

