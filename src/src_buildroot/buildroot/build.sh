#!/bin/bash
# build.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
echo "--- STARTING BUILD ---"
pwd
ls
env


echo "--- GENERATE KEYS FOR SWUPDATE---"
FILEA=./OVERLAY_FS/etc/swupdate/swupdatekey_public.pem
if test -f "$FILEA"; then
   	echo "--KEYS EYISTS --"
else	
	echo "--REGENERATE KEYS --"
 	openssl genrsa -out ./SWUPDATE/swupdatekey_private.pem
   	openssl rsa -in ./SWUPDATE/swupdatekey_private.pem -out ./SWUPDATE/swupdatekey_public.pem -outform PEM -pubout
   	cp ./SWUPDATE/swupdatekey_public.pem ./OVERLAY_FS/etc/swupdate/swupdatekey_public.pem
fi



# INCREMENT VERSION
cd ./VERSIONING && ./increment_version.sh && cd ..




FILEA=./DELETE_FOR_REBUILD
if test -f "$FILEA"; then
    echo "$FILEA exists."
    # make clean stuff
    
    #rm -Rf ./output/images
    #make raspberrypi_defconfig
else
    echo "--- PERFORM A CLEAN BUILD ---"
    rm -Rf ./output/build/.root
    rm -Rf ./output/images
    make clean
    touch ./DELETE_FOR_REBUILD
fi



# make distclean
echo "-- COPY CONFIG FILE --"
cp ./config_backup ./.config



# FORCE TO BUILD THE ATC PACKAGES
make atcgui-dirclean && rm -Rf ./dl/atcgui/
make atcctl-dirclean && rm -Rf ./dl/atcctl/
make atctp-dirclean && rm -Rf ./dl/atctp/

echo "--BUILD FIRT ITERATION --"

make

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
make



echo "--CREATING UPDATE FILE FOR SWUPDATE using script in ./SWUPDATE--"
cd ./SWUPDATE
bash ./swupdate_packer.sh

