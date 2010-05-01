/* 
 * File:   debug.h
 * Author: malcolm
 *
 * Created on 15 luty 2009, 15:05
 */

#ifndef _DEBUG_H
#define	_DEBUG_H

#include <QtDebug>
#include <QString>
#include "userinfoto.h"
#include "common.h"

void showUserDebug(UserInfoTOPtr user, QString message)
{
    if(!user)
        return;

    qDebug() << "UIN:" << user->getUin()
        << "Nick:" << user->getNick()
        << "Banned:" << user->getBanned()
        << "MSG:" << GGChatBot::unicode2latin(message);
}

#endif	/* _DEBUG_H */

