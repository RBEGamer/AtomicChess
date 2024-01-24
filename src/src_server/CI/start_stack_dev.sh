#!/bin/bash
clear
docker-compose -f ./docker-compose_dev.yml build --no-cache
docker-compose -f ./docker-compose_dev.yml down -v

docker-compose -f ./docker-compose_dev.yml up -d AtomicChessMongoDatabase AtomicChessRedisDatabase AtomicChessMoveValidator
# AtomicChessExtentionVoiceRecognition
#docker-compose up -d AtomicChessAutoPlayer
