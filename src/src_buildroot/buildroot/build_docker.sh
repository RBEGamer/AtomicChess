#!/bin/bash
bash ./restore_config.sh
# CREATE FOLDERS
rm -Rf ./output

# BUILD CONTAINER
docker build -t atcbuildroot .

# COPY FILES OVER TO HOST USING SCIRPT INSIDE OF THE CONTAINER
docker run -it --rm -v "$(pwd)":/var/buildroot atcbuildroot

