#!/bin/bash

pwd
ls -la
env
cd /var/buildroot
ls -la


# CHECK IF ALREADY BUILD
# NEEDED IN DOCKER CONTAINER
FILE=./BUILD_DOCKER_SUCC
if test -f "$FILE"; then
    echo "$FILE exists."
    exit -1
else
    echo "$FILE does not exists."
    echo "--- STARTING BUILD ---"
fi



# INCREMENT VERSION
cd ./VERSIONING && bash ./increment_version.sh && cd ..


 

echo "-- COPY CONFIG FILE --"
bash ./restore_config.sh



# FORCE TO BUILD THE ATC PACKAGES
make atcgui-dirclean && rm -Rf ./dl/atcgui/
make atcctl-dirclean && rm -Rf ./dl/atcctl/
make atctp-dirclean && rm -Rf ./dl/atctp/

echo "--BUILD FIRT ITERATION --"

make -j10

FILE=./output/images/rpi-firmware/PATCHED
if test -f "$FILE"; then
    echo "$FILE exists."
else
    echo "dtparam=i2c=on" >> ./output/images/rpi-firmware/config.txt
    echo 'dtparam=i2c1=on' >> ./output/images/rpi-firmware/config.txt
    echo 'dtparam=i2c_arm=on' >> ./output/images/rpi-firmware/config.txt
    echo 'dtparam=spi=on' >> ./output/images/rpi-firmware/config.txt
    echo 'dtparam=i2s=on' >> ./output/images/rpi-firmware/config.txt
    echo 'dtparam=audio=on' >> ./output/images/rpi-firmware/config.txt
    echo 'dtoverlay=hifiberry-dac' >> ./output/images/rpi-firmware/config.txt
    echo 'dtoverlay=i2s-mmap' >> ./output/images/rpi-firmware/config.txt
    
    
    echo 'dtoverlay=spi1-1cs' >> ./output/images/rpi-firmware/config.txt  #1 chip select
    echo 'dtoverlay=spi1-2cs' >> ./output/images/rpi-firmware/config.txt  #2 chip select
    
    cat ./output/images/rpi-firmware/config.txt
    
    # PATCH CMDLINE.txt
    echo ' fsck.repair=yes ' >> ./output/images/rpi-firmware/cmdline.txt #FILE SYSTEM REPAIR
    echo ' rootfstype=ext2 ' >> ./output/images/rpi-firmware/cmdline.txt #FILE SYSTEM REPAIR
    
    cat ./output/images/rpi-firmware/cmdline.txt
    
    touch ./output/images/rpi-firmware/PATCHED
fi

echo "-- PATCH config.txt --"



#dtoverlay=i2c-rtc,ds3231



echo "--BUILD FINAL WITH PATCHED config.txt--"
make -j10



echo "--CREATING UPDATE FILE FOR SWUPDATE using script in ./SWUPDATE--"
cd ./SWUPDATE
bash ./swupdate_packer.sh



echo "-- BUILD FINISHED --"
# TODO ADD SECONDS .config only for base sytem
bash /var/buildroot/build.sh
# CREATE FOLDERS
mkdir -p /var/build_result/images
mkdir -p /var/build_result/host
# COPY BUILD RESULT TO HOST
cp -R /var/buildroot/output/images /var/build_result/images
cp -R /var/buildroot/output/host /var/build_result/host

touch ./BUILD_DOCKER_SUCC
exit 0