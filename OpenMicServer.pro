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
    src/net/packets/stream.cpp \
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
    src/ui/WindowAbout/aboutwindow.cpp \
    src/ui/WindowMain/mainwindow.cpp \
    src/utils.cpp

HEADERS += \
    src/audio.hpp \
    src/listener/bluetoothlistener.hpp \
    src/listener/listener.hpp \
    src/listener/usblistener.hpp \
    src/listener/wifilistener.hpp \
    src/net/errorcode.hpp \
    src/net/handler.hpp \
    src/net/message.hpp \
    src/net/packet.hpp \
    src/net/packets/auth.hpp \
    src/net/packets/stream.hpp \
    src/net/packets/system.hpp \
    src/net/server.hpp \
    src/openmic.hpp \
    src/settings.hpp \
    src/ui/DialogDeviceAuth/deviceauthdialog.hpp \
    src/ui/DialogDevicePick/devicepickdialog.hpp \
    src/ui/SettingsAudio/settingsaudio.hpp \
    src/ui/SettingsDevices/settingsdevices.hpp \
    src/ui/SettingsNetwork/settingsnetwork.hpp \
    src/ui/SettingsSystem/settingssystem.hpp \
    src/ui/WindowAbout/aboutwindow.hpp \
    src/ui/WindowMain/mainwindow.hpp \
    src/utils.hpp

FORMS += \
    src/ui/DialogDeviceAuth/deviceauthdialog.ui \
    src/ui/DialogDevicePick/devicepickdialog.ui \
    src/ui/SettingsAudio/settingsaudio.ui \
    src/ui/SettingsDevices/settingsdevices.ui \
    src/ui/SettingsNetwork/settingsnetwork.ui \
    src/ui/SettingsSystem/settingssystem.ui \
    src/ui/WindowAbout/aboutwindow.ui \
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
VERSION = 0.4.0.0

win32:LIBS += -lBthprops
