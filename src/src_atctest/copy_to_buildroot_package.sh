#!/bin/bash
echo "COPY SHARED FOLDER FROM ../src_shared"
cp -Rf ../src_shared/SHARED ./src/
echo "COPY TO BUILDROOT PACKAGE"
pwd
ls
echo "COPY ./src TO ../src_buildroot/buildroot/ATC_PACKAGES/atctp"
rm -Rf ../src_buildroot/buildroot/ATC_PACKAGES/atctp/src || true
cp -rf ./src ../src_buildroot/buildroot/ATC_PACKAGES/atctp