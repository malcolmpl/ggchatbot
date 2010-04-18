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
#include <libgadu.h>

#include "profilebase.h"
#include "userinfoto.h"

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
    int m_channelFlags;
    QString m_topic;
    bool isChatClosed;

    QString removeCommand(QString message, QString command);

    void closedCommand();
    bool checkIfUserCanJoin();
    void nickCommand();
    void joinCommand();
    void leaveCommand();
    void whoCommand();
    void helpCommand();
    void kickCommand();
    void kickHelperCommand(UserInfoTOPtr);
    void banCommand();
    void banHelperCommand(UserInfoTOPtr, uint, QString);
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
};

#endif	/* _COMMANDRESOLVER_H */

