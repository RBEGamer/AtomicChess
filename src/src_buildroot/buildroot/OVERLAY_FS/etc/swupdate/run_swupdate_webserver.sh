#!/bin/sh
echo "----  STARTING SWUPDATE WEBSERVER ON PORT 8081 ----"
swupdate -v -f /etc/swupdate/swupdate.cfg -k /etc/swupdate/swupdatekey_public.pem -w "--document-root /var/www/swupdate --port 8081"
