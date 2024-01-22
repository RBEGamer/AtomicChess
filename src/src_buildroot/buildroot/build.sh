#!/bin/bash

pwd
ls -la
env
#cd /var/buildroot
ls -la



# CHECK IF ALREADY BUILD
# NEEDED IN DOCKER CONTAINER
FILE=./output/BUILD_DOCKER_SUCC
if test -f "$FILE"; then
    echo "$FILE exists."
    exit -1
else
    echo "$FILE does not exists."
    echo "--- STARTING BUILD ---"
fi








echo "-- COPY CONFIG FILE --"
bash ./restore_config.sh

echo "-- INITIAL BUILD TO CREATE HOST AND TARGET FS --"
make -j10



echo "-- COPY PACKAGES OVER --"
# COPY CUSTOM PACKAGES OVER
for f in ATC_PACKAGES/*; do
    if [ -d "$f" ]; then
        # Will not run if no directories are available
        echo "$f"
        
        # DELETE OLD PACKAE FILES IN PACKAGE FOLDER
        echo "./package/$(basename $f)"
        rm -R "./package/$(basename $f)" || true
        
        # COPY NEW CONTENT OVER
        # USE RSYNC ?
        cp -Rf "$f" "./package/$(basename $f)"
        
        # FORCE TO BUILD THE ATC PACKAGES
        rm -Rf "./dl/$(basename $f)/" || true

        make "$(basename $f)-dirclean"
    fi
done






# INCREMENT VERSION
cd ./VERSIONING && bash ./increment_version.sh && cd ..
# FORCE TO BUILD THE ATC PACKAGES
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
make -j8



echo "--CREATING UPDATE FILE FOR SWUPDATE using script in ./SWUPDATE--"
cd ./SWUPDATE
bash ./swupdate_packer.sh



echo "-- BUILD FINISHED --"
# TODO ADD SECONDS .config only for base sytem

# CREATE FOLDERS
#mkdir -p /var/build_result/images
#mkdir -p /var/build_result/host
# COPY BUILD RESULT TO HOST
#cp -R /var/buildroot/output/images /var/build_result
#cp -R /var/buildroot/output/host /var/build_result
#cp -R /var/buildroot/output/target /var/build_result

#chmod -R 777 ./output

#touch ./output/BUILD_DOCKER_SUCC
exit 0
