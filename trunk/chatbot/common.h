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

#ifndef _COMMON_H
#define	_COMMON_H

#include <QTextCodec>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace GGChatBot
{
    const int NONE_FLAG                 = 0;
    const int VOICE_USER_FLAG           = 1;
    const int OP_USER_FLAG              = 2;
    const int SUPER_USER_FLAG           = 4;

    const int CHANNEL_MODERATED         = 0;

    const int UTCtoPLsec                = 7200;

    QString cp2unicode(const QByteArray &buf);
    QByteArray unicode2cp(const QString &buf);
    QByteArray unicode2latin(const QString &buf);

    QString makeInternalMessage(QString buf);
    QString makeMessage(QString buf);

    QDateTime getDateTime();

    class UserNick
    {
    public:
	QString nick;
	unsigned char *format;
	int formatlen;
    };
}

#endif	/* _COMMON_H */

