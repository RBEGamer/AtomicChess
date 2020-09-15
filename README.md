# AtomicChessOS


# BRANCH - BUILDROOT

This branch contains the buildroot framework to build the AtomicChessOS.

The buildroot configuration is stored under `./buildroot/.config`

## BUILD THE IMAGE AND HOST BINARIES

`$ cd ./buildroot/ && make` - build the image and the host binaries.

The build image can be found in `./buildroot/output/images/sdcard.img`


## CROSS COMPILER

The crosscompiler (gcc,g++,gdb and qt stuff) can be found in the sysroot directory `./buildroot/output/host/bin/`

* G++ `arm-buildroot-linux-gnueabihf-g++`
* GCC `arm-buildroot-linux-gnueabihf-gcc`
* GDB `arm-buildroot-linux-gnueabihf-gdb`
* CMAKE `cmake`
* QT_QMAKE `qmake`

The sysroot of the target system (needed for the QT Kit Sysroot Setting) is located in the  `./buildroot/output/host/arm-buildroot-linux-gnueabihf/sysroot` Folder.


## CONFIGURATION NOTES

The configuration of the buildroot system was a bit difficult. There are a few dependencies eg for the touchscreen which have to be checkes manually.

The goal of the buildroot system for the ATC Project is, to build a working/ ready to use image, which contains all needed software to drive a ATC Table.



## OVERVIEW BUILDROOT
* how its works
* packages
* configuration +  screenshot
* make and make clean

## BASIC CONFIGURATION
* file system size
* networking
* ssh enable; audio_enable, qt5 enable
* disable password

## ADDING OWN PACKAGES 
* how to create a own packes
* which packaes are needed for the ATC OS

## FILE SYSTEM OVERLAY
* general function and usage
* usage in buildroot to modify the final filesystem
* concrete usage here, with ssh keys


## TOUCHSCREEN CONFIGURATION
* which type
* what is a device tree
* how to modify
* rpi overlay loading using modprobe
* including into buildroot (kernel->evdev libsci)
* testing the touchscreen


## AUTOBOOT

## HOWTO INTEGRATE INTO JENKINS
* setup instructions
* using ftp to upload final image
