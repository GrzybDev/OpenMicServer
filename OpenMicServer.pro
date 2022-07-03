QT       += core gui websockets multimedia bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/audio.cpp \
    src/listener/bluetoothlistener.cpp \
    src/listener/listener.cpp \
    src/listener/usblistener.cpp \
    src/listener/wifilistener.cpp \
    src/main.cpp \
    src/net/handler.cpp \
    src/net/packet.cpp \
    src/net/packets/auth.cpp \
    src/net/packets/system.cpp \
    src/net/server.cpp \
    src/openmic.cpp \
    src/settings.cpp \
    src/ui/DialogDeviceAuth/deviceauthdialog.cpp \
    src/ui/DialogDevicePick/devicepickdialog.cpp \
    src/ui/SettingsAudio/settingsaudio.cpp \
    src/ui/SettingsDevices/settingsdevices.cpp \
    src/ui/SettingsNetwork/settingsnetwork.cpp \
    src/ui/SettingsSystem/settingssystem.cpp \
    src/ui/WindowMain/mainwindow.cpp \
    src/utils.cpp

HEADERS += \
    src/audio.h \
    src/listener/bluetoothlistener.h \
    src/listener/listener.h \
    src/listener/usblistener.h \
    src/listener/wifilistener.h \
    src/net/exitcode.h \
    src/net/handler.h \
    src/net/message.h \
    src/net/packets/auth.h \
    src/net/packets/system.h \
    src/net/packet.h \
    src/net/server.h \
    src/openmic.h \
    src/settings.h \
    src/ui/DialogDeviceAuth/deviceauthdialog.h \
    src/ui/DialogDevicePick/devicepickdialog.h \
    src/ui/SettingsAudio/settingsaudio.h \
    src/ui/SettingsDevices/settingsdevices.h \
    src/ui/SettingsNetwork/settingsnetwork.h \
    src/ui/SettingsSystem/settingssystem.h \
    src/ui/WindowMain/mainwindow.h \
    src/utils.h

FORMS += \
    src/ui/DialogDeviceAuth/deviceauthdialog.ui \
    src/ui/DialogDevicePick/devicepickdialog.ui \
    src/ui/SettingsAudio/settingsaudio.ui \
    src/ui/SettingsDevices/settingsdevices.ui \
    src/ui/SettingsNetwork/settingsnetwork.ui \
    src/ui/SettingsSystem/settingssystem.ui \
    src/ui/WindowMain/mainwindow.ui

TRANSLATIONS += \
    src/lang/OpenMicServer_pl_PL.ts

RESOURCES += \
    src/res/resources.qrc

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
VERSION = 0.2.0.0

win32:LIBS += -lBthprops
