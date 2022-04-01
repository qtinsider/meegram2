cmake_minimum_required(VERSION 3.18.0)
project(MeeGram VERSION 0.1.4 LANGUAGES CXX)

option(MEEGO_EDITION_HARMATTAN "Use MeeGo Harmattan Device." OFF)

find_package(Qt4 4.7.4 COMPONENTS QtCore QtDBus QtGui QtDeclarative REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(boostable QUIET qdeclarative-boostable)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(dependencies)

set(QML_IMPORT_PATH "/opt/QtSDK/Simulator/Qt/gcc/imports" CACHE STRING "")

set(src_files
    src/ChatModel.cpp
    src/DBusAdaptor.cpp
    src/ImageProviders.cpp
    src/Localization.cpp
    src/LottieAnimation.cpp
    src/main.cpp
    src/MessageModel.cpp
    src/NotificationManager.cpp
    src/SelectionModel.cpp
    src/Settings.cpp
#   src/SortFilterProxyModel.cpp
    src/StorageManager.cpp
    src/TdApi.cpp
    src/Utils.cpp
)

set(header_files
    src/ChatModel.hpp
    src/Common.hpp
    src/DBusAdaptor.hpp
    src/ImageProviders.hpp
    src/Localization.hpp
    src/LottieAnimation.hpp
    src/MessageModel.hpp
    src/NotificationManager.hpp
    src/SelectionModel.hpp
    src/Serialize.hpp
    src/Settings.hpp
#   src/SortFilterProxyModel.hpp
    src/StorageManager.hpp
    src/TdApi.hpp
    src/Utils.hpp
)

set(qrc_files resources/resources.qrc)

add_executable(meegram ${src_files} ${header_files} ${qrc_files})

set_target_properties(meegram PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_features(meegram PRIVATE cxx_std_20)
target_compile_options(meegram PRIVATE -Wall -Wextra -pedantic $<IF:$<CONFIG:Debug>, -Werror, -Wno-psabi>)

target_link_libraries(meegram PRIVATE external::libs Qt4::QtCore Qt4::QtDBus Qt4::QtGui Qt4::QtDeclarative Threads::Threads)

if (boostable_FOUND)
    target_compile_options(meegram PRIVATE ${boostable_CFLAGS})
    target_compile_definitions(meegram PRIVATE MEEGO_EDITION_HARMATTAN)
    target_include_directories(meegram PRIVATE ${boostable_INCLUDE_DIRS})

    target_link_options(meegram PRIVATE
        -static-libstdc++
        -Wl,--dynamic-linker=/lib/ld-linux.so.3
        -pie
        -rdynamic
        )

    target_link_libraries(meegram PRIVATE ${boostable_LIBRARIES})
endif()
