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

#include <QCoreApplication>
#include "botsettings.h"

namespace
{
    const QString BOT_CONFIG = "ggchatbot.ini";
}

BotSettings::BotSettings()
{
    settings = SettingsPtr(new QSettings(QCoreApplication::applicationDirPath() + "/" + BOT_CONFIG, QSettings::IniFormat));
    ReadBotSettings();
    SaveBotSettings();
}

BotSettings::~BotSettings()
{
    SaveBotSettings();
}

void BotSettings::ReadBotSettings()
{
    qDebug() << "ReadSettings() called";

    settings->beginGroup("Main");
        m_botSettings.setDebugLevel(settings->value("DebugLevel", 0).toInt());
        m_botSettings.setUin(settings->value("UIN", 0).toUInt());
        m_botSettings.setPassword(settings->value("Password", "pass").toString());
        m_botSettings.setDefaultDescription(settings->value("DefaultDescription", "v0.3").toString());
        m_botSettings.setChannelModerated(settings->value("ChannelModerated", false).toBool());
        m_botSettings.setChannelClosed(settings->value("ChannelClosed", false).toBool());
        m_botSettings.setWhoDescription(settings->value("WhoDescription", "").toString());
        m_botSettings.setBlockTopic(settings->value("BlockTopic", false).toBool());
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
        settings->setValue("ChannelModerated", m_botSettings.getChannelModerated());
        settings->setValue("ChannelClosed", m_botSettings.getChannelClosed());
        settings->setValue("WhoDescription", m_botSettings.getWhoDescription());
        settings->setValue("BlockTopic", m_botSettings.getBlockTopic());
    settings->endGroup();
}
