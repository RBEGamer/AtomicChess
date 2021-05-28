ls
cd ../
STARTPATH=$(pwd)

# BUILD DOCKER COMPOSE STACK
cd $STARTPATH/src_server/CI/
bash ./start_stack_rpi3.sh
# CLEANUP OLD IMAGES
#docker rm -v $(docker ps -a -q -f status=exited);
#docker volume rm $(docker volume ls -q -f dangling=true);
#docker rmi -f $(docker images -f "dangling=true" -q)


#BUILD ATC GUI
cd $STARTPATH/src_atcqtui/src
qmake .
make
# COPY BINARY TO ATC LOCATION
FILE=./atcgui
if test -f "$FILE"; then
    echo "$FILE exists."
    mkdir -p /usr/ATC
    cp ./atcgui /usr/ATC/atc_ui
fi


#BUILD ATC CONTROLLER
cd $STARTPATH/src_controller/controller/controller
rm -f ./CMakeCache.txt 
rm -Rf ./CMakeFiles

# USE DIFFEREN GCC 10.x ON PI
export CXX=/usr/local/bin/arm-linux-gnueabihf-g++
export CC=/usr/local/bin/arm-linux-gnueabihf-gcc
cmake .
make
# COPY BINARY TO ATC LOCATION
FILE=./atc_controller
if test -f "$FILE"; then
    echo "$FILE exists."
    mkdir -p /usr/ATC
    cp ./atc_controller /usr/ATC/atc_controller
fi



