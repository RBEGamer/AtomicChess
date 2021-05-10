
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
  * [check_login_state](#13-check_login_state)
  * [get_profile_information_secure](#14-get_profile_information_secure)
  * [get_server_config](#15-get_server_config)
  * [perform_user_login](#16-perform_user_login)
  * [server_config](#17-server_config)
  * [set_user_config_single_key](#18-set_user_config_single_key)
  * [store_user_log](#19-store_user_log)


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



### 13. check_login_state


returns true if the a admin session was claimed trougth perform_user_login


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/check_login_state
```



### 14. get_profile_information_secure


This request returns the profile data from a user including config


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/get_profile_information_secure
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| pid | {{PLAYER_VIRTUAL_ID}} | Player virtual ID |
| authkey | {{API_KEY}} | api key set in config |



### 15. get_server_config


Returns the server config file (config.json)


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/get_server_config
```



### 16. perform_user_login


This request is used to claim a administrator session key to save edit server settings


***Endpoint:***

```bash
Method: POST
Type: FORMDATA
URL: http://{{ATC_BACKEND_URL}}/rest/perform_user_login
```



***Body:***

| Key | Value | Description |
| --- | ------|-------------|
| user_pw | 1337 | password set in config.json |
| origin_url | / | after successful login rediret to this destination |



### 17. server_config


This request sets a specific key in the global server config (config.json)


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/server_config
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| key | matchmaking_ai_enable | the key from the config want to change |
| authkey | {{API_KEY}} |  |
| value | false | value of the key |



### 18. set_user_config_single_key


Sets a USER_ key into the users profile config.


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/set_user_config_single_key
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| key | USER_GENERAL_ENABLE_AUTO_MATCHMAKING_ENABLE |  |
| authkey | {{API_KEY}} | api key set in config.json |
| value | false | new config value |
| vid | {{PLAYER_VIRTUAL_ID}} | virtual id of the player profile |



### 19. store_user_log


This request stores the logfile from a active table into the users profile


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_BACKEND_URL}}/rest/store_user_log
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | {{PLAYER_HWID}} | player hwid |
| sid | {{PLAYER_SID}} | players session key |
| log | --LOG_FROM_PLAYER-- | the log content |



***Available Variables:***

| Key | Value | Type |
| --- | ------|-------------|
| BASE_URL | 127.0.0.1:3000 |  |



---
[Back to top](#atomicchessbackend)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2021-05-10 22:06:10 by [docgen](https://github.com/thedevsaddam/docgen)
