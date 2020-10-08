#!/bin/sh
# /etc/swupdate/run_swupdate_hawkbit.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
echo "----  STARTING SWUPDATE HAWKBIT ----"
swupdate -v -f /etc/swupdate/swupdate.cfg -u "${SURICATTA_ARGS}"

