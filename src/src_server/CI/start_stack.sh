#!/bin/bash
clear
docker-compose build --no-cache
docker-compose down -v

docker-compose up -d AtomicChessMongoDatabase AtomicChessRedisDatabase AtomicChessMoveValidator AtomicChessExtentionVoiceRecognition

sleep 30s
docker-compose up -d AtomicChessBackend
sleep 30s
docker-compose up -d AtomicChessAutoPlayer
