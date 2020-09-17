
# AtomicChessBackend



## Indices

* [Ungrouped](#ungrouped)

  * [GET_AI_PLAYER_COUNT](#1-get_ai_player_count)
  * [GET_PLAYERS_AVARIABLE](#2-get_players_avariable)
  * [GET_PLAYER_STATE](#3-get_player_state)
  * [GET_PLAYER_STATE Copy](#4-get_player_state-copy)
  * [HEARTBEAT](#5-heartbeat)
  * [LOGIN](#6-login)
  * [PLAYER_SETUP_CONFIRMATION](#7-player_setup_confirmation)
  * [PLAYER_STATE](#8-player_state)
  * [TEST](#9-test)


--------


## Ungrouped



### 1. GET_AI_PLAYER_COUNT



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



### 2. GET_PLAYERS_AVARIABLE



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



### 3. GET_PLAYER_STATE



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



### 4. GET_PLAYER_STATE Copy



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
| playertype | 1 | 0=HUMAN_PLAYER 1=AI_PLAYER used for automatchmaking |



### 7. PLAYER_SETUP_CONFIRMATION



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



### 8. PLAYER_STATE



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



### 9. TEST



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
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2020-07-29 23:42:15 by [docgen](https://github.com/thedevsaddam/docgen)
