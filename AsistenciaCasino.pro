#-------------------------------------------------
#
# Project created by QtCreator 2012-04-01T12:27:30
#
#-------------------------------------------------

QT       += core gui sql

TARGET = AsistenciaCasino
TEMPLATE = app


SOURCES += main.cpp\
        casino.cpp \
    dblocal.cpp \
    db.cpp \
    rut.cpp \
    dbsinc.cpp \
    dbremota.cpp \
    csv_lector.cpp

HEADERS  += casino.h \
    dblocal.h \
    db.h \
    rut.h \
    dbsinc.h \
    dbremota.h \
    csv_lector.h

FORMS    += casino.ui

CONFIG += static

static { # everything below takes effect with CONFIG += static

    CONFIG += static
    CONFIG += staticlib # this is needed if you create a static library, not a static executable
    DEFINES += STATIC
    message("~~~ static build ~~~") # this is for information, that the static build is done
    win32: TARGET = $$join(TARGET,,,s) #this adds an s in the end, so you can seperate static build from non static build
}

OTHER_FILES += \
    DB/db_remota_schema.sql \
    DB/db_local_schema.sql \
    DB/db_local.db
