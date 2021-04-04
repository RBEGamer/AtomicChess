#!/bin/bash

docker build -t atccontroller:latest .
docker run -it atccontroller:latest