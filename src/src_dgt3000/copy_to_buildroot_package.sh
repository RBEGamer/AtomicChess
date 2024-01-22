#!/bin/bash


echo "COPY TO BUILDROOT PACKAGE"
pwd
ls
echo "COPY ./src TO ../src_buildroot/buildroot/ATC_PACKAGES/atcdgt3000"
rm -Rf ../src_buildroot/buildroot/ATC_PACKAGES/atcdgt3000/src || true
cp -rf ./src ../src_buildroot/buildroot/ATC_PACKAGES/atcdgt3000
