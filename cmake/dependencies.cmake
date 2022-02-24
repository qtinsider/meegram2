# dependencies

add_library(libs INTERFACE)
add_library(external::libs ALIAS libs)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# tdlib
message(STATUS "Fetching dependency TDLib...")
FetchContent_Declare(td
    GIT_REPOSITORY https://github.com/tdlib/td.git
    GIT_TAG        1e1ab5d1b0e4811e6d9e1584a82da08448d0cada
    GIT_SHALLOW    TRUE
    )

FetchContent_GetProperties(td)
if (NOT td_POPULATED)
    FetchContent_Populate(td)
    add_subdirectory(${td_SOURCE_DIR} ${td_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# nlohmann json
message(STATUS "Fetching dependency nlohmann json...")
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
    )

FetchContent_GetProperties(json)
if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# rlottie
set(rlottie_patch_command)
if (MEEGO_EDITION_HARMATTAN)
    set(rlottie_patch_command PATCH_COMMAND git reset --hard && git apply ${CMAKE_CURRENT_SOURCE_DIR}/tools/rlottie-harmattan.patch)
endif()

message(STATUS "Fetching dependency rlottie...")
FetchContent_Declare(rlottie
    GIT_REPOSITORY https://github.com/Samsung/rlottie.git
    GIT_TAG        875626965959d8e269ca22175c8e1ad190696c43
    GIT_SHALLOW    TRUE
    ${rlottie_patch_command}
    )

FetchContent_GetProperties(rlottie)
if (NOT rlottie_POPULATED)
    FetchContent_Populate(rlottie)
    add_subdirectory(${rlottie_SOURCE_DIR} ${rlottie_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_include_directories(libs INTERFACE "${td_SOURCE_DIR}/td/generate/auto/" "${td_SOURCE_DIR}/tdutils/" "${td_BINARY_DIR}/tdutils/")
target_link_libraries(libs INTERFACE nlohmann_json::nlohmann_json rlottie::rlottie Td::TdJsonStatic)
