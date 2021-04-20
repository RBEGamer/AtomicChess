#!/bin/bash
bash ./restore_config.sh
# CREATE FOLDERS
#rm -Rf ./output

# BUILD CONTAINER

if [[ "$(docker images -q atcbuildroot:latest 2> /dev/null)" == "" ]]; then
 echo "atcbuildroot:latest IMAGE BUILD STARTED"
 docker build -t atcbuildroot:latest .
else
 echo "atcbuildroot:latest IMAGE EXISTS; NO BUILD REQUIRED"
fi


# COPY FILES OVER TO HOST USING SCIRPT INSIDE OF THE CONTAINER
docker run -i --rm -v "$(pwd)":/var/buildroot atcbuildroot

