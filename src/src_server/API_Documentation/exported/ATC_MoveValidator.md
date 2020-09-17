
# AtomicChessChessMoveValidator



## Indices

* [Ungrouped](#ungrouped)

  * [CHECK_MOVE](#1-check_move)
  * [EXECUTE_MOVE](#2-execute_move)
  * [STATE](#3-state)
  * [VALIDATE_BOARD](#4-validate_board)


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



### 3. STATE



***Endpoint:***

```bash
Method: GET
Type: 
URL: {{ATC_MOVE_VALIDATOR_URL}}/rest/state
```



### 4. VALIDATE_BOARD



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
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2020-07-29 23:42:15 by [docgen](https://github.com/thedevsaddam/docgen)
