#!/usr/bin/bash
# ATC Project Marcel Ochsendorf marcelochsendorf.com
# This script increments a versionnumber before each build
# its normally called by the ../build.sh script

python3 ./increment_version.py


cp ./VERSION ../OVERLAY_FS/VERSION
cp ./VERSION ../SWUPDATE/VERSION
cp ./VERSION ../output/images

TIMESTAMP=`date +%Y-%m-%d_%H-%M-%S`
echo "${TIMESTAMP}" > ./BUILD_DATE
cp ./BUILD_DATE ../OVERLAY_FS/BUILD_DATE
cp ./BUILD_DATE ../SWUPDATE/BUILD_DATE
cp ./BUILD_DATE ../output/images
