
# AtomicChessVoiceRecognitionService



## Indices

* [Ungrouped](#ungrouped)

  * [http://{{ATC_VOICE_RECOGNITION_URL}}/alexa_register?userId=1234&userPin=1337](#1-http:{{atc_voice_recognition_url}}alexa_register?userid=1234&userpin=1337)
  * [http://{{ATC_VOICE_RECOGNITION_URL}}/get_move?hwid=b42e99fb4a2b](#2-http:{{atc_voice_recognition_url}}get_move?hwid=b42e99fb4a2b)
  * [http://{{ATC_VOICE_RECOGNITION_URL}}/reset_move?hwid=hwid](#3-http:{{atc_voice_recognition_url}}reset_move?hwid=hwid)


--------


## Ungrouped



### 1. http://{{ATC_VOICE_RECOGNITION_URL}}/alexa_register?userId=1234&userPin=1337



***Endpoint:***

```bash
Method: GET
Type: RAW
URL: http://{{ATC_VOICE_RECOGNITION_URL}}/alexa_register
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| userId | 1234 | Alexa / User ID |
| userPin | 1337 | alexa skill generated pairing pin |



***Body:***

```js        
{
  "requestEnvelope": {
    "version": "1.0",
    "session": {
      "new": false,
      "sessionId": "amzn1.echo-api.session.0ee27e97-d689-4838-8c67-a6efaaa10ac9",
      "application": {
        "applicationId": "amzn1.ask.skill.abc9437c-29d1-4d18-b8b1-c70bf85b405b"
      },
      "user": {
        "userId": "amzn1.ask.account.AFDENJBI4ECYG7GYB7RFYM4FDDYLHGTFEM3VOG5FAUW2JEWJZEKYCVF7HTOBU66HPI2WKQAQZNB4CLX2CDUBB54TSFQGZZUG4KCN4RKWQT65AI76LY3PTNCHVFLOGPVY6YIGLJ6JLLKXKC7IFERPT4VWJBNHKXWOC3NNE7U2RGAPRJN6AUD4ERPMPGISVOH2LGMHWSOHUOVDPTI"
      }
    },
    "context": {
      "Extensions": {
        "available": {}
      },
      "System": {
        "application": {
          "applicationId": "amzn1.ask.skill.abc9437c-29d1-4d18-b8b1-c70bf85b405b"
        },
        "user": {
          "userId": "amzn1.ask.account.AFDENJBI4ECYG7GYB7RFYM4FDDYLHGTFEM3VOG5FAUW2JEWJZEKYCVF7HTOBU66HPI2WKQAQZNB4CLX2CDUBB54TSFQGZZUG4KCN4RKWQT65AI76LY3PTNCHVFLOGPVY6YIGLJ6JLLKXKC7IFERPT4VWJBNHKXWOC3NNE7U2RGAPRJN6AUD4ERPMPGISVOH2LGMHWSOHUOVDPTI"
        },
        "device": {
          "deviceId": "amzn1.ask.device.AGWIKJD5PGUDMGPCQK2GBMRA4TIXZVRKN5V46UB63IJRXMWXHP6VTWYCZB23ELVTBSHHOAYKLUG3VFKNLTTIBTYU5AY4GFKJ43KXIKP5U4IH5OBMZKURHJIDLYFQP5YQJ25ER3YCOMUA5PGRIICMF6ALT5DXEDPBJPSQNGNEW24EKVDC56AHG",
          "supportedInterfaces": {}
        },
        "apiEndpoint": "https://api.eu.amazonalexa.com",
        "apiAccessToken": "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6IjEifQ.eyJhdWQiOiJodHRwczovL2FwaS5ldS5hbWF6b25hbGV4YS5jb20iLCJpc3MiOiJBbGV4YVNraWxsS2l0Iiwic3ViIjoiYW16bjEuYXNrLnNraWxsLmFiYzk0MzdjLTI5ZDEtNGQxOC1iOGIxLWM3MGJmODViNDA1YiIsImV4cCI6MTYxOTcwNjExOSwiaWF0IjoxNjE5NzA1ODE5LCJuYmYiOjE2MTk3MDU4MTksInByaXZhdGVDbGFpbXMiOnsiY29udGV4dCI6IkFBQUFBQUFBQVFBU3lrOURjOGtqTGtNd1JkNm5SaS93S3dFQUFBQUFBQUIrQW1VSm1pVGJLdDNlWWNCcmoyd2MxT3BnUWZHM2p3NWdmejFSUkNTNms5RURLNFRJc0ZkUVkvbHIzdVNWUWJsR2Y5WWpaT0tON3pBZ1VIdEtiVmpuUEErMkdnTUVickJEazg1bnpFZW1lZlRnckF4OU14Nm12Vm1yU3VvenpVK1ZLTUxLeXhHYVNnWG5OWTd3TkVvYksxb0MzcXh2T1ZPeUdJcE5OSEVvWisvN0o1M3Fybk13b0tMcEJmUFBlazBONjNWeTNHRENSN3JDb3RwcDFJSHZXYUEzeEU0TUJGbjlIeC9JeXB5UnIvaWF2SzhNWHBad2ZVVVBWQ2ZsZzJkSUU5V2NIcEhkeWUrSGE3VTNWa21LSGNtTHIzclR0L0ptRDFZVDdqY0ZOendPTXJqb0FaQ2ZCNXNOVFVUZ2Vtajk3WXA0SVdjdThudStMdTRNeTFabTZPZFAxSUVTeTNXWjlvbFVPbWpnMUxJVmt6bHFqcEcveHkzb1g5eEU3eVMzVHYzbk1SRHJsaG9Va3c9PSIsImNvbnNlbnRUb2tlbiI6bnVsbCwiZGV2aWNlSWQiOiJhbXpuMS5hc2suZGV2aWNlLkFHV0lLSkQ1UEdVRE1HUENRSzJHQk1SQTRUSVhaVlJLTjVWNDZVQjYzSUpSWE1XWEhQNlZUV1lDWkIyM0VMVlRCU0hIT0FZS0xVRzNWRktOTFRUSUJUWVU1QVk0R0ZLSjQzS1hJS1A1VTRJSDVPQk1aS1VSSEpJRExZRlFQNVlRSjI1RVIzWUNPTVVBNVBHUklJQ01GNkFMVDVEWEVEUEJKUFNRTkdORVcyNEVLVkRDNTZBSEciLCJ1c2VySWQiOiJhbXpuMS5hc2suYWNjb3VudC5BRkRFTkpCSTRFQ1lHN0dZQjdSRllNNEZERFlMSEdURkVNM1ZPRzVGQVVXMkpFV0paRUtZQ1ZGN0hUT0JVNjZIUEkyV0tRQVFaTkI0Q0xYMkNEVUJCNTRUU0ZRR1paVUc0S0NONFJLV1FUNjVBSTc2TFkzUFROQ0hWRkxPR1BWWTZZSUdMSjZKTExLWEtDN0lGRVJQVDRWV0pCTkhLWFdPQzNOTkU3VTJSR0FQUkpONkFVRDRFUlBNUEdJU1ZPSDJMR01IV1NPSFVPVkRQVEkifX0.EasxwkMQWK9np72c5XLcda392mv54YRUOmdl-6mHnmWCD4wajjF-M1IUCzpRtenP9sc332FmjTEmCpc7kmySJx26USQpVaWCatropugUUzDLEAs5Jo5LrQR54ECZ9m6Legc5tLGRl4AfFFPCgf7YEHXsUDN3om0wtahIl_jl4DzrjUUXFJx2Yd8e-nmtvMxEbZMU6WDsncfAP29BVa_0CaXx5eDDmPM7MP45-RThZn8fsVy1L2PyjN57hOROnQv2c6OIjNi0UVh0JpHxyPePVUrJiVobmWY3hUGz9ZnwSIe5BhskYTAFiocfTqkrr9VVIbcFDJXFnFMJAQp6z7UDMA",
        "unit": {
          "unitId": "amzn1.ask.unit.AERSLAX4OHKUYGVYSCW4MAGEMKRIBR7DYL42IBR5ZT3P6GJICG6YAZJGFGYZGZPWNULG7BHGOQNAZQAVCZITTEP6ZQVODYL2AI32JFMYJO6C7DS4BZTFAFWJ643XN2ICPM"
        }
      }
    },
    "request": {
      "type": "IntentRequest",
      "requestId": "amzn1.echo-api.request.0615e925-b1bc-4a2b-94c9-c41b1d57fb22",
      "locale": "en-US",
      "timestamp": "2021-04-29T14:16:59Z",
      "intent": {
        "name": "makemove",
        "confirmationStatus": "NONE",
        "slots": {
          "column": {
            "name": "column",
            "value": "a",
            "resolutions": {
              "resolutionsPerAuthority": [
                {
                  "authority": "amzn1.er-authority.echo-sdk.amzn1.ask.skill.abc9437c-29d1-4d18-b8b1-c70bf85b405b.fields_col",
                  "status": {
                    "code": "ER_SUCCESS_MATCH"
                  },
                  "values": [
                    {
                      "value": {
                        "name": "A",
                        "id": "a"
                      }
                    }
                  ]
                }
              ]
            },
            "confirmationStatus": "NONE",
            "source": "USER"
          },
          "figure": {
            "name": "figure",
            "value": "pawn",
            "resolutions": {
              "resolutionsPerAuthority": [
                {
                  "authority": "amzn1.er-authority.echo-sdk.amzn1.ask.skill.abc9437c-29d1-4d18-b8b1-c70bf85b405b.figures",
                  "status": {
                    "code": "ER_SUCCESS_MATCH"
                  },
                  "values": [
                    {
                      "value": {
                        "name": "pawn",
                        "id": "p"
                      }
                    }
                  ]
                }
              ]
            },
            "confirmationStatus": "NONE",
            "source": "USER"
          },
          "row": {
            "name": "row",
            "value": "5",
            "resolutions": {
              "resolutionsPerAuthority": [
                {
                  "authority": "amzn1.er-authority.echo-sdk.amzn1.ask.skill.abc9437c-29d1-4d18-b8b1-c70bf85b405b.fields_row",
                  "status": {
                    "code": "ER_SUCCESS_MATCH"
                  },
                  "values": [
                    {
                      "value": {
                        "name": "5",
                        "id": "5"
                      }
                    }
                  ]
                }
              ]
            },
            "confirmationStatus": "NONE",
            "source": "USER"
          }
        }
      },
      "dialogState": "COMPLETED",
      "sensitivityClassification": null
    }
  },
  "context": {
    "callbackWaitsForEmptyEventLoop": true,
    "functionVersion": "38",
    "functionName": "abc9437c-29d1-4d18-b8b1-c70bf85b405b",
    "memoryLimitInMB": "512",
    "logGroupName": "/aws/lambda/abc9437c-29d1-4d18-b8b1-c70bf85b405b",
    "logStreamName": "2021/04/29/[38]2c001553f0a34e8a90f3f4429aed24ec",
    "invokedFunctionArn": "arn:aws:lambda:eu-west-1:202906012113:function:abc9437c-29d1-4d18-b8b1-c70bf85b405b:Release_0",
    "awsRequestId": "c1cf4f64-a99b-4490-8339-e09c6076b3f0"
  },
  "attributesManager": {},
  "responseBuilder": {},
  "registered_user": true
}
```



### 2. http://{{ATC_VOICE_RECOGNITION_URL}}/get_move?hwid=b42e99fb4a2b


used to get the last move the user entered over the alexa skill



***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_VOICE_RECOGNITION_URL}}/get_move
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | b42e99fb4a2b | client / table hwid |



### 3. http://{{ATC_VOICE_RECOGNITION_URL}}/reset_move?hwid=hwid


used for reset the last move on the voice system


***Endpoint:***

```bash
Method: GET
Type: 
URL: http://{{ATC_VOICE_RECOGNITION_URL}}/reset_move
```



***Query params:***

| Key | Value | Description |
| --- | ------|-------------|
| hwid | hwid | client / table hwid |



---
[Back to top](#atomicchessvoicerecognitionservice)
> Made with &#9829; by [thedevsaddam](https://github.com/thedevsaddam) | Generated at: 2021-05-10 22:06:10 by [docgen](https://github.com/thedevsaddam/docgen)
