#!/bin/bash
clear
docker-compose -f ./docker-compose_rpi3.yml build --no-cache
docker-compose -f ./docker-compose_rpi3.yml down -v

docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessMongoDatabase AtomicChessRedisDatabase AtomicChessMoveValidator redis-commander AtomicChessExtentionVoiceRecognition

sleep 30s
docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessBackend
sleep 30s
docker-compose -f ./docker-compose_rpi3.yml up -d AtomicChessAutoPlayer
