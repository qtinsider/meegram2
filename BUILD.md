# Build Instruction

You need to have `QtSDK` install on your system.


## Required Packages
* Starting with a clean Debian system, you must first install a few packages:

  ```
  $ sudo apt-get update
  $ sudo apt-get install build-essential g++ gawk make wget
  ```

* Everything else will be built from source. Create a new directory somewhere, and download the following source packages.

  ```
  $ wget http://ftpmirror.gnu.org/binutils/binutils-2.32.tar.xz
  $ wget http://ftpmirror.gnu.org/gcc/gcc-11.1.0/gcc-11.1.0.tar.xz
  ```

## Build Steps

* Extract all the source packages.

  ```
  $ tar xf binutils-2.32.tar.xz
  $ tar xf gcc-11.1.0.tar.xz
  ```

* Download GCC prerequisites..

  ```
  $ cd gcc-11.1.0
  $ contrib/download_prerequisites
  $ cd ..
  ```

Choose an installation directory, and make sure you have write permission to it. In the steps that follow, I’ll install the new toolchain to `/opt/strawberry-gcc-11.1` .

```
$ sudo mkdir -p /opt/strawberry-gcc-11.1
$ sudo chown $USER /opt/strawberry-gcc-11.1
```
Throughout the entire build process, make sure the installation’s bin subdirectory is in your PATH environment variable. You can remove this directory from your `PATH` later, but most of the build steps expect to find `arm-none-linux-gnueabi` and other host tools via the PATH by default.
```
$ export PATH=/opt/strawberry-gcc-11.1/bin:$PATH
```

### Binutils

```
$ mkdir build-binutils
$ cd build-binutils
$ ../binutils-2.32/configure --prefix=/opt/strawberry-gcc-11.1  --target=arm-none-linux-gnueabi --with-float=hard --with-sysroot=/opt/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim --disable-multilib
$ make -j4
$ make install
$ cd ..
```

### GCC

```
$ mkdir -p build-gcc
$ cd build-gcc
$ ../gcc-11.1.0/configure --prefix=/opt/strawberry-gcc-11.1 --with-sysroot=/opt/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim --with-fpu=neon --with-float=hard --target=arm-none-linux-gnueabi --with-arch=armv7-a --enable-languages=c,c++ --disable-multilib 
$ make -j4 
$ make install
$ cd ..
```

You can verify by typing this:

```
$ arm-none-linux-gnueabi-gcc -v
```

You are expected to see output like:

```
Using built-in specs.
COLLECT_GCC=arm-none-linux-gnueabi-gcc
COLLECT_LTO_WRAPPER=/opt/strawberry-gcc-11.1/libexec/gcc/arm-none-linux-gnueabi/11.1.0/lto-wrapper
Target: arm-none-linux-gnueabi
Configured with: ../gcc-11.1.0/configure --prefix=/opt/strawberry-gcc-11.1 --with-sysroot=/opt/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim --with-fpu=neon --with-float=hard --disable-multilib --target=arm-none-linux-gnueabi --with-arch=armv7-a --enable-languages=c,c++
Thread model: posix
Supported LTO compression algorithms: zlib
gcc version 11.1.0 (GCC)
```
