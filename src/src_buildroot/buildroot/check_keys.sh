#!/bin/bash
echo "--- GENERATE KEYS FOR SWUPDATE---"
FILEA=./OVERLAY_FS/etc/swupdate/swupdatekey_public.pem
if test -f "$FILEA"; then
   	echo "--KEYS EYISTS --"
else	
	echo "--REGENERATE KEYS --"
 	openssl genrsa -out ./SWUPDATE/swupdatekey_private.pem
   	openssl rsa -in ./SWUPDATE/swupdatekey_private.pem -out ./SWUPDATE/swupdatekey_public.pem -outform PEM -pubout
   	cp ./SWUPDATE/swupdatekey_public.pem ./OVERLAY_FS/etc/swupdate/swupdatekey_public.pem
    cp ./SWUPDATE/swupdatekey_public.pem ./output/images/swupdatekey_public.pem
fi