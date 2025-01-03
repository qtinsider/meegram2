cmake_minimum_required(VERSION 3.20)
project(MeeGram VERSION 0.1.7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

find_package(Td REQUIRED)
find_package(rlottie REQUIRED)
find_package(Qt4 4.7.4 COMPONENTS QtCore QtDBus QtGui QtSvg QtXml QtDeclarative REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

find_package(Sanitizers QUIET)

option(BUILD_HARMATTAN "Build for MeeGo 1.2 Harmattan Device" OFF)

if (BUILD_HARMATTAN)
    pkg_check_modules(boostable QUIET qdeclarative-boostable)
endif()

set(QML_IMPORT_PATH "/home/insider/QtSDK/Simulator/Qt/gcc/imports" CACHE STRING "")

include(cmake/qr_code_generator.cmake)

set(SRC_FILES
    src/AppManager.cpp
    src/Authorization.cpp
    src/BasicGroup.cpp
    src/Chat.cpp
    src/ChatFolderModel.cpp
    src/ChatManager.cpp
    src/ChatModel.cpp
    src/ChatPhotoProvider.cpp
    src/ChatPosition.cpp
    src/Client.cpp
    src/CountryModel.cpp
    src/Emoji.cpp
    src/EmojiModel.cpp
    src/File.cpp
    src/LanguagePackInfoModel.cpp
    src/Localization.cpp
    src/LottieAnimation.cpp
    src/main.cpp
    src/Message.cpp
    src/MessageContent.cpp
    src/MessageModel.cpp
    src/MessageService.cpp
    src/PluralRules.cpp
    src/QrCodeItem.cpp
    src/Settings.cpp
    src/StorageManager.cpp
    src/SupergroupFullInfo.cpp
    src/Supergroup.cpp
    src/User.cpp
    src/Utils.cpp
)

set(HEADER_FILES
    src/AppManager.hpp
    src/Authorization.hpp
    src/BasicGroup.hpp
    src/Chat.hpp
    src/ChatFolderModel.hpp
    src/ChatManager.hpp
    src/ChatModel.hpp
    src/ChatPhotoProvider.hpp
    src/ChatPosition.hpp
    src/Client.hpp
    src/Common.hpp
    src/CountryModel.hpp
    src/Emoji.hpp
    src/EmojiModel.hpp
    src/File.hpp
    src/LanguagePackInfoModel.hpp
    src/Localization.hpp
    src/LottieAnimation.hpp
    src/Message.hpp
    src/MessageContent.hpp
    src/MessageModel.hpp
    src/MessageService.hpp
    src/PluralRules.hpp
    src/QrCodeItem.hpp
    src/Settings.hpp
    src/StorageManager.hpp
    src/Supergroup.hpp
    src/SupergroupFullInfo.hpp
    src/User.hpp
    src/Utils.hpp
)

set(QRC_FILES
    resources/emoji_1.qrc
    resources/emoji_2.qrc
    resources/emoji_3.qrc
    resources/emoji_4.qrc
    resources/emoji_5.qrc
    resources/emoji_6.qrc
    resources/fonts.qrc
    resources/images.qrc
    resources/qml.qrc
    resources/tgs.qrc
)

add_executable(meegram ${SRC_FILES} ${HEADER_FILES} ${QRC_FILES})

set_target_properties(meegram PROPERTIES AUTOMOC ON AUTORCC ON)

target_compile_options(meegram PRIVATE -Wall -Wextra -pedantic $<IF:$<CONFIG:Debug>, -Werror, -Wno-psabi>)

target_link_libraries(meegram PRIVATE
    Lib::QRCode
    Td::TdStatic
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

add_sanitizers(meegram)

if (BUILD_HARMATTAN)
    target_compile_options(meegram PRIVATE ${boostable_CFLAGS})
    target_include_directories(meegram PRIVATE ${boostable_INCLUDE_DIRS})

    target_link_options(meegram PRIVATE -Wl,-rpath,/opt/meegram/lib -Wl,--dynamic-linker=/lib/ld-linux.so.3 -pie -rdynamic)

    target_link_libraries(meegram PRIVATE ${boostable_LIBRARIES} $<$<BOOL:${SANITIZE_ADDRESS}>:-static-libasan> -static-libstdc++)

    if(NOT EXISTS "${CMAKE_BINARY_DIR}/debian")
        file(COPY ${CMAKE_SOURCE_DIR}/debian DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    install(FILES resources/meegram_harmattan.desktop DESTINATION /usr/share/applications)
    install(FILES resources/meegram80.png DESTINATION /usr/share/icons/hicolor/80x80/apps)
    install(FILES resources/meegram-splash.png DESTINATION /opt/meegram/share)

    install(TARGETS meegram DESTINATION /opt/meegram/bin)

    add_custom_target(package
        COMMAND mad dpkg-buildpackage -nc -uc -us
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building Debian package with mad..."
    )
endif()
