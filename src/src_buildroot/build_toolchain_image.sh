#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# COPY PACKAGE SOURCES
cd ../src_atctest && bash copy_to_buildroot_package.sh && cd ../src_buildroot
cd ../src_atcqtui && bash copy_to_buildroot_package.sh && cd ../src_buildroot
cd ../src_controller && bash copy_to_buildroot_package.sh && cd ../src_buildroot


cd ./buildroot && bash ./build_docker.sh && cd ../