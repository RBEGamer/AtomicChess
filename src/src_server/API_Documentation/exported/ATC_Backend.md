
# AtomicChessBackend



## Indices

* [Ungrouped](#ungrouped)

  * [CLIENT_STATUS](#1-client_status)
  * [GET_AI_PLAYER_COUNT](#2-get_ai_player_count)
  * [GET_PLAYERS_AVARIABLE](#3-get_players_avariable)
  * [GET_PLAYER_STATE](#4-get_player_state)
  * [HEARTBEAT](#5-heartbeat)
  * [LOGIN](#6-login)
  * [LOGOUT](#7-logout)
  * [MAKE_MOVE](#8-make_move)
  * [PLAYER_SETUP_CONFIRMATION](#9-player_setup_confirmation)
  * [SERVICE_STATE](#10-service_state)
  * [SET_PLAYER_STATE](#11-set_player_state)
  * [TEST](#12-test)


--------


## Ungrouped



### 1. CLIENT_STATUS


IF REPSONSE IS SUCCESSFUL SERVER IS ONLINE


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/client_status
```



### 2. GET_AI_PLAYER_COUNT



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/get_avariable_ai_players
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} | NOT NEEDED |
| sid | {{PLAYER_SID}} | NOT NEEDED |



### 3. GET_PLAYERS_AVARIABLE



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/get_players_avariable
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |



### 4. GET_PLAYER_STATE



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/get_player_state
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |
| simplified | 1 | RETURN ONLY NEEDED INFORAMTION  |



### 5. HEARTBEAT



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/heartbeat
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |



### 6. LOGIN



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/login
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| playertype | 0 | 0=HUMAN_PLAYER 1=AI_PLAYER used for automatchmaking |



### 7. LOGOUT



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/logout
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| playertype | 1 | 0=HUMAN_PLAYER 1=AI_PLAYER used for automatchmaking |



### 8. MAKE_MOVE



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/make_move
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |
| move | e2e4 |  |



### 9. PLAYER_SETUP_CONFIRMATION



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/player_setup_confirmation
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |



### 10. SERVICE_STATE


IF REPSONSE IS SUCCESSFUL SERVER IS ONLINE


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/service_state
```



### 11. SET_PLAYER_STATE



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/set_player_state
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} |  |
| sid | {{PLAYER_SID}} |  |
| ps | 4 | PLAYER_STATE 0=IDLE 3=SEARCHING_MATCHMAKING |



### 12. TEST



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/gmm
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} | NOT NEEDED |
| sid | {{PLAYER_SID}} | NOT NEEDED |



***Available Variables:***

| Key | Value | Type |
| --- | ------|-------------|
| BASE_URL | 127.0.0.1:3000 |  |



---
[Back to top](#atomicchessbackend)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2020-11-25 22:23:12 by [docgen](https://github.com/thedevsaddam/docgen)
