#!/bin/bash
echo "--- GENERATE KEYS FOR SWUPDATE---"
openssl genrsa -out ./swupdatekey_private.pem
openssl rsa -in ./swupdatekey_private.pem -out ./swupdatekey_public.pem -outform PEM -pubout


cp ./swupdatekey_public.pem ../OVERLAY_FS/etc/swupdatekey_public.pem
