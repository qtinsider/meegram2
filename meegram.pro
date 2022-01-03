TEMPLATE = app

TARGET = meegram

QT += declarative

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = /opt/QtSDK/Simulator/Qt/gcc/imports

CONFIG += meegotouch link_pkgconfig

PKGCONFIG += tdjson openssl zlib # libzstd rlottie tgvoip

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

CONFIG += mobility
MOBILITY += systeminfo

INCLUDEPATH += src

# Submodules
include(lib/libs.pri)
include(lib/warnings.pri)

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    src/ChatModel.cpp \
    src/ImageProviders.cpp \
    src/Stores.cpp \
    src/main.cpp \
    src/MessageModel.cpp \
    src/SelectionModel.cpp \
    src/TdApi.cpp \
    src/Utils.cpp

HEADERS += \
    src/ChatModel.hpp \
    src/Common.hpp \
    src/ImageProviders.hpp \
    src/MessageModel.hpp \
    src/SelectionModel.hpp \
    src/Serialize.hpp \
    src/Stores.hpp \
    src/TdApi.hpp \
    src/Utils.hpp

QMAKE_CXXFLAGS += -O2 -std=c++2a
QMAKE_LFLAGS   += -rdynamic

contains(MEEGO_EDITION, harmattan) {
    QMAKE_CC = /opt/strawberry-gcc-11.2/bin/arm-none-linux-gnueabi-gcc
    QMAKE_CXX = /opt/strawberry-gcc-11.2/bin/arm-none-linux-gnueabi-g++
    QMAKE_LINK = /opt/strawberry-gcc-11.2/bin/arm-none-linux-gnueabi-g++

    QMAKE_LFLAGS += -latomic -Wl,-rpath,/opt/meegram/lib -Wl,--hash-style=gnu -Wl,--as-needed -Wl,--dynamic-linker=/lib/ld-linux.so.3

    desktop.files = resources/meegram_harmattan.desktop
    desktop.path = /usr/share/applications

    icon.files = resources/meegram80.png
    icon.path = /usr/share/icons/hicolor/80x80/apps

    libs.files = libssl.so.1.1 libcrypto.so.1.1 libtdjson.so.1.8.0 libz.so.1 libstdc++.so.6 libatomic.so.1
    libs.path = /opt/meegram/lib

    splash.files = resources/meegram-splash.png
    splash.path = /opt/meegram/share

    target.path = /opt/meegram/bin

    INSTALLS += desktop icon libs splash target
}

RESOURCES += \
    resources/resources.qrc

DISTFILES += \
    debian/README \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/meegram.aegis \
    debian/rules \
    README.md

TRANSLATIONS += \
    resources/i18n/meegram_en.ts

# Rule for regenerating .qm files for translations (missing in qmake default ruleset, ugh!)
translations.input = TRANSLATIONS
translations.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
translations.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
translations.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += translations
PRE_TARGETDEPS += compiler_translations_make_all
