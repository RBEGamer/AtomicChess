#!/bin/bash
echo "COPY SHARED FOLDER FROM ../src_shared"
cp -Rf ../src_shared/SHARED ./src/controller/controller
echo "COPY TO BUILDROOT PACKAGE"
pwd
ls
echo "COPY ./controller/controller TO ../src_buildroot/buildroot/package/atcctl"
cp -rf ./controller/controller ../src_buildroot/buildroot/package/atcctl

# ADDITIONAL RENAME STEP
rm -Rf ../src_buildroot/buildroot/package/atcctl/src
mv ../src_buildroot/buildroot/package/atcctl/controller ../src_buildroot/buildroot/package/atcctl/src