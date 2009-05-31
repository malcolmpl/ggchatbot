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

#ifndef _SESSIONCLIENT_H
#define	_SESSIONCLIENT_H

#include <QtCore>
#include <QtDebug>
#include <boost/shared_ptr.hpp>
#include <time.h>

#include "libgadu.h"
#include "eventmanager.h"
#include "profilebase.h"
#include "pingserverjob.h"
#include "kickuserjob.h"

typedef boost::shared_ptr<gg_session*> SessionPtr;
typedef boost::shared_ptr<gg_event*> EventPtr;

class SessionScheduler;

class SessionClient : public QObject, public ProfileBase
{
    Q_OBJECT
public:
    SessionClient(QObject *parent = 0);
    ~SessionClient();

public slots:
    void MakeConnection();
    void sendMessage(QString message);
    void sendMessage(uin_t uin, QString message);
    void sendMessageTo(uin_t uin, QString message);
    void sendMessageToSuperUser(uin_t uin, QString message);
    void ChangeStatus(QString description = "v0.3", int status = GG_STATUS_AVAIL_DESCR);
    
signals:
    void endServer();
    void restartConnection();
    
private:
    struct gg_login_params loginParams;
    struct gg_recv_msg gotmsg;
    struct gg_session *session;
    struct gg_event *event;
    EventManager eventManager;
    SessionScheduler *scheduler;
    fd_set rd, wd, ex;
    struct timeval tv;
    JobPtr pingServer;
    boost::shared_ptr<KickUserJob> kickUser;

    void FreeSession(gg_session *session);
    void Logout(gg_session *session);
    void CleanAndExit();
    void SetDebugLevel();
    bool Login();
    bool SendContactList();
    bool WaitForEvent();
    void EventLoop();
};

typedef boost::shared_ptr<SessionClient> SessionClientPtr;

#endif	/* _SESSIONCLIENT_H */

