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
#include "imagedescriptionsettings.h"
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
    const QString CMD_USERS             = "/users";
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
    const QString CMD_CLOSED            = "/closed";
    const QString CMD_IMGSTATUSLIST	= "/imgstatuslist";
    const QString CMD_SETIMGSTATUS      = "/setimgstatus";
    const QString CMD_PRIV              = "/priv";
    const QString CMD_KICKALL           = "/kickall";
    const QString CMD_TOTALKICK         = "/totalkick";
    const QString CMD_STATS             = "/stats";
    const QString CMD_SETNICK           = "/setnick";
    const QString CMD_SEEN              = "/seen";
    const QString CMD_BLOCKTOPIC        = "/blocktopic";

    const QString MSG_NICK_EXIST        = "Uzytkownik o takim nicku juz istnieje!";
    const QString MSG_HELP              = "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/start - wejscie na czat\n/koniec 'tekst'- opuszczenie czatu, opcjonalnie z tekstem\n" \
        "/kto - spis osob dostepnych na czacie\n/priv Nick 'tekst' - wysyla prywatna wiadomosc\n" \
        "/stats - statystyki czata\n" \
        "/pomoc - pomoc ktora wlasnie czytasz ;)\n" \
        "/seen 'nick' - data, gdy osoba byla ostatnio widziana na czacie";

    const QString MSG_HELP_FOR_OPS      = "Dostepne komendy:\n/nick 'Nick' - zmiana nicka\n" \
        "/join /start - wejscie na czat\n/leave /stop /quit /koniec 'tekst'- opuszczenie czatu\n" \
        "/who /kto - spis osob dostepnych na czacie\n/priv Nick 'tekst' - wysyla prywatna wiadomosc\n" \
        "/stats - statystyki czata\n" \
        "/help /pomoc - pomoc\n/ban /unban nick/numer czas opis - banowanie" \
        ", czas w minutach, 0=rok\n/kick nick/numer opis - wywalenie z czatu\n/op numer - ustawia flage op'a\n" \
        "/voice numer - ustawia flage voice\n/removeflags numer - zdejmuje wszystkie flagi\n" \
        "/moderate /unmoderate - ustawia/zdejmuje czat moderowany\n/imgstatuslist - pokazuje dostepne statusy graficzne\n" \
        "/setimgstatus numer - ustawia status graficzny o podanym numerze" \
        "/seen 'nick' - data, gdy osoba byla ostatnio widziana na czacie";
}

CommandResolver::CommandResolver()
    : mLastUserJoin(QDateTime::currentDateTime())
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

    BotSettingsTO bs = GetProfile()->getBotSettings();
    mChannelModerated = bs.getChannelModerated();
    mChannelClosed = bs.getChannelClosed();
    m_topicIsBlocked = bs.getBlockTopic();

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
        else if(command.compare(CMD_USERS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_USERS);
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
        else if(command.compare(CMD_CLOSED, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_CLOSED);
            closedCommand();
            return true;
        }
        else if(command.compare(CMD_IMGSTATUSLIST, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_IMGSTATUSLIST);
            imgStatusList();
            return true;
        }
        else if(command.compare(CMD_SETIMGSTATUS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_SETIMGSTATUS);
            setImgStatus();
            return true;
        }
        else if(command.compare(CMD_PRIV, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_PRIV);
            privCommand();
            return true;
        }
        else if(command.compare(CMD_KICKALL, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_KICKALL);
            kickAllCommand();
            return true;
        }
        else if(command.compare(CMD_TOTALKICK, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_TOTALKICK);
            totalKickCommand();
            return true;
        }
        else if(command.compare(CMD_STATS, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_STATS);
            statsCommand();
            return true;
        }
        else if(command.compare(CMD_SETNICK, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_SETNICK);
            setNickCommand();
            return true;
        }
        else if(command.compare(CMD_SEEN, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_SEEN);
            seenCommand();
            return true;
        }
        else if(command.compare(CMD_BLOCKTOPIC, Qt::CaseInsensitive)==0)
        {
            lastString = removeCommand(str, CMD_BLOCKTOPIC);
            blockTopicCommand();
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

void CommandResolver::imgStatusList()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);    
    
    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    ImageDescriptionSettings imageDescSettings;
    QList<ImageDescription> idescList;
    imageDescSettings.readImageDescSettings(idescList);

    QString msg = "Dostepne statusy graficzne:\n";
	
    if(idescList.isEmpty())
        msg += QString("Brak dostepnych statusow graficznych.");
	
    int i = 0;
    foreach(ImageDescription imgDesc, idescList)
    {
        msg += QString("%1. %2\n").arg(i).arg(imgDesc.userbarId);
        i++;
    }
	
    GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
}

void CommandResolver::setImgStatus()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    ImageDescriptionSettings imageDescSettings;
    QList<ImageDescription> idescList;
    imageDescSettings.readImageDescSettings(idescList);

    QRegExp rx("^(\\d+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        bool bOk = true;
        int itemPos = rx.cap(1).toInt(&bOk);
        if(!bOk)
            return;

        if(itemPos > idescList.size())
            return;

        if(QDateTime::currentDateTime() > idescList.at(itemPos).expireTime)
            return;

        GetProfile()->getSession()->SetImageStatus(idescList.at(itemPos).userbarId);
    }
}

void CommandResolver::privCommand()
{
    if(lastString.isEmpty())
        return;

    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(!user->getOnChannel())
        return;

    if(GetProfile()->messageIsSpam(user, lastString))
        return;

    QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString newNick = rx.cap(1);
        lastString = removeCommand(rx.cap(0), newNick);  
        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            if(u->getNick() == newNick)
            {
                GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);

                if(!u->getOnChannel())
                {
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Uzytkownika %1 nie ma na czacie.").arg(u->getNick()));
                    return;
                }
                if(lastString.isEmpty())
                {
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Poprawna komenda to: /priv Nick tekst"));
                    return;
                }

                GetProfile()->getSession()->sendMessageTo(u->getUin(), QString("* (priv) * %1 %2").arg(userNick.nick).arg(lastString));
                return;
            }
        }

        GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Nie ma uzytkownika o nicku %1.").arg(newNick));
        return;
    }

    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Poprawna komenda to: /priv Nick tekst"));
}

void CommandResolver::nickCommand()
{
    if(lastString.isEmpty() || mChannelModerated)
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
            if(u->getNick().compare(newNick, Qt::CaseInsensitive)==0)
            {
                if(u->getUin() == user->getUin())
                {
		    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Hej %1, juz masz ustawiony nick!").arg(userNick.nick));
                    return;
                }
                qDebug() << u->getUin() << user->getUin();
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

void CommandResolver::closedCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
        return;

    mChannelClosed = !mChannelClosed;
    BotSettingsTO bs = GetProfile()->getBotSettings();
    bs.setChannelClosed(mChannelClosed);
    GetProfile()->setBotSettings(bs);

    QString msg;

    if(mChannelClosed)
    {
        msg = "Czat zostal zamkniety dla nieznajomych!";
    }
    else
    {
        msg = "Czat zostal otwarty dla wszystkich.";
    }

    GetProfile()->getSession()->sendMessage(msg);
}

bool CommandResolver::checkIfUserCanJoin()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    QString msg = QString("Limit osob na czacie zostal przekroczony. Sproboj ponownie za chwile");

    // limit na 2 ktosiow na czacie, nowe nie moga wchodzic
    if(user->getNick().startsWith("Ktos", Qt::CaseInsensitive))
    {
        int ktosCount = 0;
        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {   
            if(GetProfile()->getUserDatabase()->isUserOnChannel(u) && u->getNick().startsWith("Ktos", Qt::CaseInsensitive))
            {
                ktosCount++;
            }
        }

        if(ktosCount >= 2)
        {
            GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
            return false;
        }
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();
    currentDateTime = currentDateTime.addSecs(-15); // przez 15 sekund uzytkownicy bez flag nie moga wejsc na czat
    if(user->getUserFlags() < GGChatBot::VOICE_USER_FLAG)
    {
        if(mLastUserJoin > currentDateTime)
        {
            GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
            return false;
        }
    }

    if(mChannelClosed && user->getUin() >= 38000000)
    {
        qDebug() << "UIN:" << user->getUin() << "User blocked!!!" << msg;
        return false;
    }

    if(!mChannelClosed)
        return true;

    if(user->getUserFlags() >= GGChatBot::VOICE_USER_FLAG)
        return true;

    if(user->getNick().isEmpty() || user->getNick().startsWith("Ktos", Qt::CaseInsensitive))
        return false;

    QDateTime lastSeen = user->getLastSeen();
    QDateTime currentTime = GGChatBot::getDateTime();
    currentTime.addSecs(-3600); // 3600 - godzina
    if(lastSeen > currentTime)
        return false;

    return true;
}

void CommandResolver::joinCommand()
{
    setTopic(m_topic, false);
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);
    if(user->getOnChannel())
        return;

    if(user->getNick().isEmpty())
	user->setNick(QString("Ktos%1").arg(user->getUin()));
    
    if(!checkIfUserCanJoin())
        return;

    if(user->getBanned())
    {
        QDateTime currentTime = GGChatBot::getDateTime();
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
    
    if(!mChannelModerated)
        GetProfile()->getSession()->sendMessage(msg);

    mLastUserJoin = QDateTime::currentDateTime();
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

    if(GetProfile()->messageIsSpam(user, reason))
        reason.clear();

    bool isBad = false;
    reason = GetProfile()->replaceBadWords(reason, isBad);

    GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
    QString msg = "Odchodzi " + userNick.nick + " " + reason;
    qDebug() << "UIN:" << user->getUin() << msg;

    if(!mChannelModerated)
        GetProfile()->getSession()->sendMessage(msg);

    user->setOnChannel(false);
    GetProfile()->getUserDatabase()->saveDatabase();

    if(user->getNick().startsWith("Ktos", Qt::CaseInsensitive))
        user->setNick(QString());
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

    if(mChannelModerated)
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
    UserInfoTOPtr sender = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(sender->getUserFlags() < GGChatBot::OP_USER_FLAG)
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
                    GetProfile()->kickHelperCommand(u, sender, lastString);
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
                    GetProfile()->kickHelperCommand(u, sender, lastString);
                    return;
                }
            }
        }
    }


    void CommandResolver::kickHelperCommand(UserInfoTOPtr u, UserInfoTOPtr sender)
    {
        UserInfoTOPtr user;
        if(u->getUserFlags() > GGChatBot::OP_USER_FLAG)
            user = sender;
        else
            user = u;

        GGChatBot::UserNick userNick = GetProfile()->getUserDatabase()->makeUserNick(user);
        QString msg = QString("%1 wylatuje z czatu. %2").arg(userNick.nick).arg(lastString);
        if(user->getOnChannel())
            GetProfile()->getSession()->sendMessage(msg);
        user->setOnChannel(false);
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
                    banHelperCommand(u, minutes, description, user);
                    break;
                }
            }
            else
            {
                QString nickStr = u->getNick();
                if(nickStr == nick)
                {
                    banHelperCommand(u, minutes, description, user);
                    break;
                }
            }
        }
    }

    void CommandResolver::banHelperCommand(UserInfoTOPtr u, uint banTime, QString description, UserInfoTOPtr sender)
    {
        UserInfoTOPtr user;
        if(u->getUserFlags() > GGChatBot::SUPER_USER_FLAG)
            user = sender;
        else
            user = u;

        QString message;
        QString nickName = user->getNick();
        
        if(nickName.isEmpty())
            nickName = QString("%1").arg(user->getUin());

        QDateTime currentDate = GGChatBot::getDateTime();
        if(banTime == 0)
            currentDate = currentDate.addYears(1);        // jesli banTime==0 to dajemy bana na rok
        else
            currentDate = currentDate.addSecs(banTime*60);
        QString banEndTimeDesc = currentDate.toString();//"dddd d-M-yyyy h:m:s

        message = QString("%1 banuje %2").arg(sender->getNick()).arg(user->getUin());
        GetProfile()->getSession()->sendMessageToStaff(message);

        if(description.isEmpty())
            message = QString("%1 dostaje bana do %2.").arg(nickName).arg(banEndTimeDesc);
        else
            message = QString("%1 dostaje bana do %2. Powod: %3").arg(nickName).arg(banEndTimeDesc).arg(description);

        GetProfile()->getSession()->sendMessage(message);

        lastString = "";
        GetProfile()->kickHelperCommand(user, sender, lastString);
        user->setBanned(true);
        user->setBanReason(description);
        user->setNick(QString());
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
            qDebug() << "Nie znaleziono uzytkownika.";
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
            qDebug() << "Nie znaleziono uzytkownika";
        }
    }

    void CommandResolver::unbanHelperCommand(UserInfoTOPtr u)
    {
        if(!u->isBanned())
        {
            qDebug() << "Uzytkownik nie ma bana";
            return;
        }

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

        if(m_topicIsBlocked)
        {
            GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Zostala wlaczona blokada zmiany opisu, nie mozesz go zmienic."));
            return;
        }

        QRegExp rx("^(.*)");
        int pos = 0;

        if((pos = rx.indexIn(lastString, pos)) != -1)
        {
            m_topic = rx.cap(1);
            setTopic(m_topic);
        }
    }

    void CommandResolver::blockTopicCommand()
    {
        UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(user->getUserFlags() < GGChatBot::OP_USER_FLAG)
            return;

        QString message;

        if(m_topicIsBlocked)
        {
            m_topicIsBlocked = false;

            message = QString("Zmiana tematu zostala odblokowana.");
            GetProfile()->getSession()->sendMessageToStaff(message);
        }        
        else
        {
            message = QString("Zmiana tematu zostala zablokowana przez szefunia.");
            GetProfile()->getSession()->sendMessageToStaff(message);

            m_topicIsBlocked = true;
        }

        BotSettingsTO bs = GetProfile()->getBotSettings();
        bs.setBlockTopic(m_topicIsBlocked);
        GetProfile()->setBotSettings(bs);
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
                        QString msg = QString("Na przekor ewolucji %1 cofa sie do poziomu malpy.").arg(u->getNick());
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
                        GetProfile()->getSession()->sendMessage(msg);
                        qDebug() << msg;
                        if(u->getUserFlags() >= GGChatBot::OP_USER_FLAG)
                            return;
                        u->setUserFlags(GGChatBot::VOICE_USER_FLAG);
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

        if(mChannelModerated || user->getUserFlags() < GGChatBot::OP_USER_FLAG)
            return;

        mChannelModerated = true;
        BotSettingsTO bs = GetProfile()->getBotSettings();
        bs.setChannelModerated(mChannelModerated);
        GetProfile()->setBotSettings(bs);

        QString msg = QString("CZAT MODEROWANY !! Tylko osoby z voice/op moga rozmawiac.");
        qDebug() << msg;
        GetProfile()->getSession()->sendMessage(msg);
    }

    void CommandResolver::unmoderateCommand()
    {
        UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(!mChannelModerated || user->getUserFlags() < GGChatBot::OP_USER_FLAG)
            return;

        mChannelModerated = false;
        BotSettingsTO bs = GetProfile()->getBotSettings();
        bs.setChannelModerated(mChannelModerated);
        GetProfile()->setBotSettings(bs);

        QString msg = QString("Moderacja zostala wylaczona !!");
        qDebug() << msg;
        GetProfile()->getSession()->sendMessage(msg);
    }

    void CommandResolver::kickAllCommand()
    {
        UserInfoTOPtr sender = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(sender->getUserFlags() < GGChatBot::OP_USER_FLAG)
            return;

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr user, users)
        {
            if(user->getOnChannel() && user->getUserFlags() < GGChatBot::VOICE_USER_FLAG)
                GetProfile()->kickHelperCommand(user, sender, lastString);
        }
    }

    void CommandResolver::totalKickCommand()
    {
        UserInfoTOPtr sender = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(sender->getUserFlags() < GGChatBot::SUPER_USER_FLAG)
            return;

        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr user, users)
        {
            if(user->getOnChannel() && user->getUserFlags() <= GGChatBot::OP_USER_FLAG)
                GetProfile()->kickHelperCommand(user, sender, lastString);
        }
    }

    void CommandResolver::statsCommand()
    {
        UserInfoTOPtr sender = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(!sender->getOnChannel())
            return;

        const uint64_t MINUTE = 60;
        const uint64_t HOUR   = MINUTE * 60;
        const uint64_t DAY    = HOUR * 24;
        const uint64_t MONTH  = DAY * 30;
        const uint64_t YEAR   = MONTH * 12;
        uint64_t years = 0;
        uint64_t months = 0;
        uint64_t days = 0;
        uint64_t hours = 0;
        uint64_t minutes = 0;
        uint64_t seconds = 0;
        int64_t secs = 0;

        if(mStatsPtr.isNull())
        {
            mStatsPtr = new Stats;
        }

        mStatsPtr->init();

            QDateTime currentTime = GGChatBot::getDateTime();
            QDateTime last24h = GGChatBot::getDateTime();
            last24h = last24h.addSecs(-86400);
            QDateTime lastWeek = GGChatBot::getDateTime();
            lastWeek = lastWeek.addSecs(-604800);
            QDateTime lastMonth = GGChatBot::getDateTime();
            lastMonth = lastMonth.addSecs(-2592000);

            QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
            mStatsPtr->totalUsers = users.size();

            foreach(UserInfoTOPtr u, users)
            {
                if(u->getUserFlags() > GGChatBot::OP_USER_FLAG)
                    mStatsPtr->adminUsers++;
                if(u->getUserFlags() == GGChatBot::OP_USER_FLAG)
                    mStatsPtr->opUsers++;
                if(u->getUserFlags() == GGChatBot::VOICE_USER_FLAG)
                    mStatsPtr->voiceUsers++;

                if(u->getNick().startsWith("Ktos", Qt::CaseInsensitive))
                    mStatsPtr->ktosUsers++;

                QDateTime lastSeen = u->getLastSeen();
                if(lastSeen > last24h)
                    mStatsPtr->last24h++;

                if(lastSeen > lastWeek)
                    mStatsPtr->lastWeek++;

                if(lastSeen > lastMonth)
                    mStatsPtr->lastMonth++;

                if(u->getBanned())
                {
                    QDateTime banTime = u->getBanTime();
                    secs = currentTime.secsTo(banTime);

                    // ban already ends, so clear it and don't count
                    if(secs <= 0)
                    {
                        u->setBanned(false);
                        continue;
                    }
                    mStatsPtr->banUsers++;
                    mStatsPtr->totalBansTime += secs;
                }
            }

        seconds = mStatsPtr->totalBansTime;

        if(years = (seconds/YEAR))
            seconds -= YEAR * years;
        if(months = (seconds/MONTH))
            seconds -= MONTH * months;
        if(days = (seconds/DAY))
            seconds -= DAY * days;
        if(hours = (seconds/HOUR))
            seconds -= HOUR * hours;
        if(minutes = (seconds/MINUTE))
            seconds -= MINUTE * minutes;
        
        QString bansTimeMsg = QString("%1 lat %2 miesiecy %3 dni %4 godzin %5 minut %6 sekund").arg(years).arg(months).arg(days).arg(hours).arg(minutes).arg(seconds);
        QString userInfo = QString("Wszystkich uzytkownikow: %1 w tym:\nAdminow: %2\nOpow: %3\nVoiceow: %4\nUzytkownikow bez ustawionego nicka: %5")
            .arg(mStatsPtr->totalUsers).arg(mStatsPtr->adminUsers).arg(mStatsPtr->opUsers).arg(mStatsPtr->voiceUsers).arg(mStatsPtr->ktosUsers);
        QString msg = QString("%1\nUzytkownikow w ostatnich:\n24 godzinach: %2\ntygodniu: %3\nmiesiacu: %4\n"
            "Uzytkownikow z banem: %5\nLaczny czas wszystkich banow: %6\n").arg(userInfo).arg(mStatsPtr->last24h).arg(mStatsPtr->lastWeek).arg(mStatsPtr->lastMonth).arg(mStatsPtr->banUsers).arg(bansTimeMsg);

        GetProfile()->getSession()->sendMessageTo(sender->getUin(), msg);
    }

    void CommandResolver::setNickCommand()
    {
        UserInfoTOPtr sender = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

        if(sender->getUserFlags() < GGChatBot::OP_USER_FLAG)
            return;

        QRegExp rx2("^(\\d+).*");
        int pos = 0;

        if((pos = rx2.indexIn(lastString, pos)) != -1)
        {
            QString uinTo = rx2.cap(1);
            lastString = removeCommand(rx2.cap(0), uinTo);

            QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
            foreach(UserInfoTOPtr u, users)
            {
                QString uin = QString("%1").arg(u->getUin());
                if(uin == uinTo)
                {
                    u->setNick(lastString);
                    return;
                }
            }
        }
    }

void CommandResolver::seenCommand()
{
    UserInfoTOPtr user = GetProfile()->getUserDatabase()->getUserInfo(m_event->event.msg.sender);

    if(lastString.isEmpty())
    {
        GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Poprawna komenda to: /seen Nick"));
        return;
    }

    if(!user->getOnChannel())
        return;

    QRegExp rx("^(\\w+).*");
    int pos = 0;

    if((pos = rx.indexIn(lastString, pos)) != -1)
    {
        QString newNick = rx.cap(1);
        lastString = removeCommand(rx.cap(0), newNick);
        QList<UserInfoTOPtr> users = GetProfile()->getUserDatabase()->getUserList();
        foreach(UserInfoTOPtr u, users)
        {
            if(u->getNick() == newNick)
            {
                if(u->getOnChannel())
                {
                    QString msg = QString("Uzytkownik %1 jest w tej chwili na czacie.").arg(u->getNick());
                    GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
                    return;
                }

                QDateTime lastSeen = u->getLastSeen();
                QString msg = QString("Uzytkownik %1 ostatnio byl na czacie o %2").arg(newNick).arg(lastSeen.toString("hh:mm:ss dd/MM/yyyy"));
                GetProfile()->getSession()->sendMessageTo(user->getUin(), msg);
                return;
            }
        }

        GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Nie ma uzytkownika o nicku %1.").arg(newNick));
        return;
    }

    GetProfile()->getSession()->sendMessageTo(user->getUin(), QString("Poprawna komenda to: /seen Nick"));

}

