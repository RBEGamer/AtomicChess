#!/bin/bash

echo "--- COPY sw-description to ../output/images/ ---"
cp ./sw-description ../output/images
echo "--- RUN PACKAGE to .swu in ../output/images/ ---"
cd ../output/images
echo ${pwd}
CONTAINER_VER="1.0.2"
PRODUCT_NAME="atctable"
FILES="sw-description rootfs.ext2"

for i in $FILES;do
	echo $i;done | cpio -ov -H crc > ${PRODUCT_NAME}_${CONTAINER_VER}.swu
