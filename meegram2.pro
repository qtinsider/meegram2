TEMPLATE = app

TARGET = meegram2

QT += declarative

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = /opt/QtSDK/Simulator/Qt/gcc/imports

CONFIG += meegotouch link_pkgconfig

PKGCONFIG += tdjson libzstd rlottie # tgvoip

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

CONFIG += mobility
MOBILITY += systeminfo

INCLUDEPATH += src/

# Submodules
include(lib/nlohmann.pri)
include(lib/warnings.pri)

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    src/ChatModel.cpp \
    src/CountryModel.cpp \
    src/ImageProviders.cpp \
    src/Lottie.cpp \
    src/main.cpp \
    src/MessageModel.cpp \
    src/Stores.cpp \
    src/TdApi.cpp \
    src/Utils.cpp

HEADERS += \
    src/ChatModel.hpp \
    src/Common.hpp \
    src/CountryModel.hpp \
    src/ImageProviders.hpp \
    src/Lottie.hpp \
    src/MessageModel.hpp \
    src/Serialize.hpp \
    src/Stores.hpp \
    src/TdApi.hpp \
    src/Utils.hpp

QMAKE_CXXFLAGS += -std=c++2a
QMAKE_LFLAGS += -O2 -rdynamic

contains(MEEGO_EDITION, harmattan) {
    QMAKE_CC = /opt/strawberry-gcc-11.1/bin/arm-none-linux-gnueabi-gcc
    QMAKE_CXX = /opt/strawberry-gcc-11.1/bin/arm-none-linux-gnueabi-g++
    QMAKE_LINK = /opt/strawberry-gcc-11.1/bin/arm-none-linux-gnueabi-g++

    QMAKE_LFLAGS += -Wl,-rpath,/opt/meegram2/lib -Wl,--hash-style=gnu -Wl,--as-needed -Wl,--dynamic-linker=/lib/ld-linux.so.3

    icon.files = resources/meegram2.png
    icon.path = /usr/share/icons/hicolor/80x80/apps

    desktopfile.files = resources/meegram2.desktop
    desktopfile.path = /usr/share/applications

    target.path = /opt/meegram2/bin

    librlottie.files = ../build/lib/librlottie.so.0
    librlottie.path = /opt/meegram2/lib

    libstdcxx.files = ../build/lib/libstdc++.so.6
    libstdcxx.path = /opt/meegram2/lib

    libtdjson.files = ../build/lib/libtdjson.so.1.7.4
    libtdjson.path = /opt/meegram2/lib

    INSTALLS += desktopfile icon target libtdjson librlottie libstdcxx
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/i18n/meegram_en.ts

# Rule for regenerating .qm files for translations (missing in qmake default ruleset, ugh!)
translations.input = TRANSLATIONS
translations.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
translations.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
translations.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += translations
PRE_TARGETDEPS += compiler_translations_make_all
