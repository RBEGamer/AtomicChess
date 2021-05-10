
# AtomicChessChessMoveValidator



## Indices

* [Ungrouped](#ungrouped)

  * [CHECK_MOVE](#1-check_move)
  * [EXECUTE_MOVE](#2-execute_move)
  * [INIT_BOARD](#3-init_board)
  * [STATE](#4-state)
  * [VALIDATE_BOARD](#5-validate_board)


--------


## Ungrouped



### 1. CHECK_MOVE



***Endpoint:***

```bash
Method: POST
Type: FORMDATA
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/check_move
```



***Body:***

| Key | Value | Description |
| --- | ------|-------------|
| fen | rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1 |  |
| move | h7h6 |  |
| player | 1 |  |



### 2. EXECUTE_MOVE



***Endpoint:***

```bash
Method: POST
Type: FORMDATA
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/execute_move
```



***Body:***

| Key | Value | Description |
| --- | ------|-------------|
| fen | rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1 |  |
| move | a7a5 |  |



### 3. INIT_BOARD



***Endpoint:***

```bash
Method: GET
Type: 
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/init_board
```



### 4. STATE



***Endpoint:***

```bash
Method: GET
Type: 
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/state
```



### 5. VALIDATE_BOARD



***Endpoint:***

```bash
Method: POST
Type: FORMDATA
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/validate_board
```



***Body:***

| Key | Value | Description |
| --- | ------|-------------|
| fen | rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 0 1 |  |



***Available Variables:***

| Key | Value | Type |
| --- | ------|-------------|
| BASE_URL | http://127.0.0.1:5000 |  |



---
[Back to top](#atomicchesschessmovevalidator)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2021-05-10 22:06:10 by [docgen](https://github.com/thedevsaddam/docgen)
