#!/bin/bash
clear
docker-compose down
docker-compose build --no-cache

docker-compose up -d AtomicChessMongoDatabase AtomicChessRedisDatabase AtomicChessMoveValidator redis-commander

sleep 10s
docker-compose up -d AtomicChessBackend
sleep 10s
docker-compose up -d AtomicChessAutoPlayer