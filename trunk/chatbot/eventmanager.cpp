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
#include "profile.h"

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

    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(sender);
    QString msg = GGChatBot::makeInternalMessage(GGChatBot::cp2unicode((const char*)m_event->event.msg.message));
    QString message;

    // refresh user time action
    user->setLastSeen(QDateTime::currentDateTime());

    if(checkCommand())
    {
        showUserDebug(user, msg);
        return;
    }
    
    if(!GetProfile()->getUserDatabase()->isUserOnChannel(user))
    {
        welcomeMessage();
        return;
    }

    if(user->getLastMessage() == msg)
    {
        message = "Nie powtarzaj sie!";
        emit sendMessageTo(user->getUin(), message);
        return;
    }
    else
    {
        user->setLastMessage(msg);
    }

    unsigned char * result;
    unsigned int memoryPosition = sizeof(gg_msg_richtext);
    gg_msg_richtext header;
    gg_msg_richtext_format format;
    gg_msg_richtext_color color, color_black;
    format.font = 0;

    int resultlen = sizeof(gg_msg_richtext) + 2*(sizeof(format) + sizeof(color));
    result = new unsigned char[resultlen];

    header.flag = 2;
    header.length = gg_fix16(resultlen - sizeof(gg_msg_richtext));
    memcpy(result, &header, sizeof(header));

    color_black.red = 0;
    color_black.green = 0;
    color_black.blue = 0;

    if((GetProfile()->getUserDatabase()->isSuperUser(user)))
    {
        message = "<!" + user->getNick() + "> " + msg;
	color.red = 255;
	color.green = 0;
	color.blue = 0;
	format.position = gg_fix16(2);
	format.font |= GG_FONT_COLOR;
	memcpy(result + memoryPosition, &format, sizeof(format));
	memoryPosition += sizeof(format);
	memcpy(result + memoryPosition, &color, sizeof(color));
    	memoryPosition += sizeof(color);
    }
    else if((GetProfile()->getUserDatabase()->isUserHaveOp(user)))
    {
        message = "<@" + user->getNick() + "> " + msg;
        color.red = 51;
        color.green = 0;
        color.blue = 204;
        format.position = gg_fix16(2);
        format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);

    }
    else if((GetProfile()->getUserDatabase()->isUserHaveVoice(user)))
    {
        message = "<+" + user->getNick() + "> " + msg;
        color.red = 0;
        color.green = 150;
        color.blue = 0;
        format.position = gg_fix16(2);
        format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);
    }
    else
    {
        message = "<" + user->getNick() + "> " + msg;
        color.red = 100;
        color.green = 0;
        color.blue = 0;
        format.position = gg_fix16(1);
        format.font |= GG_FONT_COLOR;
        memcpy(result + memoryPosition, &format, sizeof(format));
        memoryPosition += sizeof(format);
        memcpy(result + memoryPosition, &color, sizeof(color));
        memoryPosition += sizeof(color);
    }										

    format.font = 0;
    format.position = gg_fix16(format.position + user->getNick().length());
    memcpy(result + memoryPosition, &format, sizeof(format));
    memoryPosition += sizeof(format);
    memcpy(result + memoryPosition, &color_black, sizeof(color_black));
   
    emit sendMessageRichtext(sender, message, result, resultlen);
    showUserDebug(user, msg);
}

void EventManager::welcomeMessage()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    QString msg = QString::fromAscii((const char*)m_event->event.msg.message);
    showUserDebug(user, msg);
    QString welcome;
    if(user->getNick().isEmpty())
    {
        welcome = "Witaj!\nWpisz /nick 'Nick' aby ustawic swoj nick a nastepnie\nwpisz /join aby dolaczyc do czatu.\nnp:\n/nick Ania\n/join\n\nMilej zabawy :-)";
	emit sendMessageTo(m_event->event.msg.sender, welcome);
    }
    else
    {
        GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
        welcome = QString("Witaj %1! Wpisz /join aby dolaczyc do czatu.").arg(userNick.nick);
	emit sendMessageRichtextTo(m_event->event.msg.sender, welcome, userNick.format, userNick.formatlen);
    }
}

bool EventManager::checkCommand()
{
    cmdResolv.SetProfile(GetProfile());
    return cmdResolv.checkCommand(m_event);
}


