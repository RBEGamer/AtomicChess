// ATC PROJECT
// MARCEL OCHSENDORF marcelochsendorf.com 
/*
CONTROLS COILS AND USER LES STRIP VIA SPI AND READ THE PN532 NFC TAGS



*/

#include "SPICommunicationSharedDefines.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include<SPI.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);


#define NEOPIXEL_STRIP_PIXRLS 16 // Popular NeoPixel ring size
#define NEOPIXEL_STRIP  7 //SPI 11-> 16
const int COILA_A = 3;
const int COILA_B = 6;
const int COILB_A = 5;
const int COILB_B = 9;
const int STATE_LED = 2;
const int COIL_STRENGHT_INPUT = A0;
const int ACK_SPDR_CMD = 42;
Adafruit_NeoPixel pixels(NEOPIXEL_STRIP_PIXRLS, NEOPIXEL_STRIP, NEO_GRB + NEO_KHZ800);

const byte LED_STRIP_COLORS[5] = {240,180,140,90,35}; //LED STRIP COLORS OFF, IDLE, WHITE_TURN, BLACK_TURN, PROCESSING
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

//SPI SLAVE ISR FUNTION
byte myArray[1];
volatile int i = 0;
ISR (SPI_STC_vect)
{
while ((SPSR & (1<<SPIF))); //WAIT FOR FINISH TRANSMITION
  //i++;
}


//GET A COLOR FROM THE HSV WHEEL
//SEE ADAFRUIT_NEOPIXEL LIB EXAMPLE FOR THE FUNTION
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


byte current_led_pos = 0;
void set_neopixel(byte _pos){
  //GET THE COLOR FROM PREDEFINED COLOR ARRAY ELSE GET ANY COLOR...
  if(_pos >= 0 && _pos < 5){
   _pos= LED_STRIP_COLORS[_pos];
   }
   //HOW MANY STEPS TO REACH THE TARGET COLOR
   int steps = abs(_pos - current_led_pos); 
   if(steps == 0){return;}
   //MAKE STEPS IN WHICH DIRECTORY OF THE COLOR WHEEL
   int step_dir = 1;
   if((_pos-current_led_pos) > 0){
      step_dir = -1;
    }
    

    
   for(int j =0; j<steps; j++) { 
    //INC DEC CURRENT LED POS +-1 TO GET CLOSER TO TARGET COLOR
    current_led_pos += step_dir; 
   //SET ALL PIXELS TO THE NEW STEP COLOR
   for(int i=0; i<NEOPIXEL_STRIP_PIXRLS; i++) { 
    pixels.setPixelColor(i, Wheel(current_led_pos)); 
  }
  
  pixels.show(); //SHOW PIXELS
  delay(5*(255/steps)); //WAIT SOME TIME BUT MAX 5*255ms FOR A FULL COLOR ROTATION
  }
  current_led_pos = _pos; //SAVE NEW COLOR AS CURRENT COLOR
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
    pixels.begin();
    pixels.show(); 
    set_neopixel(0);

    
    //  set_neopixel(2);
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
 
 set_neopixel(4);
 
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
      SPDR = MAGIC_ACK_BYTE;
        
    }else if(SPDR == 2){
         digitalWrite(STATE_LED,LOW);
         SPDR = MAGIC_ACK_BYTE;
    }else if(SPDR == 3){
         SPDR = MAGIC_ACK_BYTE;
    }else if(SPDR == 4){
      SPDR = MAGIC_ACK_BYTE;
      return;
     }else if(SPDR == 4){
      set_coil_state(true,true);
      SPDR = MAGIC_ACK_BYTE;
     }else if(SPDR == 5){
      set_coil_state(true,true);
      SPDR = MAGIC_ACK_BYTE;
     }else if(SPDR == 6){
      set_coil_state(true,false);
      SPDR = MAGIC_ACK_BYTE;
     }else if(SPDR == 7){
      set_coil_state(false,true);
      SPDR = MAGIC_ACK_BYTE;
     }else if(SPDR == 8){
      set_coil_state(false,false);
      SPDR = MAGIC_ACK_BYTE;
      }else if(SPDR == 9){
      set_coil_state(false,false);
      Serial.println("NFC READING");
      SPDR = scan_nfc_tag();
      Serial.println(SPDR);
      return;
      }else if(SPDR >= 11 && SPDR < 16){ //USER LED STRRIP COMMAND 11-15
        set_neopixel(SPDR-11);
        SPDR = MAGIC_ACK_BYTE;
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
