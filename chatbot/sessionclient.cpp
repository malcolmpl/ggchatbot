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

#include <errno.h>

SessionClient::SessionClient()
{
}

SessionClient::~SessionClient()
{
}

void SessionClient::MakeConnection()
{
    qDebug() << "MakeConnection() called";
    
    SetDebugLevel();

    if(!Login())
    {
        emit endServer();
        return;
    }

    EventLoop();
}

void SessionClient::FreeSession(gg_session *session)
{
    qDebug() << "FreeSession() called";
    gg_free_event(event);
    gg_free_session(session);
}

void SessionClient::Logout(gg_session *session)
{
    qDebug() << "Logout() called";
    gg_logoff(session);
}

void SessionClient::CleanAndExit()
{
    Logout(session);
    FreeSession(session);

    qDebug() << "Bye, bye";
    emit endServer();
}

void SessionClient::SetDebugLevel()
{
    qDebug() << "SetDebugLevel() called";

    gg_debug_level = GetProfile()->getBotSettings().getDebugLevel();
}

bool SessionClient::Login()
{
    qDebug() << "Login() called";
    memset(&loginParams, 0, sizeof(loginParams));

    loginParams.uin = GetProfile()->getBotSettings().getUin();
    loginParams.password = GetProfile()->getBotSettings().getPassword().toAscii().data();
    loginParams.async = 1;

    if (!( session = gg_login(&loginParams) ) )
    {
        qDebug() << "Nie udalo sie polaczyc";   //printf("Nie uda?o si? po??czy?: %s\n", strerror(errno));
        CleanAndExit();
        return false;
    }

    qDebug() << "Polaczono";

    scheduler = new SessionScheduler(session);
    scheduler->start();
    
    return true;
}

bool SessionClient::SendContactList()
{
    qDebug() << "SendContactList called()";
    if(gg_notify(session, NULL, 0) == -1)
    {
        qDebug() << "Blad wysylania listy kontaktow na serwer";
        return false;
    }

    qDebug() << "Lista kontaktow wyslana poprawnie";
    return true;
}

void SessionClient::ChangeStatus(QString description, int status)
{
    qDebug() << "Zmiana statusu: " << description;
    gg_change_status_descr(session, status, description.toAscii());
}

bool SessionClient::WaitForEvent()
{
    qDebug() << "WaitForEvent() called";

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
            qApp->processEvents();
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
                qDebug() << "None";
                CleanAndExit();
                return;
            }

            if(event->type == GG_EVENT_NONE)
            {
                gg_event_free(event);
                continue;
            }

            qDebug() << event->type;

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
                emit restartConnection();
                return;
            }

            eventManager.ResolveEvent(event);
            
            gg_event_free(event);
        }
    }
}
