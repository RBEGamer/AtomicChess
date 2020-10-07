#!/bin/sh
echo "----  STARTING SWUPDATE HAWKBIT ----"
swupdate -v -f /etc/swupdate/swupdate.cfg -u "${SURICATTA_ARGS}"

