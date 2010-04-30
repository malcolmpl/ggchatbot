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

#ifndef _BOTSETTINGSTO_H
#define	_BOTSETTINGSTO_H

#include <QString>
#include "libgadu.h"

class BotSettingsTO
{
public:
    BotSettingsTO();
    virtual ~BotSettingsTO();

    void setDebugLevel(const int v) { m_level = v; }
    int getDebugLevel() const { return m_level; }

    void setUin(const uin_t v) { m_uin = v; }
    uin_t getUin() const { return m_uin; }

    void setPassword(const QString v) { m_password = v; }
    QString getPassword() const { return m_password; }

    void setDefaultDescription(const QString v) { m_defaultDescription = v; }
    QString getDefaultDescription() const { return m_defaultDescription; }

    void setChannelModerated(const bool v) { m_channelModerated = v; }
    bool getChannelModerated() { return m_channelModerated; }

    void setChannelClosed(const bool v) { m_channelClosed = v; }
    bool getChannelClosed() { return m_channelClosed; }

    void setWhoDescription(const QString v) { m_whoDescription = v; }
    QString getWhoDescription() const { return m_whoDescription; }
    
private:
    int m_level;
    uin_t m_uin;
    QString m_password;
    QString m_defaultDescription;
    QString m_whoDescription;
    int m_channelModerated;
    bool m_channelClosed;
};

#endif	/* _BOTSETTINGSTO_H */

