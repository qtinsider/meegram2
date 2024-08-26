cmake_minimum_required(VERSION 3.10)

set(QR_CODE_GENERATOR_SOURCES ${CMAKE_SOURCE_DIR}/lib/QR-Code-generator/cpp/qrcodegen.cpp)
set(QR_CODE_GENERATOR_HEADERS ${CMAKE_SOURCE_DIR}/lib/QR-Code-generator/cpp/qrcodegen.hpp)

add_library(QRCodeGenerator STATIC ${QR_CODE_GENERATOR_SOURCES})

target_include_directories(QRCodeGenerator PUBLIC ${CMAKE_SOURCE_DIR}/lib/QR-Code-generator/cpp)

set_target_properties(QRCodeGenerator PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
)

target_compile_options(QRCodeGenerator PRIVATE -Wall -Wextra -pedantic)
