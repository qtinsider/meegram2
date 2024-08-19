cmake_minimum_required(VERSION 3.20)
project(MeeGram VERSION 0.1.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Td REQUIRED)
find_package(rlottie REQUIRED)
find_package(Qt4 4.7.4 COMPONENTS QtCore QtDBus QtGui QtSvg QtXml QtDeclarative REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

option(BUILD_HARMATTAN "Build for MeeGo 1.2 Harmattan Device" OFF)

if (BUILD_HARMATTAN)
    pkg_check_modules(boostable QUIET qdeclarative-boostable)
endif()

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

target_link_libraries(meegram PRIVATE
    Td::TdJson
    rlottie::rlottie
    Qt4::QtCore
    Qt4::QtDBus
    Qt4::QtDeclarative
    Qt4::QtGui
    Qt4::QtSvg
    Qt4::QtXml
    ZLIB::ZLIB
    Threads::Threads
)

if (BUILD_HARMATTAN)
    target_compile_options(meegram PRIVATE ${boostable_CFLAGS})
    target_include_directories(meegram PRIVATE ${boostable_INCLUDE_DIRS})

    target_link_options(meegram PRIVATE -Wl,-rpath,/opt/meegram/lib -Wl,--dynamic-linker=/lib/ld-linux.so.3 -pie -rdynamic)

    target_link_libraries(meegram PRIVATE ${boostable_LIBRARIES})

   if(NOT EXISTS "${CMAKE_BINARY_DIR}/debian")
        file(COPY ${CMAKE_SOURCE_DIR}/debian DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    install(FILES resources/meegram_harmattan.desktop DESTINATION /usr/share/applications)
    install(FILES resources/meegram80.png DESTINATION /usr/share/icons/hicolor/80x80/apps)
    install(FILES resources/meegram-splash.png DESTINATION /opt/meegram/share)

    install(FILES
        lib/libcrypto.so.1.1
        lib/libssl.so.1.1
        lib/libtdjson.so.1.8.35
        lib/librlottie.so.0
        lib/libz.so.1
        lib/libstdc++.so.6
        DESTINATION /opt/meegram/lib
    )

    install(TARGETS meegram DESTINATION /opt/meegram/bin)

    add_custom_target(package
        COMMAND mad dpkg-buildpackage -nc -uc -us
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building Debian package with mad..."
    )

endif()

