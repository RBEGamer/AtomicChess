// ATC PROJECT
// MARCEL OCHSENDORF marcelochsendorf.com 




#define UBC_COMMAND_VERSION "version"
#define UBC_COMMAND_READNFC "readnfc"
#define UBC_COMMAND_STATE "state"
#define UBC_COMMAND_LED "led"

#include <Wire.h>


#include "PN532_I2C.h"
#include "PN532.h"
#include <NfcAdapter.h>


PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);


String readString;
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {
    0,
    -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}



char scan_nfc_read_error = 0;
char scan_nfc_tag(){
  scan_nfc_read_error = 0;
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
      //  tag.print();
        NdefMessage  msg = tag.getNdefMessage();
        if(msg.getRecordCount() > 0){
         NdefRecord  record = msg.getRecord(0);
         int payloadLength = record.getPayloadLength();
          byte payload[payloadLength];
          record.getPayload(payload);
          String payloadAsString = "";
          for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
     
          }
          
          //Serial.println(payloadAsString);
          int index_equal =payloadAsString.indexOf("="); //GET FIRST = charakter which is the start of the payload
          //CHECK IF = EXISTS AND LENGHT IS HIGHER  =<ID>=
          if(index_equal >= 0 && payloadLength > index_equal+1){
            char id = payloadAsString[index_equal+1];
            //Serial.println(id);
          return id;
          }
          
         return 0;
         scan_nfc_read_error = 1;
         }

         return 0;
        scan_nfc_read_error = 2;
    }

    //NO TAG PRESENT
    scan_nfc_read_error = 3;
  }


int STATE_LED = LED_BUILTIN;






void setup(void) {
  pinMode(STATE_LED, OUTPUT);
  digitalWrite(STATE_LED,LOW);
    Serial.begin(115200);
  
    delay(100);
  
 

    nfc.begin();
 

  digitalWrite(STATE_LED,HIGH);
  delay(200);
  digitalWrite(STATE_LED,LOW);
  delay(200);
  digitalWrite(STATE_LED,HIGH);
  delay(200);
  digitalWrite(STATE_LED,LOW);
  Serial.println("_ENTERLOOP_");

 
}
void loop() {


  while (Serial.available()) {
    delay(30); //delay to allow buffer to fill
    if (Serial.available() > 0) {
      char c = Serial.read(); //gets one byte from serial buffer
      readString += c; //makes the string readString
      if (c == '\n') {
        Serial.flush();
        break;
      }
    }
  }

if (readString.length() > 0) {

  if (getValue(readString, '_', 1) == UBC_COMMAND_READNFC) {

   char readres = scan_nfc_tag();
   String error_code = "ok";
   if(scan_nfc_read_error != 3 && scan_nfc_read_error != 0){
    error_code = "readerror";
   }else if(scan_nfc_read_error == 3){
       error_code = "notagpresent";
   }
    //scan_nfc_read_error = 3 // NO TAG PRESENT
    Serial.println("_readnfc_res_" + String(readres) + "_" + error_code + "_");
   

  } else if (getValue(readString, '_', 1) == UBC_COMMAND_STATE) {
     Serial.println("_state_res_OK_");
  }else if (getValue(readString, '_', 1) == UBC_COMMAND_VERSION) {
     Serial.println("_version_res_1.04_");
  }else if (getValue(readString, '_', 1) == UBC_COMMAND_LED) {
  }


  
  readString = "";
}



//EINBRENNEN VERHINDERN

}
