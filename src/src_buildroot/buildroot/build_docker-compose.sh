#!/bin/bash

# CREATE FOLDERS
rm -Rf ./build_result
mkdir -p ./build_result

docker-compose up atcbuildroot
 
