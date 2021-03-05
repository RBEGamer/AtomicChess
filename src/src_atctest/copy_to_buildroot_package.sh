#!/bin/bash
echo "COPY TO BUILDROOT PACKAGE"
pwd
ls
echo "COPY ./src TO ../src_buildroot/buildroot/package/atctp"
cp -rf ./src ../src_buildroot/buildroot/package/atctp