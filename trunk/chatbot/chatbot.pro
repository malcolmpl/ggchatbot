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
    botsettingsto.cpp \
    userdatabase.cpp \
    userinfoto.cpp \
    logscheduler.cpp \
    sessionscheduler.cpp \
    commandresolver.cpp \
    job.cpp \
    pingserverjob.cpp
HEADERS += connectionthread.h \
    sessionclient.h \
    eventmanager.h \
    profilebase.h \
    profile.h \
    botsettings.h \
    botsettingsto.h \
    userdatabase.h \
    userinfoto.h \
    logscheduler.h \
    sessionscheduler.h \
    commandresolver.h \
    job.h \
    pingserverjob.h
