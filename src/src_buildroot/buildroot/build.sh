#!/bin/bash
echo "--- STARTING BUILD ---"
pwd
ls
env


# make distclean
echo "-- COPY CONFIG FILE --"
cp ./config_backup ./.config



FILEA=./DELETE_FOR_REBUILD
if test -f "$FILEA"; then
    echo "$FILEA exists."
    # make clean stuff
    rm -Rf ./output/build/.root
else
    make clean
    touch ./DELETE_FOR_REBUILD
fi

make atcgui-dirclean && rm -Rf ./dl/atcgui/
make atcctl-dirclean && rm -Rf ./dl/atcctl/
make atctp-dirclean && rm -Rf ./dl/atctp/
make atrpc-dirclean && rm -Rf ./dl/atcrpc/

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
    cat ./output/images/rpi-firmware/config.txt
    touch ./output/images/rpi-firmware/PATCHED
fi

echo "-- PATCH config.txt --"



#dtoverlay=i2c-rtc,ds3231



echo "--BUILD FINAL WITH PATCHED config.txt--"
make
