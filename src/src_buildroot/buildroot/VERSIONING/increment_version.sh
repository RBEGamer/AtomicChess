#!/usr/bin/bash
# ATC Project Marcel Ochsendorf marcelochsendorf.com
# This script increments a versionnumber before each build
# its normally called by the ../build.sh script

python ./increment_version.py


cp ./VERSION ../OVERLAY_FS/VERSION
cp ./VERSION ../SWUPDATE/VERSION
cp ./VERSION ../output/images
