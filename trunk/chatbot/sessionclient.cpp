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

SessionClient::SessionClient()
{
    settings = SettingsPtr(new QSettings("ggbot.ini", QSettings::IniFormat));
    ReadSettings();
    SaveSettings();
}

SessionClient::~SessionClient()
{
    SaveSettings();
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

    if(!SendContactList())
    {
        emit endServer();
        return;
    }

    ChangeStatus(m_defaultDescription);

    EventLoop();

    CleanEndExit();
}

void SessionClient::ReadSettings()
{
    qDebug() << "ReadSettings() called";

    settings->beginGroup("Main");
    m_level = settings->value("DebugLevel", 0).toInt();
    m_uin = settings->value("UIN", 1234).toUInt();
    m_password = settings->value("Password", "pass").toString();
    m_defaultDescription = settings->value("DefaultDescription", "v0.3").toString();
    settings->endGroup();
}

void SessionClient::SaveSettings()
{
    qDebug() << "SaveSettings() called";

    settings->beginGroup("Main");
    settings->setValue("DebugLevel", m_level);
    settings->setValue("UIN", m_uin);
    settings->setValue("Password", m_password);
    settings->setValue("DefaultDescription", m_defaultDescription);
    settings->endGroup();
}

void SessionClient::FreeSession(gg_session *session)
{
    qDebug() << "FreeSession() called";
    gg_free_session(session);
}

void SessionClient::Logout(gg_session *session)
{
    qDebug() << "Logout() called";
    gg_logoff(session);
}

void SessionClient::CleanEndExit()
{
    Logout(session);
    FreeSession(session);

    qDebug() << "Bye, bye";
    emit endServer();
}

void SessionClient::SetDebugLevel()
{
    qDebug() << "SetDebugLevel() called";

    gg_debug_level = m_level;
}

bool SessionClient::Login()
{
    qDebug() << "Login() called";
    memset(&loginParams, 0, sizeof(loginParams));

    loginParams.uin = m_uin;
    loginParams.password = m_password.toAscii().data();

    if (!( session = gg_login(&loginParams) ) )
    {
        qDebug() << "Nie udalo sie polaczyc";   //printf("Nie uda?o si? po??czy?: %s\n", strerror(errno));
        CleanEndExit();
        return false;
    }

    qDebug() << "Polaczono";
    return true;
}

bool SessionClient::SendContactList()
{
    qDebug() << "SendContactList called()";
    if(gg_notify_ex(session, NULL, NULL,0  ) == -1)
    {
        qDebug() << "Blad wysylania listy kontaktow na serwer";
        CleanEndExit();
        return false;
    }

    qDebug() << "Lista kontaktow wyslana poprawnie";
    return true;
}

void SessionClient::ChangeStatus(QString description, int status)
{
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
        if(!WaitForEvent())
            return;

        eventManager.ResolveEvent(event);

        gg_free_event(event);
    }
}
