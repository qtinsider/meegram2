#!/usr/bin/env bash

set -euo pipefail

# Define colors for output
declare -r COLOR_RESET="\033[0m"
declare -r COLOR_RED="\033[31m"
declare -r COLOR_GREEN="\033[32m"
declare -r COLOR_BLUE="\033[34m"
declare -r COLOR_CYAN="\033[36m"
declare -r COLOR_MAGENTA="\033[35m"

# Arguments and paths
readonly ARGS="${1:-}"
readonly SDK_PATH="${2:-}"

# Check if TOOLCHAIN_PREFIX is set, if needed
check_toolchain_prefix() {
    if [[ "$ARGS" == "harmattan" ]]; then
        if [[ -z "${TOOLCHAIN_PREFIX:-}" ]]; then
            error "TOOLCHAIN_PREFIX is not set. Please define it before running the script."
            exit 1
        fi
    fi
}

# Functions for colored output
info() {
    echo -e "${COLOR_BLUE}INFO: $*${COLOR_RESET}"
}

success() {
    echo -e "${COLOR_GREEN}SUCCESS: $*${COLOR_RESET}"
}

warn() {
    echo -e "${COLOR_MAGENTA}WARNING: $*${COLOR_RESET}"
}

error() {
    echo -e "${COLOR_RED}ERROR: $*${COLOR_RESET}" >&2
}

# Validate arguments
validate_args() {
    if [ -z "$ARGS" ]; then
        error "No device specified. Please specify 'harmattan' or 'simulator'."
        exit 1
    fi

    if [[ "$ARGS" != "harmattan" && "$ARGS" != "simulator" ]]; then
        error "Incorrect device specified. Please specify 'harmattan' or 'simulator'."
        exit 1
    fi
}

# SHA256 checksum verification
check_sha256() {
    local expected_hash="$1"
    local file="$2"

    info "Verifying SHA256 checksum for $file..."
    local actual_hash
    actual_hash=$(sha256sum "$file" | awk '{print $1}')
    
    if [ "$actual_hash" != "$expected_hash" ]; then
        error "SHA256 checksum mismatch for $file. Expected $expected_hash, got $actual_hash."
        exit 1
    fi

    success "SHA256 checksum verified for $file."
}

# Build OpenSSL
build_openssl() {
    local version="3.3.1"
    local hash="777cd596284c883375a2a7a11bf5d2786fc5413255efab20c50d6ffe6d020b7e"
    local filename="openssl-$version.tar.gz"

    if [ -d "openssl-$version" ]; then
        warn "OpenSSL directory already exists, skipping download and extraction."
    else
        if [ ! -f "$filename" ]; then
            info "Downloading OpenSSL sources..."
            wget "https://www.openssl.org/source/$filename"
        fi
        
        check_sha256 "$hash" "$filename"

        info "Extracting OpenSSL sources..."
        tar xzf "$filename"
    fi

    cd "openssl-$version"

    info "Configuring OpenSSL..."
    if [[ "$ARGS" == "harmattan" ]]; then
        ./Configure --cross-compile-prefix="$TOOLCHAIN_PREFIX-" linux-generic32 shared no-unit-test
    else
        ./config shared
    fi

    info "Building OpenSSL..."
    make depend
    make -j4

    info "Installing OpenSSL..."
    mkdir -p ../build/crypto/lib
    cp libcrypto.so* libssl.so* ../build/crypto/lib
    cp -r include ../build/crypto

    cd ..
    success "OpenSSL built successfully."
}

# Build ZLib
build_zlib() {
    local version="1.3.1"
    local hash="38ef96b8dfe510d42707d9c781877914792541133e1870841463bfa73f883e32"
    local filename="zlib-$version.tar.xz"

    if [ -d "zlib-$version" ]; then
        warn "ZLib directory already exists, skipping download and extraction."
    else
        if [ ! -f "$filename" ]; then
            info "Downloading ZLib sources..."
            wget "https://www.zlib.net/$filename"
        fi

        check_sha256 "$hash" "$filename"

        info "Extracting ZLib sources..."
        tar xJf "$filename"
    fi

    cd "zlib-$version"

    info "Configuring ZLib..."
    if [[ "$ARGS" == "harmattan" ]]; then
        CC="$TOOLCHAIN_PREFIX-gcc" CFLAGS="-fPIC" ./configure --shared
    else
        CFLAGS="-fPIC" ./configure --shared
    fi

    info "Building ZLib..."
    make -j4

    info "Installing ZLib..."
    mkdir -p ../build/zlib/lib ../build/zlib/include
    cp libz.so* ../build/zlib/lib
    cp zconf.h zlib.h ../build/zlib/include

    cd ..
    success "ZLib built successfully."
}

# Build TDLib
build_tdlib() {
    if [ -d "td" ]; then
        warn "TDLib repository already exists, skipping clone."
    else
        info "Cloning TDLib repository..."
        git clone --depth=1 https://github.com/tdlib/td
    fi

    cd td

    if [[ "$ARGS" == "harmattan" ]]; then
        sed -i 's/TD_HAS_MMSG 1/TD_HAS_MMSG 0/g' tdutils/td/utils/port/config.h
    fi

    cd ..

    rm -rf build/generate build/tdlib
    mkdir -p build/generate build/tdlib

    local td_root=$(realpath td)
    local zlib_root=$(realpath build/zlib)
    local openssl_root=$(realpath build/crypto)
    local openssl_crypto_lib="$openssl_root/lib/libcrypto.so"
    local openssl_ssl_lib="$openssl_root/lib/libssl.so"
    local zlib_lib="$zlib_root/lib/libz.so"

    local openssl_options="-DOPENSSL_FOUND=1 -DOPENSSL_INCLUDE_DIR=\"$openssl_root/include\" -DOPENSSL_CRYPTO_LIBRARY=\"$openssl_crypto_lib\" -DOPENSSL_SSL_LIBRARY=\"$openssl_ssl_lib\""
    local zlib_options="-DZLIB_FOUND=1 -DZLIB_LIBRARIES=\"$zlib_lib\" -DZLIB_INCLUDE_DIR=\"$zlib_root/include\""

    if [[ "$ARGS" == "harmattan" ]]; then
        info "Setting up cross-compilation for Harmattan..."
        echo "
        SET(CMAKE_SYSTEM_NAME Linux)
        SET(CMAKE_SYSTEM_PROCESSOR arm)
        SET(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc)
        SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
        " > "$td_root/toolchain.cmake"
    fi

    if [[ "$ARGS" == "harmattan" ]]; then
        cd build/generate
        cmake "$td_root"
        cd ../..

        cd build/tdlib
        cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DTD_ENABLE_LTO=ON -DCMAKE_TOOLCHAIN_FILE="$td_root/toolchain.cmake" $openssl_options $zlib_options "$td_root"
        cd ../..

        info "Generating TDLib autogenerated source files..."
        cmake --build build/generate --target prepare_cross_compiling
        info "Building TDLib for Harmattan..."
        cmake --build build/tdlib
    else
        cd build/tdlib
        cmake -DCMAKE_BUILD_TYPE=Release $openssl_options $zlib_options "$td_root"
        cmake --build .
    fi

    if [[ "$ARGS" == "harmattan" ]]; then
        info "Installing TDLib to Harmattan SDK..."
        make DESTDIR="$SDK_PATH/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim" install
    else
        make install
    fi

    success "TDLib built and installed successfully."
}

# Main function to run the build process
main() {
    validate_args
    check_toolchain_prefix
    build_zlib
    build_openssl
    build_tdlib
}

main "$@"
