# AtomicChessOS


# BRANCH - BUILDROOT

This branch contains the buildroot framework to build the AtomicChessOS.

The buildroot configuration is stored under `./buildroot/.config`

## BUILD THE IMAGE AND HOST BINARIES

`$ cd ./buildroot/ && make` - build the image and the host binaries.

The build image can be found in `./buildroot/output/images/sdcard.img`

The crosscompiler (gcc,g++,gdb and qt stuff) can be found in the sysroot directory `./buildroot/output/host/bin/`

* G++ `arm-buildroot-linux-gnueabihf-g++`
* GCC `arm-buildroot-linux-gnueabihf-gcc`
* GDB `arm-buildroot-linux-gnueabihf-gdb`
* CMAKE `cmake`
* QT_QMAKE `qmake`

The sysroot of the target system (needed for the QT Kit Sysroot Setting) is located in the  `./buildroot/output/host/arm-buildroot-linux-gnueabihf/sysroot` Folder.
