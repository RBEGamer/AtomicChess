# BRANCH - BUILDROOT

This branch contains the buildroot framework to build the AtomicChessOS.

The buildroot configuration is stored under `./buildroot/.config`



# NOTES



## OVERVIEW BUILDROOT

### GOAL OF THIS CHAPTER


### WHAT IS BUILDROOT

Buildroot is s software framework that automates the process of building a complete Linux Images for many embedded systems.
It also generate a cross compilation toolchain for the host system.
Buildroot not only generates a host site toolchain, it also generates the target file system with compiled kernel and bootloader.

"Buildroot is useful mainly for people working with embedded systems. Embedded systems often use processors that are not the regular x86 processors everyone is used to having in his PC. They can be PowerPC processors, MIPS processors, ARM processors, etc.
Buildroot supports numerous processors and their variants; it also comes with default configurations for several boards available off-the-shelf."

[SOURCE](https://buildroot.org/downloads/manual/manual.html#_about_buildroot)


### HOW BUILDROOT WORKS

* image


### SYSTEM REQUIREMENTS

In order to get the buildrootpackage running, the host linux system need some packages.
These packages can be installed with the `$ sudo apt install <PACKAGE>` command, the `$ sudo apt  update` command might be necessary, to get the latest versions.

* `$ sudo apt  update`
* `$ sudo apt install sed make binutils build-essential gcc g++ gzip bzip2 perl tar cpio unzip rsync bc wget git python -y`
* `$ sudo apt install gtk2.0 gtk2.0-dev csv python-glade2 libncurses5 libncurses5-dev -y`

The installation took place on a fresh installed Ubuntu 20.04, where most of the packages are already installed.

[SOURCE](https://buildroot.org/downloads/manual/manual.html#requirement)

#### INSTALL BUILDROOT

For this project a complete configured buildroot installation can be found in the git repository locatate in Folder:
`/src/src_buildroot/buildroot`. All commands listed in this chapter are relative to this path.

Its also possible to create a fresh installation of buildroot. The base version of buildroot used in this project, was the release number `2020.02` which can be found on the buildroot download page `https://buildroot.org/download.html`.


After downloading the `tar.gt` file from the downloadpage. The archive must be unpacked using the `tar` command:

`$ tar -xf Downloads/buildroot-2020.02.tar.gz`

The setup of buildroot is finished ater extraction process finished.


 
## BASIC CONFIGURATION

To create a basic configuration buildroot provied a set of template boards/architectures to setup a minimal running configuration.
All avariable configuration can be found in the boards folder `./board`. Our Target Sytem is the RaspberryPi 3b+, there is a template for this already in the board folder and can be loaded with the `make <BOARD_NAME>_defconfig`. After running `$ make raspberrypi3_defconfig`, the buildroot config file `./.config` contains all needed parameters and packages.

Buildroot offer a terminal based grahpical menu, to select the right options and packages to install.
To access the menu, simply call `$ make menuconfig` inside of the buildroot root folder.
The following screen appears in the current terminal window:

![make menuconfig](./documentation_images/buildroot_make_config.png)

The menu is split into several categories and the most important basic configuration points are explained below.

### Target options

* `Target options -> Target Architecture`, the RPI uses an ARM CPU, so the setting is set to `ARM (little endian)`
* `Target options -> Target Architecture Variant`, defines the specific CPU Model. In the RPI3b+ case its an `Cortex A53`

### Kernel

#### WHAT IS A KERNEL

#### WHAT IS A DEVICE TREE

#### KERNEL CONFIGURATION

![make menuconfig](./documentation_images/buildroot_kernel.png)
?? WHAT IS A DEVICE TREE ??

* `Kernel -> Device Tree Settings`, this point defines which device tree to build. In this case the predefined device tree `bcm-2710-rpi3b` is used. The DTB `DeviceTreeBinary` Files are downloaded from the RaspberryPi-Firmware Repository automaticly form buildroot.

Its also possible to add additional Device Tree Overlays in additon to the predefined overlay. This is the case if we want to add additional hardware to the target system. In our case, we add a an DTD Overlay to the system, to access the Touchscreen.
The tocuhscreen controller used, is the FT 5406. This controller is also used in the original 7" Raspberry Pi FTF Display, so there is a DTD Overlayfile already existing for loading.
The DTB File can be found in the RaspverryPi-Firmware Repository `rpi-firmware/overlays/rpi-ft5406-overlay.dtb`. We have to make shure that this file exists in the boot partition `<SD_CARD>/boot/overlays/` of the final image.
All DTB files will be loaded at startup if nessessarry. Its also possible to load additional DTB files with the configuration file `/boot/config.txt` of the Raspberry-Pi firmware.

* `Kernel -> Kernel Version`, in this case the precompiled kernel was used which are downloaded form the RaspberryPi-GitHub-Repository.

These are basic nedded configuration items to setup a minimum booting system.
Also buildroot generates a cross-compiler for the host system. So its possible to build software for the target-system.

The next step is the system configuration and the target packages configuration.

### TOOLCHAIN

![make menuconfig](./documentation_images/buildroot_toolchain.png)

The buildroot framework does not only build a image for the target device, it also build all nessessary tools for build software for the target. This software collection is called toolchain and in runnung on the host system. On the target plattform is no compiler or build system installed, so its nit possible to compile software directly on the target, with an buildroot image.

#### SITENOTE_TOOLCHAIN

With the standard raspian os for the raspberry pi, its possible to install an compiler like gcc and a buildsystem like make, directly on the raspberry pi.

#### END SITENOTE

In the `Toolchain` Menu, its possible to setup, all parameters for the host toolchain that buildroot builds, together with the target image.
For example its possible to install a GCC or a Fortran compiler into the toolchain.
For development, we need a C++ compiler and a debugger.
The GDB Debugger has to be installed on the target and on the host. Buildroot selects the right version for host and target.

* `Toolchain -> Enable C++ support` -> enables the gcc and g++ compiler with the make build system
* `Toolchain -> Build Cross GDB` -> build the GDB Debugger for host and target

All other settings are the default settings. Its also possible to register the toolchain to an IDE linke Eclipse for easier setup on the host side.

### SYSTEM CONFIGURATION

![make menuconfig](./documentation_images/buildroot_system_config.png)

The system configuration allows to setup a root user, password or which keyboard layout should be used. Its like the `$ sudo raspi-config` command in a regular Raspbian installation.

The `System-Configuration` Settings comes also with predefined values. There are only some which needed to be modified.

* `System-Configuration -> Networkinterface DHCP`, this setting defined the Ethernet Interface to enable at startup and using a DHCP Server to obtain a IPv4 Adress. The value on a stock RPI3b+ can be ETH0 (LAN) or wlan0 (WIFI). The ATC_Table is using ethernet as its main network interface so `eth0` is the right choice.

* `System-Configuration -> Root filesystem overlay dir`, this filepath represents the Overlay Directory and is a custom Folder. All files in this directory will be copies into the final image rootfs and replaces existing files. In our case, this will be used for the wifi configuration and other ssh configuration files.

For debugging purposes, the access to the root aacount though a password is acceptable. In the production build of the image, the root access is disbaled.


#### /DEV MANAGEMENT

The setting for the /dev managemtns is very important! The ATC_OS need user interaction thought an input device, like a mouse, keyboard or a touchscreen. By default the udev is disbaled on the system, so its not possible to simply adding a touchscreen input to the system.

* `System Configuration -> /dev management (devtmpfs + eudev)`, enables udev subsystem.

UDEV basicly handles the events coming from an input source (Keyboard,.., plugged Harddrive) and performing tasks based in this inputs.
On task is to load a driver if can keyboard is attached though an usb port. In our case its used to access the touchscreen, after connecting the display via the DSI Interface to the Raspberry Pi, udev loads the FT5406 Touchscreen IC driver and make it accessable as `/dev/input/mouse1` (a usb mice is already plugged in).

![/DEV_FOLDER](./documentation_images/rpi_input_methods.png)

### TARGET PACKAGES CONFIGURATION

The target package configuration allows to install software and libraries on the target. For example we want to install an ftp server onto the target. In the target packages we can select the ftpserver packe and all other needed dependencies will be installed too.
Its a very simple process, just select the software and all other needed packages will selected too.

#### QT5

The most important library is the QT5 framework.
The userinterface of the ATC_OS  called ATC_UI is based on QT5 with the QuickControls II extention, so a QT Version >=5.7 is needed to enable this feature.
Buildroot supports the QT5.6 LTS version and the latest QT5.12 version by default. So here we have simply to enable the QT5 packge with all extetion packages.

![QT_PACKAGES_SELECTED](./documentation_images/buildroot_qt_backages.png)

After enabling the QT5 packge, buildroot automaticly enabled EGLFS as grahpics backend. The backend is needed to enable application to render and show graphics on the screen. EGLFS is able to make use of the GPU for graphics acceleration and is needed by the QT5 QuickControl II Extention to render animations.

QT5 alone is able to use other graphics backend too, for example the directfb. the directfb is the simplest graphics backend provided by the linux system, and it can be used for very simple applications.

#### WHAT IS AGRAPIC BACKEND


* `Target Packages -> Graphic -> QT5`, enables QT5, with the QT5_BASIC package. Use the `Return-Key` to access all QT5 packages.
* `Target Packages -> Graphic -> QT5 -> Default Graphic Plattform (ELGFS)`, set the default graphic backend to ELGFS for Hardware Graphics Acceleration.
  
The `Default Graphic Plattform (ELGFS)` entry is important to check, if its not set, the ATC_UI application will not start. The setting sets a environment variable called `QT_QPA_PLATFORM`, which tells the QT5 application to use the ELGFS backend.

#### SSH

In the case of the ATC_OS we need a ssh server to connect to the target via ethernet. This allows the remote debugging and development of the software. Most IDE support the deployment of the build software over ssh to a target system. So a ssh and sftp server is essential for development.

Buildroot provides the SSH and SFTP server in seperate packages:

* `Target Packages -> Networking Applications -> gsftpserver`, enable the SFTP Server, for file transfers.
* `Target Packages -> Networking Applications -> dropbear`, enable the SSH Server, for a secure shell

A NTP client will also be installed, so the target system can fetch the correct time over a NTP Server.
This is needed for a successful SSH HTTP Request of ATC_Controller application to the chess ATC_Server.
With incorrect time setting, the Client/Server is not able to verify a SSL Certificate.

#### MISC PACKAGES

In the development phase its useful to install some other software packages to the target system:

* `Target Packages -> Text editors -> nano`, install a simple text editor
* `Target Packages -> Networking Applications -> rsync`, proves simple file sync (also used by QT if avariable).
* `Target Packages -> System Tools -> htop`, a better taskmanager as the top application

There are some more applications and libraries for development and testing installed, but the system is working with these mentioned above.

### FILE SYSTEM SIZE

![make menuconfig](./documentation_images/buildroot_file_system.png)

The QT libraries and the other needed libraries are quite large in size. For the full QT5 with QT3D and the Virtual Keyboard and the nedded QuickControls II the rootfs file system is about 230MB. The default setting is 128MB. So buildroot can not build the final image beacuas its not enought space. To increase the rootfs maximum size, buildroot provides a size option under `Filesystem images -> root filesystem -> exact size`. With the `Space-Key` it is possible to edit the default value to 512M. A Zero for automatic determination is not supported.

The `Fileystem images` category, provides several other options for the rootfs. Its also possible to create a readonly filesystem or a compressed one. For debugging purposes, the readonly filesystem option is disabled. All other settings are the default settings.

After building a complete image the size is about 310MB in Size, this includes the bootloader, os with all libraries and packages with the ATC needed programs (ATC_UI, ATC_Client).

### FILE SYSTEM OVERLAY
* general function and usage
* usage in buildroot to modify the final filesystem
* concrete usage here, with ssh keys
?? IMAGE OF THE OVERLAY DIAGRAM ??


#### AUTOBOOT
*initrd file
#### SSH KNOWN HOSTS
* ssh file


## HOW TO FINALLY BUILD

Now everything is setup, so its possible to build the image. From a fresh buildroot installation it can take serveral hours to build the image. Buildroot download the sourcefiles only once to the dowload folder `./dl`. The generated output files can be found in the output directory `./output`. This folder contains several subfolder with the build results.

The `$ make` command starts the build process.
On the development pc (Ubuntu 20.04, IntelCore i9 with 24GB RAM), the a fresh build took about 2h43m.

#### CAUTION
Editing some settings can require a complete requild, for example `Kernel` and  `Target Options` settings.
To cleanup the buildroot cache and output folders, run `$ make clean`

After some time, the process output should end with `INFO: adding roofs partition; INFO: writing MBR`, so everything is finished.
The result can be found in the output directory:

![OUTBUT_FOLDERS](./documentation_images/buildroot_output.png)

* `build`, contains the compiled libraries and other compile software
* `host`, contains the build software for the host, including the cross compile toolchain
* `images`, contains the final sd card image
* `target`, this is a copy of the rootfs of the target device

#### TARGET BUILD RESULT

The build image can be found in `./output/images/sdcard.img`. This image already contains the root filesystem, the boot partition with bootloader and firmware espacially for the RPI.

It can be flashed using the linux `dd` command
`$ dd bs=4M if=./sdcard.img of=/dev/<SD_CARD> conv=fsync`
or by using a gui based utility like [Win32DiskImager](https://github.com/znone/Win32DiskImager) or the [Etcher](https://www.balena.io/etcher/).

After flashing the image to an sd card, the device is ready for booting.
If the device boot sequence finsihed, its possible to connect with the taerget board over SSH with the in the `System-Settings` Chapter given settings.
On the development pc its also possible to connect without a user password. This is possible though host public key signature the `/root/.ssh/known_hosts` file, given though the `FILE SYSTEM OVERLAY` system in the build process.

#### HOST CROSS COMPILER

The crosscompiler (gcc,g++,gdb and qt stuff) can be found in the sysroot directory `./output/host/bin/`

* G++ `arm-buildroot-linux-gnueabihf-g++`
* GCC `arm-buildroot-linux-gnueabihf-gcc`
* GDB `arm-buildroot-linux-gnueabihf-gdb`
* CMAKE `cmake`
* QT_QMAKE `qmake`

The sysroot of the target system (needed for the QT Kit Sysroot Setting) is located in the  `./output/host/arm-buildroot-linux-gnueabihf/sysroot` Folder.

#### BUILD A SINGLE PACKAGE

For building a single package, for example to debug the buildprocess if a new created package, buildroot provedes a make command.

`$ make <PACKAGE_NAME>-rebuild`

The package name can be found in the package folder of buildroot `./packages`.
A rebuild does not downloads the source again if a fixed commit id is present in the `.mk` File of the package.
Instead the already downloaded source in `./buildroot/dl` will be used.

## ADDING OWN PACKAGES

This chapter only explains the basics about creating packages. Buildroot has a very powerful packages system, which is not needed for this project.
Here also the system dependencies are not considered.

After building the first image successfully, our goal is to add custom software packages to the buildroot configuration.
All avariable pacakges definitions are located in the package directory `./packages`.
Each package is located in a seperate folder inside the `./packages` folder.
The folder is called like the package name.

The package folder contains at least two files:

* `<PACKAGE_NAME>.mk`
* `Config.in`

The `Config.in` file contains the information, that are visible on the configuration menu `make menuconfig`.

Listed below is the simplest package information, which the `Config.in` can contains:

```bash

config BR2_PACKAGE_PACKAGENAME
    bool "PACKAGENAME"
    help
        INFORMATION ABOUT THE PACKAGE

```

The first line is the start configuration for the specific package, Important is `PACKAGENAME` at the end of the line.
Followed by the bool option. This option is visible under the buildroot configuration menu.
The `help` section, can contains several information about the author / functionallity of the package.

![BUILDROOT_PACKAGE_1](./documentation_images/buildroot_packages_1.png)

The `<PACKAGENAME>.mk` is the makefile or build reciepe for the package.
The file contains the build and install instruction for the package.

![BUILDROOT_PACKAGE_2](./documentation_images/buildroot_package_2.png)


The first information, is the location of the sourcefiles of the package.
Buildroot always downloads the package sources from an other location.


Its possible to store the sources local but its not recommended, so its not explained here.
The source files of the custom packages are stored in a git repository.


* `<PACKAGESNAME>_VERSION = origin/master`, is the commitid or branch of the git repository.
* `<PACKAGESNAME>_SITE = git@github.com:RBEGamer/AtomicChessOS.git`, is the git server url
* `<PACKAGESNAME>_SITE_METHOD = git`, use git.

It is also possible to download the source as a zip or tarball archive from a webserver.
In this case the `<PACKAGESNAME>_SITE` key is the url without the filename and the `<PACKAGESNAME>_VERSION` the filename.

Next step is the definition of the build steps.

The `define <PACKAGENAME>_BUILD_CMDS` starts a block of shell commands, which will executed during the build phase.




* how to create a own packes
* which packaes are needed for the ATC OS
* which file needs to be created and where (screenshot)
* screenshot new meu items




## PREPERATION WORK FOR CI

At this point the whole buildroot setup is working on our development machine.
The final goal of the installation is, to automate the build process further more using a continous integtation (CI) system like jenkins.
To make the integration into the CI system easier, a single bash file was created to handle all for a build nessessary commands.
The file is placed in the buildroot-root directory `./build.sh` and invokes the make comand in order to run a buildroot build.

On step is important before make can be called. After a fresh download of the buildroot directory from the git server, it is not possible to directy call make. The `.config` file will not be synchronizes by the `.gitignore` file, so the file is missing in the buildroot-root directory.
To solve this issue, the buildroot configuration is stored under a different file name `./config_backup`. The `build.sh` file renames the `config_backup` to `.config`, and invoke the make command sucessfully.

Later the CI system simply have to call the `build.sh` file in order to start the build process.

In addition, later its also possible to add new features to the `build.sh`. For example to upload the final image to an FTP Server.




## CONCLUSION BUILDROOT
The configuration of the buildroot system was a bit difficult. 
There are a few dependencies eg for the touchscreen which have to be checked manually.
The goal of the buildroot system for the ATC Project is, to build a working/ ready to use image, which contains all needed software to drive a ATC Table.

For this purpose, three custom buildroot packages were created, to intregrate the needed ATC Software into the final SD-Card image.
The final SD Card image, can directly flashed to the RaspberryPi and the build cross plattform compiler toolchain can be used to develop software on the host computer and run it on the target hardware.

This setup was used in the whole software development process. Especially for the gui application. QT5 needs this cross compile toolchain to compile the application successful.


In summary buildroot was a great choice to work with. The process of integration of own packages was straight forward. In the end building an linux image for an embedded device that simply works, including all needed software for the project is very great.
This system also has advantages as not every image has to be adjusted manually, especially if the number of embedded systems to be installed increases.






