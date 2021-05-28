#!/bin/bash
clear
export ARCH=RPI3

docker-compose -f ./docker-compose_rpi3.yml build
docker-compose -f ./docker-compose_rpi3.yml down

docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessMongoDatabase AtomicChessRedisDatabase #AtomicChessMoveValidator redis-commander AtomicChessExtentionVoiceRecognition

sleep 20s
#docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessBackend
#sleep 30s
#docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessAutoPlayer
