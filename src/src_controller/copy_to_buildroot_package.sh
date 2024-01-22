#!/bin/bash
echo "COPY SHARED FOLDER FROM ../src_shared"
cp -Rf ../src_shared/SHARED ./controller/controller
echo "COPY TO BUILDROOT PACKAGE"
pwd
ls
echo "COPY ./controller/controller TO ../src_buildroot/buildroot/ATC_PACKAGES/atcctl"
cp -rf ./controller/controller ../src_buildroot/buildroot/ATC_PACKAGES/atcctl

# ADDITIONAL RENAME STEP
rm -Rf ../src_buildroot/buildroot/ATC_PACKAGES/atcctl/src || true
mv ../src_buildroot/buildroot/ATC_PACKAGES/atcctl/controller ../src_buildroot/buildroot/ATC_PACKAGES/atcctl/src