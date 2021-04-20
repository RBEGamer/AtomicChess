#!/bin/bash

if [[ "$(docker images -q atcthesis:latest 2> /dev/null)" == "" ]]; then
  echo "atcthesis:latest IMAGE BUILD STARTED"
  docker build -t atcthesis:latest .
else
 echo "atcthesis:latest IMAGE EXISTS; NO BUILD REQUIRED"
fi


docker run -i --rm -v "$(pwd)":/var/thesis atcthesis
