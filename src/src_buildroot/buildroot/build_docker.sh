#!/bin/bash
bash ./restore_config.sh
# CREATE FOLDERS
#rm -Rf ./output

# BUILD CONTAINER

if [[ "$(docker images -q atcbuildroot:lates 2> /dev/null)" == "" ]]; then
  # do something
  echo "atcbuildroot:lates IMAGE EXISTS; NO BUILD REQUIRED"
else
 echo "atcbuildroot:lates IMAGE BUILD STARTED"
 docker build -t atcbuildroot:latest .
fi


# COPY FILES OVER TO HOST USING SCIRPT INSIDE OF THE CONTAINER
docker run -i --rm -v "$(pwd)":/var/buildroot atcbuildroot

