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
#include "profile.h"
#include "common.h"

#include <QDebug>
#include <QRegExp>
#include <QDateTime>

namespace
{
    const int MAX_NICK_LENGTH           = 14;

    const QString CMD_NICK              = "/nick";
    const QString CMD_JOIN              = "/join";
    const QString CMD_JOIN_ALIAS        = "/j";
    const QString CMD_START             = "/start";
    const QString CMD_START_ALIAS       = "/s";
    const QString CMD_LEAVE             = "/leave";
    const QString CMD_LEAVE_ALIAS       = "/l";
    const QString CMD_STOP              = "/stop";
    const QString CMD_QUIT              = "/quit";
    const QString CMD_QUIT_ALIAS        = "/q";
    const QString CMD_KONIEC		= "/koniec";
    const QString CMD_WHO               = "/who";
    const QString CMD_KTO               = "/kto";
    const QString CMD_HELP              = "/help";
    const QString CMD_HELP_ALIAS        = "/h";
    const QString CMD_POMOC             = "/pomoc";
    const QString CMD_KICK              = "/kick";
    const QString CMD_KICK_ALIAS        = "/k";
    const QString CMD_BAN               = "/ban";
    const QString CMD_BAN_ALIAS         = "/b";
    const QString CMD_UNBAN             = "/unban";
    const QString CMD_TOPIC             = "/topic";
    const QString CMD_TOPIC_ALIAS       = "/t";
    const QString CMD_OP                = "/op";
    const QString CMD_VOICE             = "/voice";
    const QString CMD_REMOVEFLAGS       = "/removeflags";
    const QString CMD_MODERATE          = "/moderate";
    const QString CMD_UNMODERATE        = "/unmoderate";

    const QString MSG_NICK_EXIST        = "Uzytkownik o takim nicku juz istnieje!";
    const QString MSG_HELP              = "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/start - wejscie na czat\n/koniec 'tekst'- opuszczenie czatu, opcjonalnie z tekstem\n" \
        "/kto - spis osob dostepnych na czacie\n/pomoc - pomoc ktora wlasnie czytasz ;)";

    const QString MSG_HELP_FOR_OPS      = "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/join /start - wejscie na czat\n/leave /stop /quit /koniec 'tekst'- opuszczenie czatu\n" \
        "/who /kto - spis osob dostepnych na czacie\n/help /pomoc - pomoc\n/ban /unban nick/numer czas opis - banowanie" \
        ", czas w minutach, 0=rok\n/kick nick/numer opis - wywalenie z czatu\n/op numer - ustawia flage op'a\n" \
        "/voice numer - ustawia flage voice\n/removeflags numer - zdejmuje wszystkie flagi\n" \
        "/moderate /unmoderate - ustawia/zdejmuje czat moderowany";
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
    QString str = GGChatBot::makeInternalMessage(GGChatBot::cp2unicode((const char*)m_event->event.msg.message));
    int pos = 0;

    if((pos = rx.indexIn(str, pos)) != -1)
    {
        QString command = rx.cap(1);
        if(command.compare(CMD_NICK, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_NICK);
            nickCommand();
            return true;
        }
        else if(command.compare(CMD_JOIN, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_JOIN);
            joinCommand();
            return true;
        }
        else if(command.compare(CMD_JOIN_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_JOIN_ALIAS);
            joinCommand();
            return true;
        }
        else if(command.compare(CMD_START, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_START);
            joinCommand();
            return true;
        }
        else if(command.compare(CMD_START_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_START_ALIAS);
            joinCommand();
            return true;
        }
        else if(command.compare(CMD_LEAVE, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_LEAVE);
            leaveCommand();
            return true;
        }
        else if(command.compare(CMD_LEAVE_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_LEAVE_ALIAS);
            leaveCommand();
            return true;
        }
        else if(command.compare(CMD_STOP, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_STOP);
            leaveCommand();
            return true;
        }
        else if(command.compare(CMD_QUIT, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_QUIT);
            leaveCommand();
            return true;
        }
        else if(command.compare(CMD_QUIT_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_QUIT_ALIAS);
            leaveCommand();
            return true;
        }
	else if(command.compare(CMD_KONIEC, Qt::CaseInsensitive)==0)
	{
	    lastString = removeCommand(str, CMD_KONIEC);
	    leaveCommand();
	    return true;
	}
        else if(command.compare(CMD_WHO, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_WHO);
            whoCommand();
            return true;
        }
        else if(command.compare(CMD_KTO, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_KTO);
            whoCommand();
            return true;
        }
        else if(command.compare(CMD_HELP, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_HELP);
            helpCommand();
            return true;
        }
        else if(command.compare(CMD_HELP_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_HELP_ALIAS);
            helpCommand();
            return true;
        }
        else if(command.compare(CMD_POMOC, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_POMOC);
            helpCommand();
            return true;
        }
        else if(command.compare(CMD_KICK, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_KICK);
            kickCommand();
            return true;
        }
        else if(command.compare(CMD_KICK_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_KICK_ALIAS);
            kickCommand();
            return true;
        }
        else if(command.compare(CMD_BAN, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_BAN);
            banCommand();
            return true;
        }
        else if(command.compare(CMD_BAN_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_BAN_ALIAS);
            banCommand();
            return true;
        }
        else if(command.compare(CMD_TOPIC, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_TOPIC);
            topicCommand();
            return true;
        }
        else if(command.compare(CMD_TOPIC_ALIAS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_TOPIC_ALIAS);
            topicCommand();
            return true;
        }
        else if(command.compare(CMD_UNBAN, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_UNBAN);
            unbanCommand();
            return true;
        }
        else if(command.compare(CMD_OP, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_OP);
            opCommand();
            return true;
        }
        else if(command.compare(CMD_VOICE, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_VOICE);
            voiceCommand();
            return true;
        }
        else if(command.compare(CMD_REMOVEFLAGS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_REMOVEFLAGS);
            removeFlagsCommand();
            return true;
        }
        else if(command.compare(CMD_MODERATE, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_MODERATE);
            moderateCommand();
            return true;
        }
        else if(command.compare(CMD_UNMODERATE, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_UNMODERATE);
            unmoderateCommand();
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
    if(lastString.isEmpty() || m_channelFlags>0)
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
		    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Hej %1, juz masz ustawiony nick!").arg(userNick.nick));
                    return;
                }
                GetProfile()->getSession()->sendMessageTo(user->getUin(), MSG_NICK_EXIST);
                return;
            }
        }

        if(!user->getNick().isEmpty())
        {
	    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
	    QString oldNick = userNick.nick;
	    user->setNick(newNick);
            if(user->getOnChannel())
            {
		GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
                QString newNick = userNick.nick;
                QString message = QString("%1 zmienia nick na %2").arg(oldNick).arg(newNick);
                GetProfile()->getSession()->sendMessage(user->getUin(), message);

		message = QString("Zmieniasz nick na %1").arg(newNick);
		GetProfile()->getSession()->sendMessageTo(user->getUin(), message);
            }
        }
        else
        {
            QString debugMessage = QString("%1 %2 zmienia nick na %3").arg(user->getUin()).arg(user->getNick()).arg(newNick);
    	    qDebug() << debugMessage;
            GetProfile()->getSession()->sendMessageToSuperUser(user->getUin(), debugMessage);
	    user->setNick(newNick);
    	    GetProfile()->getUserDatabase()->saveDatabase();
        }
    }
}

void CommandResolver::joinCommand()
{
    setTopic(m_topic, false);
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(user->getOnChannel())
        return;

    if(user->getNick().isEmpty())
	user->setNick(QString("Ktos%1").arg(user->getUin()));

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

            // clear old banned persons nicks
            user->setNick(QString());
            return;
        }
    }

    user->setOnChannel(true);
    GetProfile()->getUserDatabase()->saveDatabase();
    whoCommand();
    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
    QString msg = "Przychodzi " + userNick.nick;
    qDebug() << "UIN:" << user->getUin() << msg;

    if(m_channelFlags==0)
        GetProfile()->getSession()->sendMessage(msg);
}

void CommandResolver::leaveCommand()
{
    setTopic(m_topic, false);
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel())
        return;

    QString reason;
    if(!lastString.isEmpty())
    {
        reason = lastString;
    }

    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
    QString msg = "Odchodzi " + userNick.nick + " " + reason;
    qDebug() << "UIN:" << user->getUin() << msg;

    if(m_channelFlags==0)
        GetProfile()->getSession()->sendMessage(msg);

    user->setOnChannel(false);
    GetProfile()->getUserDatabase()->saveDatabase();
}

void CommandResolver::whoCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel() && user->getUserFlags() <= GGChatBot::OP_USER_FLAG)
        return;

    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    QList<UserInfoTOPtr> usersOnChannel;
    foreach(UserInfoTOPtr u, users)
    {
        if(GetProfile()->getUserDatabase()->isUserOnChannel(u))
        usersOnChannel.push_back(u);
    }

    QString listOfUsers = QString("Osoby na czacie [%1]:\n").arg(usersOnChannel.size());

    int i = 1;
    if(user->getUserFlags() >= GGChatBot::OP_USER_FLAG)
    {
        foreach(UserInfoTOPtr u, usersOnChannel)
        {
            GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(u);
            if(i<usersOnChannel.size())
                listOfUsers += QString("[%1] %2, ").arg(u->getUin()).arg(userNick.nick);
            else
                listOfUsers += QString("[%1] %2").arg(u->getUin()).arg(userNick.nick);
            i++;
        }
    }
    else
    {
    	foreach(UserInfoTOPtr u, usersOnChannel)
        {
            GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(u);
            if(i<usersOnChannel.size())
                listOfUsers += userNick.nick + ", ";
            else
                listOfUsers += userNick.nick;
            i++;
        }
    }

    QString whoDesc = GetProfile()->getBotSettings().getWhoDescription();
    if(!whoDesc.isEmpty())
        listOfUsers += "\n" + whoDesc;

    if(m_channelFlags)
        listOfUsers += "\nCZAT MODEROWANY !! Tylko osoby z voice/op moga rozmawiac.";

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
    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
    QString msg = QString("%1 wylatuje z czatu. %2").arg(userNick.nick).arg(lastString);
    if(user->getOnChannel())
        GetProfile()->getSession()->sendMessage(msg);
    user->setOnChannel(false);
    qDebug() << msg;
    setTopic(m_topic, false);
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
    QString nickName = user->getNick();
    
    if(nickName.isEmpty())
        nickName = user->getUin();

    if(description.isEmpty())
        message = QString("%1 dostaje bana na %2 minut.").arg(nickName).arg(banTime);
    else
        message = QString("%1 dostaje bana na %2 minut. Powod: %3").arg(nickName).arg(banTime).arg(description);

    GetProfile()->getSession()->sendMessage(message);

    lastString = "";
    kickHelperCommand(user);
    user->setBanned(true);
    user->setBanReason(description);
    user->setNick(QString());
    QDateTime currentDate = QDateTime::currentDateTime();
    if(banTime == 0)
        currentDate = currentDate.addYears(1);        // jesli banTime==0 to dajemy bana na rok
    else
        currentDate = currentDate.addSecs(banTime*60);
    user->setBanTime(currentDate);

    qDebug() << message;
    setTopic(m_topic, false);
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
    if(!u->isBanned())
	return;

    QString message = QString("%1 zostal odbanowany.").arg(u->getUin());
    GetProfile()->getSession()->sendMessage(message);
    u->setBanned(false);
    u->setBanTime(QDateTime());
    qDebug() << message;
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
        m_topic = rx.cap(1);
        setTopic(m_topic);
    }
}

void CommandResolver::setTopic(QString topic, bool showMessage)
{
    // Remove line below if you want setting automatically topic
    if(!showMessage)
        return;


    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    int usersCount = 0;
    QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
    QString oldTopic = topic;

    foreach(UserInfoTOPtr u, users)
    {
        if(u->getOnChannel())
	    usersCount++;
    }

    QString usersCountText = QString(" | %1 os.").arg(usersCount);
//  if(topic.size()+usersCountText.size() > 255)
//      topic.chop(usersCountText.size());
//  topic += usersCountText;

    GetProfile()->getSession()->ChangeStatus(topic);
	
    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);	
    QString message = QString("%1 zmienia temat na: %2").arg(userNick.nick).arg(oldTopic);

    if(showMessage)
        GetProfile()->getSession()->sendMessage(message);

    qDebug() << message;
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
		    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
                    QString msg = QString("%1 ustawia op dla %2").arg(userNick.nick).arg(u->getNick());
                    u->setUserFlags(GGChatBot::OP_USER_FLAG);
                    GetProfile()->getSession()->sendMessage(msg);
                    qDebug() << msg;
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
                    QString msg = QString("%1 tajemniczo dostaje voice.").arg(u->getNick());
                    u->setUserFlags(GGChatBot::VOICE_USER_FLAG);
                    GetProfile()->getSession()->sendMessage(msg);
                    qDebug() << msg;
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
                    QString msg = QString("%1 tajemniczo traci przywileje.").arg(u->getNick());
                    u->setUserFlags(GGChatBot::NONE_FLAG);
                    GetProfile()->getSession()->sendMessage(msg);
                    qDebug() << msg;
            	    return;
                }
            }
        }
    }
}

// TODO: poprawic to!
void CommandResolver::moderateCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(m_channelFlags>0 || user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    m_channelFlags = GGChatBot::CHANNEL_MODERATED;
    BotSettingsTO bs = GetProfile()->getBotSettings();
    bs.setChannelFlags(m_channelFlags);
    GetProfile()->setBotSettings(bs);

    QString msg = QString("CZAT MODEROWANY !! Tylko osoby z voice/op moga rozmawiac.");
    GetProfile()->getSession()->sendMessage(msg);
}

void CommandResolver::unmoderateCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(m_channelFlags==0 || user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    m_channelFlags = GGChatBot::NONE_FLAG;
    BotSettingsTO bs = GetProfile()->getBotSettings();
    bs.setChannelFlags(m_channelFlags);
    GetProfile()->setBotSettings(bs);

    QString msg = QString("Moderacja zostala wylaczona !!");
    GetProfile()->getSession()->sendMessage(msg);
}

