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

To create a basic configuration buildroot provied a set of template boards/architectures to setup a minimal running configuration.
All avariable configuration can be found in the boards folder `./buildroot/board`. Our Target Sytem is the RaspberryPi 3b+, there is a template for this already in the board folder and can be loaded with the `make <BOARD_NAME>_defconfig`. After running `$ make raspberrypi3_defconfig`, the buildroot config file `./buildroot/.config` contains all needed parameters and packages.

The important basic configuration points are the following:
* `Target options -> Target Architecture`, the RPI uses an ARM CPU, so the setting is set to `ARM (little endian)`
* `Target options -> Target Architecture Variant`, defines the specific CPU Model. In the RPI3b+ case its an `Cortex A53`


?? WHAT IS A DEVICE TREE ??
* `Kernel -> Device Tree Settings`, this point defines which device tree to build. In this case the predefined device tree `bcm-2710-rpi3b` is used. The DTB (DeviceTreeBinary) Files are downloaded from the RaspberryPi-Firmware GitHub-Repository automaticly form buildroot.

* `Kernel -> Kernel Version`, in this case the precompiled kernel was used which are downloaded form the RaspberryPi-GitHub-Repository.

These a re basic nedded configuration items to setup a minimum booting system. Also buildroot generates a cross-compiler for the host syetem. So its possible to build software for the target-system. 

The next step is the system configuration and the target packages configuration.

## TOOLCHAIN

The buildroot framework does not only build a image for the target device, it also build all nessessary tools for build software for the target. This software collection is called toolchain and in runnung on the host system. On the target plattform is no compiler or build system installed, so its nit possible to compile software directly on the target, with an buildroot image.

** SITENOTE:
With the standart raspian os for the raspberry pi, its possible to install an compiler like gcc and a buildsystem like make, directly on the raspberry pi.



## SYSTEM CONFIGURATION,
The system configuration allows to setup a root user, password, which keyboard layout should be used,...

The `System-Configuration` Settings comes also with predefined values. There are only some which needed to be modified.

* `System-Configuration -> Networkinterface DHCP`, this setting defined the Ethernet Interface to enable at startup and using a DHCP Server to obtain a IPv4 Adress. The value on a stock RPI3b+ can be ETH0 (LAN) or wlan0 for Wifi. The ATC_Table is using ethernet as its main network interface so `eth0` is the right choice.

* `System-Configuration -> Root filesystem overlay dir`, this filepath represents the Overlay Directory and is a custom Folder. All files in this directory will be copies into the final image rootfs and replaces existing files. In our case, this will be used for the wifi configuration and other ssh configuration files. ?? WHICH ONE


# TARGET PACKAGES CONFIGURATION



### FILE SYSTEM SIZE
The QT libraries and the other needed libraries are quite large in size. For the full QT5 with QT3D and the Virtual Keyboard and the nedded QuickControls II the rootfs file system is about 230MB. The default setting is about 128MB. So buildroot can not build the final image beacuas its not enought space. To increase the rootfs maximum size, buildroot provides a size option under `Filesystem images -> root filesystem -> exact size`. With the SPACEBAR_KEY its possible to edit the default value to 512M. A Zero for automatic determination is not supported.

The `Fileystem images` category, provides several other options for the rootfs. Its also possible to create a readonly filesystem or a compressed one. For debudding purposes, the readonly filesystem option is disabled. All other settings are the default settings.

After building a complete image the size is about 270mb in Size, this includes the bootloader, os with all libraries and packages with the ATC needed programs (UI, Communication Client).

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
