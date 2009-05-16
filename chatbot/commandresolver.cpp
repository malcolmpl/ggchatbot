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

#include "userinfoto.h"


#include "sessionclient.h"
#include "botsettings.h"
#include "userdatabase.h"
#include "commandresolver.h"

#include <QDebug>
#include <QRegExp>
#include <QDateTime>

namespace
{
    const int MAX_NICK_LENGTH		= 14;

    const QString CMD_NICK              = "/nick";
    const QString CMD_JOIN              = "/join";
    const QString CMD_START		= "/start";
    const QString CMD_LEAVE             = "/leave";
    const QString CMD_STOP		= "/stop";
    const QString CMD_QUIT              = "/quit";
    const QString CMD_WHO               = "/who";
    const QString CMD_KTO		= "/kto";
    const QString CMD_HELP              = "/help";
    const QString CMD_POMOC             = "/pomoc";
    const QString CMD_KICK              = "/kick";
    const QString CMD_BAN               = "/ban";
	const QString CMD_UNBAN				= "/unban";
    const QString CMD_TOPIC             = "/topic";
	const QString CMD_OP				= "/op";
	const QString CMD_VOICE				= "/voice";
	const QString CMD_REMOVEFLAGS		= "/removeflags";

    const QString MSG_NICK_EXIST        = "Uzytkownik o takim nicku juz istnieje!";
    const QString MSG_HELP              = "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/join /start - wejscie na czat\n/leave /stop /quit 'tekst'- opuszczenie czatu, opcjonalnie z tekstem\n" \
        "/who /kto - spis osob dostepnych na czacie\n/help /pomoc - pomoc ktora wlasnie czytasz ;)";

	const QString MSG_HELP_FOR_OPS		= "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/join /start - wejscie na czat\n/leave /stop /quit 'tekst'- opuszczenie czatu\n" \
        "/who /kto - spis osob dostepnych na czacie\n/help /pomoc - pomoc\n/ban /unban nick/numer czas opis - banowanie" \
		", czas w minutach, 0=rok\n/kick nick/numer opis - wywalenie z czatu\n/op numer - ustawia flage op'a\n" \
		"/voice numer - ustawia flage voice\n/removeflags numer - zdejmuje wszystkie flagi";
}

CommandResolver::CommandResolver()
{
}

CommandResolver::~CommandResolver()
{
}

bool CommandResolver::checkCommand(gg_event *event)
{
    m_event = event;
    
    QRegExp rx("^(/\\w+).*");
    QString str = QString::fromAscii((const char*)m_event->event.msg.message);
    int pos = 0;

    if((pos = rx.indexIn(str, pos)) != -1)
    {
        QString command = rx.cap(1);
        if(command == CMD_NICK)
        {
            lastString = removeCommand(str, CMD_NICK);
            nickCommand();
            return true;
        }
        else if(command == CMD_JOIN)
        {
            lastString = removeCommand(str, CMD_JOIN);
            joinCommand();
            return true;
        }
		else if(command == CMD_START)
		{
	    	lastString = removeCommand(str, CMD_START);
		    joinCommand();
		    return true;
		}
        else if(command == CMD_LEAVE)
        {
            lastString = removeCommand(str, CMD_LEAVE);
            leaveCommand();
            return true;
        }
		else if(command == CMD_STOP)
		{
	    	lastString = removeCommand(str, CMD_STOP);
		    leaveCommand();
		    return true;
		}
        else if(command == CMD_QUIT)
        {
            lastString = removeCommand(str, CMD_QUIT);
            leaveCommand();
            return true;
        }
        else if(command == CMD_WHO)
        {
            lastString = removeCommand(str, CMD_WHO);
            whoCommand();
            return true;
        }
		else if(command == CMD_KTO)
		{
	    	lastString = removeCommand(str, CMD_KTO);
		    whoCommand();
		    return true;
		}
        else if(command == CMD_HELP)
        {
            lastString = removeCommand(str, CMD_HELP);
            helpCommand();
            return true;
        }
        else if(command == CMD_POMOC)
        {
            lastString = removeCommand(str, CMD_POMOC);
            helpCommand();
            return true;
        }
        else if(command == CMD_KICK)
        {
            lastString = removeCommand(str, CMD_KICK);
            kickCommand();
            return true;
        }
        else if(command == CMD_BAN)
        {
            lastString = removeCommand(str, CMD_BAN);
            banCommand();
            return true;
        }
        else if(command == CMD_TOPIC)
        {
            lastString = removeCommand(str, CMD_TOPIC);
            topicCommand();
            return true;
        }
		else if(command == CMD_UNBAN)
		{
			lastString = removeCommand(str, CMD_UNBAN);
			unbanCommand();
			return true;
		}
		else if(command == CMD_OP)
		{
			lastString = removeCommand(str, CMD_OP);
			opCommand();
			return true;
		}
		else if(command == CMD_VOICE)
		{
			lastString = removeCommand(str, CMD_VOICE);
			voiceCommand();
			return true;
		}
		else if(command == CMD_REMOVEFLAGS)
		{
			lastString = removeCommand(str, CMD_REMOVEFLAGS);
			removeFlagsCommand();
			return true;
		}
    }

    return false;
}

QString CommandResolver::removeCommand(QString message, QString command)
{
    QString ret = message.right(message.size()-command.size());
    return ret.simplified();
}

void CommandResolver::nickCommand()
{
    if(lastString.isEmpty())
        return;
    
    QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
		UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    	    QString newNick = rx.cap(1);
		if(newNick.size() > MAX_NICK_LENGTH)
		{
	    	QString msg = "Maksymalna dlugosc nicka to 14 znakow!";
	    	GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
	    	return;
		}

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            if(u->getNick() == newNick)
            {
                if(u->getUin() == user->getUin())
                {
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Hej %1, juz masz ustawiony nick!").arg(GetProfile()->getUserDatabase()->makeUserNick(user)));
                    return;
                }
                GetProfile()->getSession()->sendMessageTo(user->getUin(), MSG_NICK_EXIST);
                return;
            }
        }

        if(!user->getNick().isEmpty())
        {
			QString oldNick = GetProfile()->getUserDatabase()->makeUserNick(user);
			user->setNick(newNick);
			QString newNick = GetProfile()->getUserDatabase()->makeUserNick(user);
            QString message = QString("%1 zmienia nick na %2").arg(oldNick).arg(newNick);
            GetProfile()->getSession()->sendMessage(message);
        }
		else
		{
	        QString debugMessage = QString("%1 %2 zmienia nick na %3").arg(user->getUin()).arg(user->getNick()).arg(newNick);
    	    qDebug() << debugMessage;
        	//GetProfile()->getSession()->sendMessageToSuperUser(user->getUin(), debugMessage);
	        user->setNick(newNick);
    	    GetProfile()->getUserDatabase()->saveDatabase();
		}
    }
}

void CommandResolver::joinCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(user->getOnChannel())
        return;

    if(user->getNick().isEmpty())
	return;

    if(user->getBanned())
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        QDateTime banTime = user->getBanTime();
        if(currentTime>banTime)
        {
            user->setBanned(false);     // zdejmujemy bana, moze wejsc
        }
        else
        { // a jesli ma bana, to wyjazd
            QString msg;
            if(user->getBanReason().isEmpty())
                msg = "Przykro nam, masz zakaz wjazdu do: " + banTime.toString("dd MMMM yyyy h:mm:ss");
            else
                msg = QString("Przykro nam, masz zakaz wjazdu do: %1. Powod: %2").arg(banTime.toString("dd MMMM yyyy h:mm:ss")).arg(user->getBanReason());

            GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
            return;
        }
    }

    user->setOnChannel(true);
    GetProfile()->getUserDatabase()->saveDatabase();
    whoCommand();
    QString msg = "Przychodzi " + GetProfile()->getUserDatabase()->makeUserNick(user);
    qDebug() << "UIN:" << user->getUin() << msg;
    GetProfile()->getSession()->sendMessage(msg);
}

void CommandResolver::leaveCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel())
        return;

    QString reason;
    if(!lastString.isEmpty())
    {
        reason = lastString;
    }

    QString msg = "Odchodzi " + GetProfile()->getUserDatabase()->makeUserNick(user) + " " + reason;
    qDebug() << "UIN:" << user->getUin() << msg;
    GetProfile()->getSession()->sendMessage(msg);
    user->setOnChannel(false);
    GetProfile()->getUserDatabase()->saveDatabase();
}

void CommandResolver::whoCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel())
        return;

    QString listOfUsers = "Osoby na czacie:\n";
    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
	QList<UserInfoTOPtr> usersOnChannel;
	foreach(UserInfoTOPtr u, users)
	{
		if(GetProfile()->getUserDatabase()->isUserOnChannel(u->getUin()))
			usersOnChannel.push_back(u);
	}

	int i = 0;
	if(user->getUserFlags() > GGChatBot::OP_USER_FLAG)
	{
		foreach(UserInfoTOPtr u, usersOnChannel)
		{
			QString userNick = GetProfile()->getUserDatabase()->makeUserNick(u);
			if(i<usersOnChannel.size())
				listOfUsers += QString("[%1] %2, ").arg(u->getUin()).arg(userNick);
			else
				listOfUsers += QString("[%1] %2").arg(u->getUin()).arg(userNick);
			i++;
		}
	}
	else
	{
    	foreach(UserInfoTOPtr u, usersOnChannel)
	    {
			QString userNick = GetProfile()->getUserDatabase()->makeUserNick(u);
			if(i<usersOnChannel.size())
        	   	listOfUsers += userNick + ", ";
			else
				listOfUsers += userNick;
			i++;
	    }
	}

    QString msg = QString("%1 %2 %3").arg(user->getUin()).arg(user->getNick()).arg(CMD_WHO);
    //GetProfile()->getSession()->sendMessageToSuperUser(user->getUin(), msg);
    GetProfile()->getSession()->sendMessageTo(user->getUin(), listOfUsers);
}

void CommandResolver::helpCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel())
        return;

	if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
		GetProfile()->getSession()->sendMessageTo(user->getUin(), MSG_HELP);
	else
    	GetProfile()->getSession()->sendMessageTo(user->getUin(), MSG_HELP_FOR_OPS);
}

void CommandResolver::kickCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    QRegExp rx2("^(\\d+).*");
    int pos = 0;

    if((pos = rx2.indexIn(lastString, pos)) != -1)
    {
        QString uinToKick = rx2.cap(1);
        lastString = removeCommand(rx2.cap(0), uinToKick);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            QString uin = QString("%1").arg(u->getUin());
            if(uin == uinToKick)
            {
                kickHelperCommand(u);
                return;
            }
        }
    }

    QRegExp rx("^(\\w+).*");
    pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString nickToKick = rx.cap(1);
        lastString = removeCommand(rx.cap(0), nickToKick);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            if(u->getNick() == nickToKick)
            {
                kickHelperCommand(u);
                return;
            }
        }
    }
}

void CommandResolver::kickHelperCommand(UserInfoTOPtr user)
{
    QString msg = QString("%1 wylatuje z czatu. %2").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(lastString);
    if(user->getOnChannel())
        GetProfile()->getSession()->sendMessage(msg);
    user->setOnChannel(false);
}

void CommandResolver::banCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    QRegExp rx("^(\\w+)\\s+(\\w+)\\s+(.*)");
    int pos = 0;
    QStringList list;

    QString nick = "";
    uint minutes = 0;
    QString description = "";

    qDebug() << lastString;

    while((pos = rx.indexIn(lastString, pos)) != -1)
    {
        list << rx.cap(1);
        list << rx.cap(2);
        list << rx.cap(3);
        pos += rx.matchedLength();
    }

    if(list.size() < 2)
        return;

    bool isOk = false;
    nick = list[0];
    list[1].toInt(&isOk);
    if(isOk)
        minutes = list[1].toInt(&isOk);
    if(list.size()==3)
        description = list[2];

    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    UserInfoTOPtr u;
    foreach(u, users)
    {
        isOk = false;
        nick.toInt(&isOk);
        if(isOk)
        {
            QString uin = QString("%1").arg(u->getUin());
            if(uin == nick)
            {
                banHelperCommand(u, minutes, description);
                break;
            }
        }
        else
        {
            QString nickStr = u->getNick();
            if(nickStr == nick)
            {
                banHelperCommand(u, minutes, description);
                break;
            }
        }
    }
}

void CommandResolver::banHelperCommand(UserInfoTOPtr user, uint banTime, QString description)
{
    QString message;
    if(description.isEmpty())
        message = QString("%1 zostal zbanowany na %2 minut.").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(banTime);
    else
        message = QString("%1 zostal zbanowany na %2 minut. Powod: %3").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(banTime).arg(description);

    GetProfile()->getSession()->sendMessage(message);

    lastString = "";
    kickHelperCommand(user);
    user->setBanned(true);
    user->setBanReason(description);
    QDateTime currentDate = QDateTime::currentDateTime();
    if(banTime == 0)
        currentDate = currentDate.addYears(1);        // jesli banTime==0 to dajemy bana na rok
    else
        currentDate = currentDate.addSecs(banTime*60);
    user->setBanTime(currentDate);
}

void CommandResolver::unbanCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    QRegExp rx2("^(\\d+).*");
    int pos = 0;

    if((pos = rx2.indexIn(lastString, pos)) != -1)
    {
        QString uinToUnban = rx2.cap(1);
        lastString = removeCommand(rx2.cap(0), uinToUnban);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            QString uin = QString("%1").arg(u->getUin());
            if(uin == uinToUnban)
            {
                unbanHelperCommand(u);
                return;
            }
        }
    }

    QRegExp rx("^(\\w+).*");
    pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString nickToUnban = rx.cap(1);
        lastString = removeCommand(rx.cap(0), nickToUnban);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            if(u->getNick() == nickToUnban)
            {
                unbanHelperCommand(u);
                return;
            }
        }
    }
}

void CommandResolver::unbanHelperCommand(UserInfoTOPtr u)
{
	QString message = QString("%1 zostal odbanowany.").arg(GetProfile()->getUserDatabase()->makeUserNick(u));
	GetProfile()->getSession()->sendMessage(message);
	u->setBanned(false);
	u->setBanTime(QDateTime());
}

void CommandResolver::topicCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    QRegExp rx("^(.*)");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString topic = rx.cap(1);

        GetProfile()->getSession()->ChangeStatus(topic);
		
		QString message = QString("%1 zmienil temat na: %2").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(topic);
		GetProfile()->getSession()->sendMessage(message);
    }
}

void CommandResolver::opCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;
   
	QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString uinToOp = rx.cap(1);
        lastString = removeCommand(rx.cap(0), uinToOp);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
			QString uin = QString("%1").arg(u->getUin());
            if(uin == uinToOp)
            {
				if(u->getUserFlags() < GGChatBot::SUPER_USER_FLAG)
				{
					QString msg = QString("%1 ustawia op dla %2").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(u->getNick());
					u->setUserFlags(GGChatBot::OP_USER_FLAG);
					GetProfile()->getSession()->sendMessage(msg);
            	    return;
				}
				else
				{
					GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Nie mozesz tego zrobic!"));
					return;
				}
            }
        }
    }
}

void CommandResolver::voiceCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;
   
	QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString uinToVoice = rx.cap(1);
        lastString = removeCommand(rx.cap(0), uinToVoice);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
			QString uin = QString("%1").arg(u->getUin());
            if(uin == uinToVoice)
            {
				if(u->getUserFlags() < GGChatBot::SUPER_USER_FLAG)
				{
					QString msg = QString("%1 ustawia voice dla %2").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(u->getNick());
					u->setUserFlags(GGChatBot::VOICE_USER_FLAG);
					GetProfile()->getSession()->sendMessage(msg);
            	    return;
				}
				else
				{
					GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Nie mozesz tego zrobic!"));
					return;
				}
            }
        }
    }
}

void CommandResolver::removeFlagsCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;
   
	QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString uinToRM = rx.cap(1);
        lastString = removeCommand(rx.cap(0), uinToRM);

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
			QString uin = QString("%1").arg(u->getUin());
            if(uin == uinToRM)
            {
				if(u->getUserFlags() < GGChatBot::SUPER_USER_FLAG)
				{
					QString msg = QString("%1 zabiera przywileje %2").arg(GetProfile()->getUserDatabase()->makeUserNick(user)).arg(u->getNick());
					u->setUserFlags(GGChatBot::NONE_FLAG);
					GetProfile()->getSession()->sendMessage(msg);
            	    return;
				}
            }
        }
    }
};

