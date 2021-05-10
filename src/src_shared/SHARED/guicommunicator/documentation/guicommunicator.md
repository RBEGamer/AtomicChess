
# GUICOMMUNICATOR



## Indices

* [Ungrouped](#ungrouped)

  * [GET_VERSION_CONTROLLER](#1-get_version_controller)
  * [GET_VERSION_QT](#2-get_version_qt)
  * [SET_EVENT_CONTROLLER](#3-set_event_controller)
  * [SET_EVENT_QT](#4-set_event_qt)


--------


## Ungrouped



### 1. GET_VERSION_CONTROLLER



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://127.0.0.1:8001/version
```



### 2. GET_VERSION_QT



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://127.0.0.1:8000/version
```



### 3. SET_EVENT_CONTROLLER



***Endpoint:***

```bash
Method: POST
Type: RAW
URL: http://127.0.0.1:8001/status
```



***Body:***

```js        
"{\"ack\": 1, \"event\": 30, \"is_event_valid\": false, \"ispageswitchevent\": 0, \"type\": 4, \"value\": \"\"}"
```



### 4. SET_EVENT_QT



***Endpoint:***

```bash
Method: POST
Type: RAW
URL: http://127.0.0.1:8000/status
```



***Body:***

```js        
"{\"ack\": 1, \"event\": 39, \"is_event_valid\": false, \"ispageswitchevent\": 0, \"type\": 4, \"value\": \"\"}"
```



---
[Back to top](#guicommunicator)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2021-03-11 00:47:37 by [docgen](https://github.com/thedevsaddam/docgen)
