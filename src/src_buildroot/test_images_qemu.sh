#!/bin/bash

#install qemau
#sudo apt-get install qemu-system

# SHOW IMAGE INFO
qemu-img info ./buildroot/output/images/sdcard.img 
# CONVERT IMAGE FROM RAW TO QCOW2 IMAGE
qemu-img convert -f raw -O qcow2 ./buildroot/output/images/sdcard.img ./buildroot/output/images/sdcard.qcow
# RUN QEMU
qemu-system-aarch64 -m 1024 -M raspi3 \
    -kernel qemu-rpi-kernel-master/kernel-qemu-5.4.51-buster \
    -dtb ./buildroot/output/images/bcm2710-rpi-3-b-plus.dtb \
    -sd ./buildroot/output/images/sdcard.qcow \
    -serial stdio
    #-device "drive=disk0,disable-modern=on,disable-legacy=off" \
    -net "user,hostfwd=tcp::5022-:22" \
    #-append 'root=/dev/vda2 panic=1' \
    -append 'console=ttyS0' \
    -nographic \
    -serial telnet:localhost:4321,server,nowait
    #-no-reboot