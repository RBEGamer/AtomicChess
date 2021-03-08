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


# UFW FIREWALL SETUP

* deny all incomming except:
* local communication active
```
root@h2914172:~# sudo ufw status numbered
Status: Aktiv

     Zu                         Aktion      Von
     --                         ------      ---
[ 1] Anywhere                   ALLOW IN    127.0.0.1                 
[ 2] 22/tcp                     ALLOW IN    Anywhere                  
[ 3] 22                         ALLOW IN    Anywhere                  
[ 4] 3000                       ALLOW IN    Anywhere                  
[ 5] 5001                       ALLOW IN    Anywhere                  
[ 6] 22/tcp (v6)                ALLOW IN    Anywhere (v6)             
[ 7] 22 (v6)                    ALLOW IN    Anywhere (v6)             
[ 8] 3000 (v6)                  ALLOW IN    Anywhere (v6)             
[ 9] 5001 (v6)                  ALLOW IN    Anywhere (v6) 
```

* ALLOW DOCKER INTERFACE 
```
$ iptables -t nat -A POSTROUTING ! -o docker0 -s 172.17.0.0/16 -j MASQUERADE
$ sed -i -e 's/DEFAULT_FORWARD_POLICY="DROP"/DEFAULT_FORWARD_POLICY="ACCEPT"/g' /etc/default/ufw
```
