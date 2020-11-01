

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include<SPI.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

const int COILA_A = 3;
const int COILA_B = 6;
const int COILB_A = 5;
const int COILB_B = 9;
const int STATE_LED = 2;
const int COIL_STRENGHT_INPUT = A0;
byte scan_nfc_tag(){
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
          
          Serial.println(payloadAsString);
          int index_equal =payloadAsString.indexOf("="); //GET FIRST = charakter which is the start of the payload
          //CHECK IF = EXISTS AND LENGHT IS HIGHER  =<ID>=
          if(index_equal >= 0 && payloadLength > index_equal+1){
            char id = payloadAsString[index_equal+1];
            Serial.println(id);
          return id;
          }
          
          
         return 0;
         }
        return -2;
    }
    return -1;
  }

void coils_off(){
     digitalWrite(COILA_A,LOW);
    digitalWrite(COILA_B,LOW);
    digitalWrite(COILB_A,LOW);
    digitalWrite(COILB_B,LOW);
 }

void set_coil_state(int _coil, int _state){
Serial.println(map(analogRead(COIL_STRENGHT_INPUT),0,1024,0,255));
  if(_coil){
    if(_state){
      analogWrite(COILA_A,map(analogRead(COIL_STRENGHT_INPUT),0,1024,0,255));
    }else{
      analogWrite(COILA_A,0);
    }
  }else{
     if(_state){
      analogWrite(COILB_A,map(analogRead(COIL_STRENGHT_INPUT),0,1024,0,255));
    }else{
      analogWrite(COILB_A,0);
    }
  }
  
  
  
}


byte myArray[1];
volatile int i = 0;
ISR (SPI_STC_vect)   //Inerrrput routine function
{
while ((SPSR & (1<<SPIF)));
  
 // Serial.println(SPDR);
  //i++;
}

bool state = false;
void setup(void) {
  pinMode(STATE_LED, OUTPUT);
  digitalWrite(STATE_LED,LOW);
    Serial.begin(9600);
    //COIL SERTUP
   // pinMode(COILA_A,OUTPUT);
   // pinMode(COILA_B,OUTPUT);
   // pinMode(COILB_A,OUTPUT);
   // pinMode(COILB_B,OUTPUT);
   // coils_off();
    delay(100);
   // set_coil_state(0,1);
    //set_coil_state(1,1);
    //SPI SLAVE SETUP
    
    pinMode(MISO,OUTPUT);  
    SPCR=(1<<SPE)|(1<<SPIE);       //Enable SPI && interrupt enable bit
    SPDR=0;
    SPI.attachInterrupt(); //ENABLE INTERRUPT
  //  Serial.println("NDEF Reader");
    nfc.begin();
  i = 0;

  digitalWrite(STATE_LED,HIGH);
  delay(200);
  digitalWrite(STATE_LED,LOW);
  delay(200);
  digitalWrite(STATE_LED,HIGH);
  delay(200);
  digitalWrite(STATE_LED,LOW);
  Serial.println("== ENTER LOOP ==");
}

void loop(void) {
  
if(SPDR <= 0){ return;}

    
    //STATUS
    //SET STATUSLED = 4
    //SETLEDSTRIP = 5
    //READ_USER_COIL_STRENGTH_SETTING = 6,
    //SET_COIL_STRENGHT = 7
    if(SPDR == 1){
      digitalWrite(STATE_LED,HIGH);
        //set_coil_state(true,true);
        // set_coil_state(false,false);
        
    }else if(SPDR == 2){
         digitalWrite(STATE_LED,LOW);
    }else if(SPDR == 3){
         SPDR = 42;
         return;
    }else if(SPDR == 4){
      SPDR = 42;
      return;
     }else if(SPDR == 4){
      set_coil_state(true,true);
     }else if(SPDR == 5){
      set_coil_state(true,true);
     }else if(SPDR == 6){
      set_coil_state(true,false);
     }else if(SPDR == 7){
      set_coil_state(false,true);
     }else if(SPDR == 8){
      set_coil_state(false,false);
      }else if(SPDR == 9){
      set_coil_state(false,false);
      Serial.println("NFC READING");
      SPDR = scan_nfc_tag();
      Serial.println(SPDR);
      
      return;
    }
    Serial.println("=============================================");
    Serial.println(SPDR);
    Serial.println("=============================================");
  
  SPDR = 0;
 
  //if(i>0){
   // Serial.println(i);
   // }
 //Serial.println(myArray[0]);
}
