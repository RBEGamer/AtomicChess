#!/bin/bash

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit -1
fi

# COPY PACKAGE SOURCES
bash ./copy_local_packages.sh
cd ./buildroot && bash ./build_docker.sh && cd ../
