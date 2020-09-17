# AtomicChessOS

## ATC Server

This branch contains the main server application for the ATC System.
The server contains several microservices.


### microservices
* ATC_MoveValidator - Provies chess board verification and verification of chess moves
* ATC_Backend - Main System Backend to connect the chess-table, and provides the main game and matchmaking logic
* ATC_AutoPlayerSpawner - Spawns Autoplayer containers if not enought AI Player are variable
* ATC_AutoPlayer - A AI player that logins as a normal player, and is avariable for an automatch

## ADDITIONAL NEEDED FILES
* CI - Holds a docker-compose file to setup a complete system and basic Jenkins configuration files
* API_Documentation - Documentation about the REST API ROUTES

Every service is running in a docker container for easy deployment.

## GENERAL OVERVIEW DIAGRAM
## DESCIBE EVERY SERVICE
### MAIN TASK; COMMINUCATION WITH THE OTHER SERVICES
### FOR BACKEND FUNKTION DIAGRAM

## BASIC ABLAUF DIAGRAMM FOR TWO PLAYERS

## HOW TO RUN EVERYTHING
