#!/bin/bash

cd ../src_atctest && bash copy_to_buildroot_package.sh && cd ../src_buildroot
cd ../src_atcqtui && bash copy_to_buildroot_package.sh && cd ../src_buildroot
cd ../src_controller && bash copy_to_buildroot_package.sh && cd ../src_buildroot


cd ./buildroot && ./build_docker.sh
