#!/bin/bash
bash ./restore_config.sh
# CREATE FOLDERS
rm -Rf ./build_result
mkdir -p ./build_result

# BUILD CONTAINER
docker build -t atcbuildroot .

# COPY FILES OVER TO HOST USING SCIRPT INSIDE OF THE CONTAINER
docker run -it --rm -v "$(pwd)/build_result":/var/build_result atcbuildroot

