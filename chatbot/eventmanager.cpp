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
#include "debug.h"

#include <QtDebug>

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

    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(sender);
	qDebug() << "EventManager:" << user->getNick() << user->getUin() << user->getUserFlags();
    QString msg = QString::fromAscii((const char*)m_event->event.msg.message);
    QString message;

	if((GetProfile()->getUserDatabase()->isSuperUser(user->getUin())))
		message = "!" + user->getNick() + ": " + msg;
	else if((GetProfile()->getUserDatabase()->isUserHaveOp(user->getUin())))
		message = "@" + user->getNick() + ": " + msg;
	else if((GetProfile()->getUserDatabase()->isUserHaveVoice(user->getUin())))
		message = "+" + user->getNick() + ": " + msg;
	else
		message = user->getNick() + ": " + msg;

    emit sendMessage(user->getUin(), message);
    showUserDebug(user, msg);
}

bool EventManager::isUserOnChannel(uin_t uin)
{
    return GetProfile()->getUserDatabase()->isUserOnChannel(uin);
}

bool EventManager::isUserInDatabase(uin_t uin)
{
    return GetProfile()->getUserDatabase()->isUserInDatabase(uin);
}

UserInfoTOPtr EventManager::getUser(uin_t uin)
{
    return GetProfile()->getUserDatabase()->getUserInfo(uin);
}

void EventManager::welcomeMessage()
{
    UserInfoTOPtr user = getUser(m_event->event.msg.sender);
    QString msg = QString::fromAscii((const char*)m_event->event.msg.message);
    showUserDebug(user, msg);
    QString welcome;
    if(user->getNick().isEmpty())
        welcome = "Witaj!\nWpisz /nick 'Nick' aby ustawic swoj nick.\nWpisz /join aby dolaczyc do czatu.";
    else
        welcome = QString("Witaj %1! Wpisz /join aby dolaczyc do czatu.").arg(user->getNick());
    emit sendMessageTo(m_event->event.msg.sender, welcome);
}

bool EventManager::checkCommand()
{
    cmdResolv.SetProfile(GetProfile());
    return cmdResolv.checkCommand(m_event);
}


