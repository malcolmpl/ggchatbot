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

#ifndef _EVENTMANAGER_H
#define	_EVENTMANAGER_H

#include "libgadu.h"
#include <QObject>

#include "profilebase.h"
#include "userinfoto.h"
#include "commandresolver.h"

class EventManager : public QObject, public ProfileBase
{
    Q_OBJECT
public:
    EventManager();
    virtual ~EventManager();

    void ResolveEvent(gg_event *event);

signals:
    void sendMessage(QString message);
    void sendMessage(uin_t uin, QString message);
    void sendMessageTo(uin_t uin, QString message);

private:
    gg_event *m_event;
    CommandResolver cmdResolv;
    
    void AckEvent();
    void MessageEvent();
    void welcomeMessage();
    bool checkCommand();
};

#endif	/* _EVENTMANAGER_H */

