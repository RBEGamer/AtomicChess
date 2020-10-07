#!/bin/bash


echo "--- READ VERSION ---"
SWVERSION="$(cat ./VERSION)"



echo "--- GENERATE SHA256 from rootfs.ext2 ---"
RFSHASHA=$(sha256sum ../output/images/rootfs.ext2.gz | awk '{ print $1 }')
echo "rootfs hash ${RFSHASHA}"

echo "--- PATCH sw-description from sw-description_template"
cp -rf ./sw-description_template ./sw-description
sed -i 's|RFSHASH|'"$RFSHASHA"'|g' ./sw-description
sed -i 's|VERSION|'"$SWVERSION"'|g' ./sw-description
cat ./sw-description



echo "--- SIGN sw-description ---"
openssl dgst -sha256 -sign ./swupdatekey_private.pem ./sw-description > ./sw-description.sig



echo "--- COPY sw-description to ../output/images/ ---"
cp ./sw-description.sig ../output/images/
cp ./sw-description ../output/images/
echo "--- RUN PACKAGE to .swu in ../output/images/ ---"


cd ../output/images


echo ${pwd}
CONTAINER_VER="${SWVERSION}"
PRODUCT_NAME="atctable"
FILES="sw-description sw-description.sig rootfs.ext2.gz"

for i in $FILES;do
	echo $i;done | cpio -ov -H crc > ${PRODUCT_NAME}_${CONTAINER_VER}_signed.swu
ls

