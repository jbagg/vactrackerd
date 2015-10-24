QT		+= core gui widgets xml network
TARGET	= vactrackerd
HEADERS= server.h client.h region.h regionYear.h user.h deposit.h withdrawal.h emailnotify.h
HEADERS+= parse.h depositparser.h withdrawalparser.h userparser.h regionyearparser.h regionparser.h
SOURCES= main.cpp server.cpp client.cpp region.cpp regionYear.cpp user.cpp deposit.cpp withdrawal.cpp emailnotify.cpp
SOURCES+= parse.cpp depositparser.cpp withdrawalparser.cpp userparser.cpp regionyearparser.cpp regionparser.cpp
LIBS+= -lesmtp
 
include($$PWD/qtzeroconf/qtzeroconf.pri)

                 
                 
 

