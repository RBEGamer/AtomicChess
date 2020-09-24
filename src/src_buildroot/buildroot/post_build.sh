#!bin/bash
ls
cd $(pwd)/output/
ls


scp -r ./images/sdcard.img prodevmo@127.0.0.1:/home/prodevmo/Desktop
