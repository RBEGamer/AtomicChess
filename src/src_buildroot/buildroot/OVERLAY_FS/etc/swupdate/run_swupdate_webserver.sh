#!/bin/sh
# /etc/swupdate/run_swupdate_webserver.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
echo "----  STARTING SWUPDATE WEBSERVER ON PORT 8081 ----"
swupdate -v -f /etc/swupdate/swupdate.cfg -k /etc/swupdate/swupdatekey_public.pem -w "--document-root /var/www/swupdate --port 8081"
