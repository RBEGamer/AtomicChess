#!/bin/bash

curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
cp ./arduino-cli /usr/bin
arduino-cli core update-index
