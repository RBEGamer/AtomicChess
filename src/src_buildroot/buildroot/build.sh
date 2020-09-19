#!/bin/bash
echo "--- STARTING BUILD ---"
pwd
ls
env

echo "-- COPY CONFIG FILE --"
cp ./config_backup ./.config

echo "------ RUN MAKE ------"
make


echo "- CHECK BUILD OUTPUT -"




echo "-- UPLOAD ARTEFACTS --"


