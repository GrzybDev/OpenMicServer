QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/net/handler.cpp \
    src/net/packets/client/packetclientsystem.cpp \
    src/net/server.cpp \
    src/openmic.cpp \
    src/settings.cpp \
    src/ui/WindowMain/mainwindow.cpp

HEADERS += \
    src/net/clientmessage.h \
    src/net/handler.h \
    src/net/packets/client/packetclientsystem.h \
    src/net/packets/packet.h \
    src/net/server.h \
    src/net/servermessage.h \
    src/openmic.h \
    src/settings.h \
    src/ui/WindowMain/mainwindow.h \
    src/utils.h

FORMS += \
    src/ui/WindowMain/mainwindow.ui

TRANSLATIONS += \
    src/lang/OpenMicServer_pl_PL.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = src/res/appIcon.ico
QMAKE_TARGET_COMPANY = GrzybDev
QMAKE_TARGET_DESCRIPTION = OpenMic Server
QMAKE_TARGET_COPYRIGHT = Marek Grzyb (@GrzybDev)
QMAKE_TARGET_PRODUCT = OpenMic
VERSION = 0.0.0.2
