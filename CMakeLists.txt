cmake_minimum_required(VERSION 3.20)
project(MeeGram VERSION 0.1.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Td REQUIRED)
find_package(rlottie REQUIRED)
find_package(Qt4 4.7.4 COMPONENTS QtCore QtDBus QtGui QtSvg QtXml QtDeclarative REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(boostable QUIET qdeclarative-boostable)

set(QML_IMPORT_PATH "/home/insider/QtSDK/Simulator/Qt/gcc/imports" CACHE STRING "")

set(src_files
    src/Application.cpp
    src/Authorization.cpp
    src/BasicGroup.cpp
    src/BasicGroupFullInfo.cpp
    src/Chat.cpp
    src/ChatModel.cpp
    src/Client.cpp
    src/DBusAdaptor.cpp
    src/Emoji.cpp
    src/Emoji_data.cpp
    src/File.cpp
    src/ImageProviders.cpp
    src/Localization.cpp
    src/LottieAnimation.cpp
    src/main.cpp
    src/Message.cpp
    src/MessageModel.cpp
    src/NotificationManager.cpp
    src/SelectionModel.cpp
    src/Settings.cpp
    src/SignalConnector.cpp
    src/SortFilterProxyModel.cpp
    src/StorageManager.cpp
    src/SupergroupFullInfo.cpp
    src/Supergroup.cpp
    src/TextFormatter.cpp
    src/User.cpp
    src/UserFullInfo.cpp
    src/Utils.cpp
)

set(header_files
    src/Application.hpp
    src/Authorization.hpp
    src/BasicGroup.hpp
    src/BasicGroupFullInfo.hpp
    src/Chat.hpp
    src/ChatModel.hpp
    src/Client.hpp
    src/Common.hpp
    src/DBusAdaptor.hpp
    src/Emoji.hpp
    src/File.hpp
    src/ImageProviders.hpp
    src/Localization.hpp
    src/LottieAnimation.hpp
    src/Message.hpp
    src/MessageModel.hpp
    src/NotificationManager.hpp
    src/SelectionModel.hpp
    src/Serialize.hpp
    src/Settings.hpp
    src/SignalConnector.hpp
    src/SortFilterProxyModel.hpp
    src/StorageManager.hpp
    src/Supergroup.hpp
    src/SupergroupFullInfo.hpp
    src/TdApi.hpp
    src/TextFormatter.hpp
    src/User.hpp
    src/UserFullInfo.hpp
    src/Utils.hpp
)

set(qrc_files resources/resources.qrc)

add_executable(meegram ${src_files} ${header_files} ${qrc_files})

set_target_properties(meegram PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_options(meegram PRIVATE -Wall -Wextra -pedantic $<IF:$<CONFIG:Debug>, -Werror, -Wno-psabi>)

target_link_libraries(meegram PRIVATE rlottie::rlottie Td::TdJsonStatic Qt4::QtCore Qt4::QtDBus Qt4::QtGui Qt4::QtSvg Qt4::QtXml Qt4::QtDeclarative Threads::Threads)

if (boostable_FOUND)
    target_compile_options(meegram PRIVATE ${boostable_CFLAGS})
    target_include_directories(meegram PRIVATE ${boostable_INCLUDE_DIRS})

    target_link_options(meegram PRIVATE -static-libstdc++ -Wl,--dynamic-linker=/lib/ld-linux.so.3 -pie -rdynamic)

    target_link_libraries(meegram PRIVATE ${boostable_LIBRARIES})
endif()
