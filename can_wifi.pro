TARGET = wifi_can
TEMPLATE = app

QT += quick core gui network
CONFIG += c++17 release

SOURCES += main.cpp \
           canhandler.cpp \
           wifihandler.cpp 

HEADERS += canhandler.h \
	   wifihandler.h

RESOURCES += qml.qrc
