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

#include "sessionclient.h"
#include "botsettingsto.h"
#include "sessionscheduler.h"
#include "userdatabase.h"
#include "profile.h"

#include <errno.h>
#include <QCoreApplication>

SessionClient::SessionClient(QObject *parent)
    : QObject(parent)
{
}

SessionClient::~SessionClient()
{
}

void SessionClient::MakeConnection()
{
	QObject::disconnect(&eventManager, 0, 0, 0);
    qRegisterMetaType<uin_t>("uin_t");
    eventManager.SetProfile(GetProfile());
    QObject::connect(&eventManager, SIGNAL(sendMessage(QString)), this, SLOT(sendMessage(QString)));
    QObject::connect(&eventManager, SIGNAL(sendMessage(uin_t, QString)), this, SLOT(sendMessage(uin_t, QString)));
    QObject::connect(&eventManager, SIGNAL(sendMessageTo(uin_t, QString)), this, SLOT(sendMessageTo(uin_t, QString)));

    SetDebugLevel();

    if(!Login())
    {
        return;
    }

    EventLoop();
}

void SessionClient::FreeSession(gg_session *session)
{
    if(session)
    {
        gg_free_event(event);
        gg_free_session(session);
    }
}

void SessionClient::Logout(gg_session *session)
{
    qDebug() << "Bot logout.";
    gg_logoff(session);
}

void SessionClient::CleanAndExit()
{
    Logout(session);
    FreeSession(session);

    qDebug() << "Bye, bye :)";
    //emit endServer();
}

void SessionClient::SetDebugLevel()
{
    qDebug() << "SetDebugLevel() called";

    gg_debug_level = GetProfile()->getBotSettings().getDebugLevel();
}

bool SessionClient::Login()
{
    qDebug() << "Logowanie...";
    memset(&loginParams, 0, sizeof(loginParams));

    uin_t uin = GetProfile()->getBotSettings().getUin();
    qDebug() << "UIN:" << uin;

    if(uin == 0)
    {
        qDebug() << "Skonfiguruj swoj UIN!";
        endServer();
        return false;
    }

    loginParams.uin = uin;
    loginParams.password = GetProfile()->getBotSettings().getPassword().toAscii().data();
    loginParams.async = 1;
    loginParams.encoding = GG_ENCODING_UTF8;

    if (!( session = gg_login(&loginParams) ) )
    {
        qDebug() << "Nie udalo sie polaczyc.";   //printf("Nie uda?o si? po??czy?: %s\n", strerror(errno));
        emit endServer();
        return false;
    }

    qDebug() << "Laczenie...";

    scheduler = new SessionScheduler();
    pingServer = JobPtr(new PingServerJob(session));
    KickUserJob *k = new KickUserJob();
    k->SetProfile(GetProfile());
    kickUser = JobPtr(k);
    scheduler->addJob(pingServer);
    scheduler->addJob(kickUser);
    scheduler->start();
    
    return true;
}

bool SessionClient::SendContactList()
{
    if(gg_notify(session, NULL, 0) == -1)
    {
        qDebug() << "Blad wysylania listy kontaktow na serwer";
        return false;
    }

    qDebug() << "Lista kontaktow wyslana na serwer.";
    return true;
}

void SessionClient::ChangeStatus(QString description, int status)
{
    qDebug() << "Zmieniam status na: " << description;
    QByteArray data = GGChatBot::unicode2cp(GGChatBot::makeMessage(description));
    gg_change_status_descr(session, status, data.data());
}

bool SessionClient::WaitForEvent()
{
    if( !(event = gg_watch_fd(session)) )
        return false;

    return true;
}

void SessionClient::EventLoop()
{
    forever
    {
        FD_ZERO(&rd);
        FD_ZERO(&wd);

        if ((session->check & GG_CHECK_READ))
            FD_SET(session->fd, &rd);

        if ((session->check & GG_CHECK_WRITE))
            FD_SET(session->fd, &wd);

        if (session->timeout)
        {
            tv.tv_sec = 0;//session->timeout;
            tv.tv_usec = 10;
        }

        int wynik = select(session->fd + 1, &rd, &wd, NULL, /*(session->timeout) ? &tv : NULL*/ &tv);

        if (!wynik)
        {
		QCoreApplication::instance()->processEvents();
            continue;
        }

        if (wynik == -1)
        {
            if (errno != EINTR)
            {
                qDebug() << "Blad funkcji select(): " << strerror(errno);
                emit endServer();
                return;
            }
        }

        if (FD_ISSET(session->fd, &rd) || FD_ISSET(session->fd, &wd))
        {
            if(!WaitForEvent())
            {
                qDebug() << "Rozlaczono z serwerem, ponawiam...";
                QTimer::singleShot(1, this, SLOT(MakeConnection()));
                return;
            }

            if(event->type == GG_EVENT_NONE)
            {
                gg_event_free(event);
                continue;
            }

            if(event->type == GG_EVENT_CONN_SUCCESS)
            {
                if(!SendContactList())
                {
                    CleanAndExit();
                    return;
                }

                ChangeStatus(GetProfile()->getBotSettings().getDefaultDescription());
            }

            if(event->type == GG_EVENT_CONN_FAILED)
            {
                qDebug() << "Connection failed :(";
                //emit restartConnection();
                emit endServer();
                return;
            }

            eventManager.ResolveEvent(event);
            
            gg_event_free(event);
        }
    }
}

void SessionClient::sendMessage(QString message)
{
    if(!session)
        return;

    message = GGChatBot::makeMessage(message);
    QByteArray data = GGChatBot::unicode2cp(message);

    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    foreach(UserInfoTOPtr user, users)
    {
        if(GetProfile()->getUserDatabase()->isUserOnChannel(user))
            gg_send_message(session, GG_CLASS_CHAT, user->getUin(), (const unsigned char*)data.data());
    }
}

void SessionClient::sendMessage(uin_t uin, QString message)
{
    if(!session)
        return;

    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    foreach(UserInfoTOPtr user, users)
    {
        if(user->getUin() != uin && GetProfile()->getUserDatabase()->isUserOnChannel(user))
            sendMessageTo(user->getUin(), message);
    }
}

void SessionClient::sendMessageTo(uin_t uin, QString message)
{
    if(!session)
        return;
    
    QByteArray data = GGChatBot::unicode2cp(GGChatBot::makeMessage(message));

    gg_send_message(session, GG_CLASS_CHAT, uin, (const unsigned char*)data.data());
}

void SessionClient::sendMessageToSuperUser(uin_t uin, QString message)
{
    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    foreach(UserInfoTOPtr user, users)
    {
        if(user->getUin() == uin
           && GetProfile()->getUserDatabase()->isSuperUser(user)
           && GetProfile()->getUserDatabase()->isUserOnChannel(user))
            sendMessageTo(user->getUin(), message);
    }
}

