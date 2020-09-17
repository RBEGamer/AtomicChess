# BRANCH - BUILDROOT

This branch contains the buildroot framework to build the AtomicChessOS.

The buildroot configuration is stored under `./buildroot/.config`



# NOTES

The configuration of the buildroot system was a bit difficult. 
There are a few dependencies eg for the touchscreen which have to be checkes manually.
The goal of the buildroot system for the ATC Project is, to build a working/ ready to use image, which contains all needed software to drive a ATC Table.

For this purpose, three custom buildroot packages were created, to intregrate the needed ATC Software into the final SD-Card image.
The final SD Card image, can directly flashed to the RaspberryPi and the build cross plattform compiler toolchain can be used to develop software on the host computer and run it on the target hardware. 

This setup was used in the whole software development process. Especially for the gui application. QT5 needs this cross compile toolchain to compile the application successful.


## OVERVIEW BUILDROOT

### WHAT IS BUILDROOT
Buildroot is s software framework that automates the process of building a complete Linux Images for many embedded systems.
It also generate a cross compilation toolchain for the host system.
Buildroot not only generates a host site toolchain, it also generates the target file system with compiled kernel and bootloader.

"Buildroot is useful mainly for people working with embedded systems. Embedded systems often use processors that are not the regular x86 processors everyone is used to having in his PC. They can be PowerPC processors, MIPS processors, ARM processors, etc.
Buildroot supports numerous processors and their variants; it also comes with default configurations for several boards available off-the-shelf."

[SOURCE](https://buildroot.org/downloads/manual/manual.html#_about_buildroot)

### SYSTEM REQUIREMENTS





* how its works
* packages
* configuration +  screenshot
* make and make clean

## BASIC CONFIGURATION

To create a basic configuration buildroot provied a set of template boards/architectures to setup a minimal running configuration.
All avariable configuration can be found in the boards folder `./buildroot/board`. Our Target Sytem is the RaspberryPi 3b+, there is a template for this already in the board folder and can be loaded with the `make <BOARD_NAME>_defconfig`. After running `$ make raspberrypi3_defconfig`, the buildroot config file `./buildroot/.config` contains all needed parameters and packages.

Buildroot offer a terminal based grahpical menu, to select the right options and packages to install.
To access the menu, simply call `$ make menuconfig` inside of the buildroot root folder.

![make menuconfig][./documentation_images/buildroot_make_config.png]


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

### SITENOTE_TOOLCHAIN:
With the standart raspian os for the raspberry pi, its possible to install an compiler like gcc and a buildsystem like make, directly on the raspberry pi.

In the `Toolchain` Menu, its possible to setup, all parameters for the host toolchain that buildroot builds, together with the target image.
For example its possible to install a GCC or a Fortran compiler into the toolchain.
For development, we need a C++ compiler and a debugger. The GDB Debugger has to be installed on the target and on the host. Buildroot selects the right version for host and target.

* `Toolchain -> Enable C++ support` -> enables the gcc and g++ compiler with the make build system
* `Toolchain -> Build Cross GDB` -> build the GDB Debugger for host and target

All other settings are the default settings. Its also possible to register the toolchain to an IDE linke Eclipse for easier setup on the host side.
 

## SYSTEM CONFIGURATION,
The system configuration allows to setup a root user, password, which keyboard layout should be used,...

The `System-Configuration` Settings comes also with predefined values. There are only some which needed to be modified.

* `System-Configuration -> Networkinterface DHCP`, this setting defined the Ethernet Interface to enable at startup and using a DHCP Server to obtain a IPv4 Adress. The value on a stock RPI3b+ can be ETH0 (LAN) or wlan0 for Wifi. The ATC_Table is using ethernet as its main network interface so `eth0` is the right choice.

* `System-Configuration -> Root filesystem overlay dir`, this filepath represents the Overlay Directory and is a custom Folder. All files in this directory will be copies into the final image rootfs and replaces existing files. In our case, this will be used for the wifi configuration and other ssh configuration files.

?? WHICH ONE ??
### /DEV MANAGEMENT

The setting for the /dev managemtns is very important! The ATC_OS need user interaction thought an input device, like a mouse, keyboard or a touchscreen. By default the udev is disbaled on the system, so its not possible to simply adding a touchscreen input to the system.

* `System Configuration -> /dev management (devtmpfs + eudev)`, enables udev subsystem

UDEV basicly handles the events coming from an input source (Keyboard,.., plugged Harddrive) and performing tasks based in this inputs.
On task is to load a driver if can keyboard is attached though an usb port. In our case its used to access the touchscreen, after connecting the display via the DSI Interface to the Raspberry Pi, udev loads the FT5406 Touchscreen IC driver and make it accessable as `/dev/input/mouse1`.



## TARGET PACKAGES CONFIGURATION

The target package configuration allows to install software and libraries on the target. For example we want to install an ftp server onto the target. In the target packages we can select the ftpserver packe and all other needed dependencies will be installed too.
Its a very simple process, just select the software and all other needed packages will selected too.

### QT5
The most important library is the QT5 framework.
The UserInterface of the ATC_OS is based on QT5 with the QuickControls II extention, so a QT Version > 5.7 is needed to enable this feature.
Buildroot supports the QT5.6 LTS version and the latest QT5.12 version by default. So here we have simply to enable the QT5 packge with all extetion packages.

After enabling the QT5 packge, buildroot automaticly enabled EGLFS as grahpics backend. The backend is needed to enable application to render and show graphics on the screen. EGLFS is able to make use of the GPU for graphics acceleration and is needed by the QT5 QuickControl II Extention to render animations.

QT5 alone is able to use other graphics backend too, for example the directfb. the directfb is the simplest graphics backend provied by the linux system, and it can be used for very simple applications.


?? WHAT IS AGRAPIC BACKEND ??
?? IMAGE OF ALL SELECTED PACKAGES ??


* `Target Packages -> Graphic -> QT5`, enabled QT5 
* `Target Packages -> Graphic -> QT5 -> Default Graphic Plattform (ELGFS)`, set the default graphic backend to ELGFS for Hardware Graphics Acceleration


### SSH
In the case of the ATC_OS we need a ssh server to connect to the target via ethernet. This allows the remote debugging and development of the software. Most IDE support the deployment of the build software over ssh to a target system. So a ssh and sftp server is essential for development.

Buildroot provides the SSH and SFTP server in seperate packages:

* `Target Packages -> Networking Applications -> gsftpserver`, enable the SFTP Server, for file transfers
* `Target Packages -> Networking Applications -> dropbear`, ebake the SSH Server, for a secure shell

A NTP client will also be installed, so the target system can fetch the correct time over a NTP Server.
This is needed for a successful SSH HTTP Request the ATC_CommuncicationClient is used to the chess server.
With incorrect time setting, the Client/Server is not able to verify a SSL Certificate.



### MISC PACKAGES
In the development phase its useful to install some other software packages to the target system:

* `Target Packages -> Text editors -> nano`, install a simple text editor
* `Target Packages -> Networking Applications -> rsync`, proves simple file sync (also used by QT if avariable)
* `Target Packages -> System Tools -> htop`, a better taskmanager as the top application

There are some more applications and libraries for development and testing installed, but the system is working with these mentioned above.



### FILE SYSTEM SIZE
The QT libraries and the other needed libraries are quite large in size. For the full QT5 with QT3D and the Virtual Keyboard and the nedded QuickControls II the rootfs file system is about 230MB. The default setting is about 128MB. So buildroot can not build the final image beacuas its not enought space. To increase the rootfs maximum size, buildroot provides a size option under `Filesystem images -> root filesystem -> exact size`. With the SPACEBAR_KEY its possible to edit the default value to 512M. A Zero for automatic determination is not supported.

The `Fileystem images` category, provides several other options for the rootfs. Its also possible to create a readonly filesystem or a compressed one. For debudding purposes, the readonly filesystem option is disabled. All other settings are the default settings.

After building a complete image the size is about 270mb in Size, this includes the bootloader, os with all libraries and packages with the ATC needed programs (UI, Communication Client).

## FILE SYSTEM OVERLAY
* general function and usage
* usage in buildroot to modify the final filesystem
* concrete usage here, with ssh keys
?? IMAGE OF THE OVERLAY DIAGRAM ??


### AUTOBOOT
*initrd file
### SSH KNOWN HOSTS
* ssh file


## HOW TO FINALLY BUILD

### HOST BUILD RESULT

### BUILD THE IMAGE AND HOST BINARIES

`$ cd ./buildroot/ && make` - build the image and the host binaries.




### HOST CROSS COMPILER

The crosscompiler (gcc,g++,gdb and qt stuff) can be found in the sysroot directory `./buildroot/output/host/bin/`

* G++ `arm-buildroot-linux-gnueabihf-g++`
* GCC `arm-buildroot-linux-gnueabihf-gcc`
* GDB `arm-buildroot-linux-gnueabihf-gdb`
* CMAKE `cmake`
* QT_QMAKE `qmake`

The sysroot of the target system (needed for the QT Kit Sysroot Setting) is located in the  `./buildroot/output/host/arm-buildroot-linux-gnueabihf/sysroot` Folder.

### TARGET BUILD RESULT
The build image can be found in `./buildroot/output/images/sdcard.img`. This image already contains the root filesystem, the boot partition with bootloader and firmware espacially for the RPI.

It can be flashed using the linux `dd` command
`$ dd bs=4M if=./sdcard.img of=/dev/<SD_CARD> conv=fsync`
or by using a gui based utility like [Win32DiskImager](https://github.com/znone/Win32DiskImager) or the [Etcher](https://www.balena.io/etcher/).

After flashing the image to an sd card, the device is ready for booting.
If the device boot sequence finsihed, its possible to connect with the taerget board over SSH with the in the `System-Settings` Chapter given settings.
On the development pc its also possible to connect without a user password. This is possible though host public key signature the `/root/.ssh/known_hosts` file, given though the `FILE SYSTEM OVERLAY` system in the build process.




## ADDING OWN PACKAGES 
* how to create a own packes
* which packaes are needed for the ATC OS
* which file needs to be created and where (screenshot)
* screenshot new meu items


## TOUCHSCREEN CONFIGURATION
* /dev managemtnw wurde eingerichtet
* which type
* what is a device tree
* how to modify
* rpi overlay loading using modprobe
* including into buildroot (kernel->evdev libsci)
* testing the touchscreen






## HOWTO INTEGRATE INTO JENKINS
* whta is jenkins
* setup jenkins itself (simple docker setup ( persiest config storage / worker on host itself)
* setup instructions
* when to trigger a build

# HOW TO HANDLE ARTEFACTS
* what are artefacts
* using ftp to upload final image
* tag the git commit thats build
