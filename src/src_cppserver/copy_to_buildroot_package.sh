#!/bin/bash

rm -Rf ../src_buildroot/buildroot/ATC_PACKAGES/atcsrv/src || true
cp -Rf ./src ../src_buildroot/buildroot/ATC_PACKAGES/atcsrv/src
rm -Rf ../src_buildroot/buildroot/ATC_PACKAGES/atcsrv/src/*.a