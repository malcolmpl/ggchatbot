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

#include "userdatabase.h"
#include "eventmanager.h"
#include "userinfoto.h"

#include <QtDebug>
#include <QRegExp>

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::ResolveEvent(gg_event *event)
{
    m_event = event;
    switch(event->type)
    {
        case GG_EVENT_ACK:
        {
            AckEvent();
        }
        break;

        case GG_EVENT_MSG:
        {
            MessageEvent();
        }
        break;
    }
}

void EventManager::AckEvent()
{
    qDebug() << "ACK EVENT";
}

void EventManager::MessageEvent()
{
    // dodajemy usera do listy
    uin_t sender = m_event->event.msg.sender;
    isUserOnChannel(sender);
    
    if(checkCommand())
        return;
    
    if(!isUserOnChannel(sender))
    {
        welcomeMessage();
        return;
    }
    qDebug() << "MSG EVENT "; //<< QString(m_event->event.msg.sender);
    emit sendMessage("Dupa");
}

bool EventManager::isUserOnChannel(uin_t uin)
{
    if(!isUserInDatabase(uin))
        return false;

    UserInfoTOPtr user = getUser(uin);
    if(user->getNick().isEmpty())
        return false;

    if(!user->getOnChannel())
        return false;

    return true;
}

bool EventManager::isUserInDatabase(uin_t uin)
{
    if(getUser(uin) == NULL)
    {
        UserInfoTOPtr user = UserInfoTOPtr(new UserInfoTO());
        user->setUin(uin);
        GetProfile()->getUserDatabase()->addUser(user);

        return false;
    }

    return true;
}

UserInfoTOPtr EventManager::getUser(uin_t uin)
{
    return GetProfile()->getUserDatabase()->getUserInfo(uin);
}

void EventManager::welcomeMessage()
{
    QString welcome = "Witaj!\nWpisz /nick aby ustawic swoj nick.";
    emit sendMessageTo(m_event->event.msg.sender, welcome);
}

bool EventManager::checkCommand()
{
    QRegExp rx("(/\\w+).*");
    QString str = QString::fromAscii((const char*)m_event->event.msg.message);
    int pos = 0;

    if((pos = rx.indexIn(str, pos)) != -1)
    {
        if(rx.cap(1) == "/nick")
        {
            qDebug() << "Nick Command";
            return true;
        }
    }

    return false;
}


