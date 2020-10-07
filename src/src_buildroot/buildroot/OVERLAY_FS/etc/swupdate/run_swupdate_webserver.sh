#!/bin/sh
echo "----  STARTING SWUPDATE WEBSERVER ON PORT 8081 ----"
swupdate -v -k /etc/swupdate/swupdatekey_public.pem -w "--document-root /var/www/swupdate --port 8081"
