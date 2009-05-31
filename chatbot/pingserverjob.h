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

#ifndef PINGSERVERJOB_H
#define PINGSERVERJOB_H

#include <libgadu.h>
#include "job.h"

class PingServerJob : public Job
{
    Q_OBJECT
public:
    PingServerJob(gg_session *s);
    virtual ~PingServerJob();
    void makeJob();

private:
    gg_session *session;
};

#endif // PINGSERVERJOB_H
