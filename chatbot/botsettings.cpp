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

#include "botsettings.h"

BotSettings::BotSettings()
{
    settings = SettingsPtr(new QSettings("ggchatbot.ini", QSettings::IniFormat));
    ReadBotSettings();
    SaveBotSettings();
}

BotSettings::~BotSettings()
{
    SaveSettings();
}

void BotSettings::ReadBotSettings()
{
    qDebug() << "ReadSettings() called";

    settings->beginGroup("Main");
        m_botSettings.setDebugLevel(settings->value("DebugLevel", 0).toInt());
        m_botSettings.getUin(settings->value("UIN", 1234).toUInt());
        m_botSettings.getPassword(settings->value("Password", "pass").toString());
        m_botSettings.getDefaultDescription(settings->value("DefaultDescription", "v0.3").toString());
    settings->endGroup();
}

void BotSettings::SaveBotSettings()
{
    qDebug() << "SaveSettings() called";

    settings->beginGroup("Main");
        settings->setValue("DebugLevel", m_botSettings.getDebugLevel());
        settings->setValue("UIN", m_botSettings.getUin());
        settings->setValue("Password", m_botSettings.getPassword());
        settings->setValue("DefaultDescription", m_botSettings.getDefaultDescription());
    settings->endGroup();
}
