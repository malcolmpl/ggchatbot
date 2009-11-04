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

    unsigned char * format;
    gg_msg_richtext_format rtf;
    gg_msg_richtext_color rtc, rtc_black;
    rtf.font = 0;

    rtc_black.red = 0;
    rtc_black.green = 0;
    rtc_black.blue = 0;

    int formatlen = 2*(sizeof(rtf) + sizeof(rtc));

    format = new unsigned char[formatlen];

    if((GetProfile()->getUserDatabase()->isSuperUser(user)))
    {
        message = "<!" + user->getNick() + "> " + msg;
	rtc.red = 255;
	rtc.green = 0;
	rtc.blue = 0;
	rtf.position = 2;
	rtf.font |= GG_FONT_COLOR;
	memcpy(format, &rtf, sizeof(rtf));
	format += sizeof(rtf);
	memcpy(format, &rtc, sizeof(rtc));
	format += sizeof(rtc);
    }
    else if((GetProfile()->getUserDatabase()->isUserHaveOp(user)))
    {
        message = "<@" + user->getNick() + "> " + msg;
        rtc.red = 0;
        rtc.green = 0;
        rtc.blue = 255;
        rtf.position = 2;
        rtf.font |= GG_FONT_COLOR;
        memcpy(format, &rtf, sizeof(rtf));
        format += sizeof(rtf);
        memcpy(format, &rtc, sizeof(rtc));
        format += sizeof(rtc);

    }
    else if((GetProfile()->getUserDatabase()->isUserHaveVoice(user)))
    {
        message = "<+" + user->getNick() + "> " + msg;
        rtc.red = 0;
        rtc.green = 255;
        rtc.blue = 0;
        rtf.position = 2;
        rtf.font |= GG_FONT_COLOR;
        memcpy(format, &rtf, sizeof(rtf));
        format += sizeof(rtf);
        memcpy(format, &rtc, sizeof(rtc));
        format += sizeof(rtc);
    }
    else
    {
        message = "<" + user->getNick() + "> " + msg;
        rtc.red = 0;
        rtc.green = 0;
        rtc.blue = 0;
        rtf.position = 1;
        rtf.font |= GG_FONT_COLOR;
        memcpy(format, &rtf, sizeof(rtf));
        format += sizeof(rtf);
        memcpy(format, &rtc, sizeof(rtc));
        format += sizeof(rtc);
    }										

    rtf.font = 0;
    rtf.position = rtf.position + user->getNick().length();
    memcpy(format, &rtf, sizeof(rtf));
    format += sizeof(rtf);
    memcpy(format, &rtc_black, sizeof(rtc_black));
   
    emit sendMessageRichtext(sender, message, format, formatlen);
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
        welcome = "Witaj!\nWpisz /nick 'Nick' aby ustawic swoj nick.\nWpisz /join aby dolaczyc do czatu.";
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


