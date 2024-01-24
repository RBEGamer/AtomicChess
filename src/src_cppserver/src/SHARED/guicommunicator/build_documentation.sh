#!/bin/bash

mkdir -p ./documentation

# BUILD POSTMAN REST API DOCUMENTATION
docgen build -i ./REST_REQUST_LIST.postman_collection.json -o ./documentation/guicommunicator.html
docgen build -i ./REST_REQUST_LIST.postman_collection.json -m -o ./documentation/guicommunicator.md

#BUILD SOURCE DOCUMENTATION USING DOXYGEN
doxygen ./Doxyfile
