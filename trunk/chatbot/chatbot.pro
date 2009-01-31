QT -= gui
LIBS += -lgadu
TARGET = ggchatbot
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    	connectionthread.cpp \
	sessionclient.cpp \
	eventmanager.cpp \
	profilebase.cpp \
	profile.cpp \
	botsettings.cpp \
	botsettingsto.cpp

HEADERS += connectionthread.h \
	sessionclient.h \
	eventmanager.h \
	profilebase.h \
	profile.h \
	botsettings.cpp \
	botsettingsto.cpp
