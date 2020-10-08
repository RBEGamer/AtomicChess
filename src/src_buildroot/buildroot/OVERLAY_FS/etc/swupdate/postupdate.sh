#!/bin/sh
# /etc/swupdate/postupdate.sh ATCProject Marcel Ochsendorf marcel.ochsendorf.com
echo "-- POSTUPDATE SWUPODATE ---"
# MOUNT CURRENT ROOTFS AS READ ONLY = SINGLE USER MODE
#mount -o remount,ro /
# CHECK FILESYSTEM BLOCKS
#e2fsck -fy /dev/mmcblk0p2
# REBOOT SYSTEM
sleep 10
#echo 1 > /proc/sys/kernel/sysrq
#echo b > /proc/sysrq-trigger

reboot
