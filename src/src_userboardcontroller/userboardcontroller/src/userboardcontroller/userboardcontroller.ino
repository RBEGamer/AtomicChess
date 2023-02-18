// ATC PROJECT
// MARCEL OCHSENDORF marcelochsendorf.com 


/*
 * COMMANDS:
 * 
 * _coil_0_ => ALL OFF
 * _coil_1_ => A ON
 * _coil_2_ => B ON
 * _coil_3_ => A AND B ON
 * => _coil_res_ok_
 * 
 * 
 * _readnfc_ => _readnfc_res_R_ok_ OR _readnfc_res_R_notagpresent_
 * _version_ => _version_res_1.04_
 * _state_ => _state_res_ok_
 * 
 * _led_<0-5>_ => _led_res_ok_
 * 
 * _servo_<0-255>_ => _servo_res_ok_
 */



#define UBC_COMMAND_VERSION "version"
#define UBC_COMMAND_READNFC "readnfc"
#define UBC_COMMAND_STATE "state"
#define UBC_COMMAND_LED "led"
#define UBC_COMMAND_COIL "coil"
#define UBC_COMMAND_SERVO "servo"

#include <Servo.h>
const int servo_pin = 8;
Servo myServo;

#include <Wire.h>
#include "PN532_I2C.h"
#include <PN532.h>
#include <NfcAdapter.h>
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);


#include "Adafruit_NeoPixel.h"
#define NUMPIXELS 120
#define NEOPIXEL_STRIP_PIN 6  
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_STRIP_PIN, NEO_GRB + NEO_KHZ800);
#define MAX_COLORS 6
const byte LED_STRIP_COLORS[MAX_COLORS] = {240,180,140,90,35, 0}; //LED STRIP COLORS OFF, IDLE, WHITE_TURN, BLACK_TURN, PROCESSING



const int COILA_A = 5;
const int COILA_B = 9;
const int COILB_A = 3;
const int COILB_B = 6;
const int COIL_STRENGHT_INPUT = A0;


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
void set_neopixel(const byte _pos){
  int target_led_pos = 0;
  //GET THE COLOR FROM PREDEFINED COLOR ARRAY ELSE GET ANY COLOR...
  if(_pos >= 0 && _pos < MAX_COLORS){
    target_led_pos = LED_STRIP_COLORS[_pos];
   }
   //HOW MANY STEPS TO REACH THE TARGET COLOR
   const int steps = abs(target_led_pos - current_led_pos); 
  if(steps == 0){
    for(int i=0; i<NUMPIXELS; i++) { 
      pixels.setPixelColor(i, Wheel(current_led_pos)); 
    }
    pixels.show();
  }
   //MAKE STEPS IN WHICH DIRECTORY OF THE COLOR WHEEL
   int step_dir = 1;
   if(target_led_pos < current_led_pos){
      step_dir = -1;
    }
    
  
  for(int j =0; j<steps; j++) { 
    //INC DEC CURRENT LED POS +-1 TO GET CLOSER TO TARGET COLOR
    current_led_pos += step_dir; 
   //SET ALL PIXELS TO THE NEW STEP COLOR
    for(int i=0; i<NUMPIXELS; i++) { 
      pixels.setPixelColor(i, Wheel(current_led_pos)); 
    }
  
  pixels.show(); //SHOW PIXELS
  delay(5*(255/steps)); //WAIT SOME TIME BUT MAX 5*255ms FOR A FULL COLOR ROTATION
  }

}



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


const int STATE_LED = 4;



void coils_off(){
     digitalWrite(COILA_A,LOW);
    digitalWrite(COILA_B,LOW);
    digitalWrite(COILB_A,LOW);
    digitalWrite(COILB_B,LOW);
 }

void set_coil_state(int _coil, int _state){
//Serial.println(map(analogRead(COIL_STRENGHT_INPUT),0,1024,0,255));
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


void setup(void) {
  pinMode(STATE_LED, OUTPUT);
  digitalWrite(STATE_LED,LOW);
  delay(100);
  digitalWrite(STATE_LED,HIGH);
  delay(100);
  digitalWrite(STATE_LED,LOW);
  
  Serial.begin(9600);
  

  pixels.begin();
  pixels.clear();
  pixels.show();
  set_neopixel(5);

    
  return; 
  coils_off();
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

  digitalWrite(STATE_LED,HIGH);
  
  
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
     Serial.println("_state_res_ok_");
  }else if (getValue(readString, '_', 1) == UBC_COMMAND_VERSION) {
     Serial.println("_version_res_1.04_");
  }else if (getValue(readString, '_', 1) == UBC_COMMAND_LED) {
    int t = getValue(readString, '_', 2).toInt();
    set_neopixel(t);
    Serial.println("_led_res_ok_");


    
   }else if (getValue(readString, '_', 1) == UBC_COMMAND_COIL) {
    int coil = getValue(readString, '_', 2).toInt();

    //set_coil_state( COIL 1-0, STATE 1-0)
    
     if(coil == 0){
      set_coil_state(0, 0);
     }else if(coil == 1){
      set_coil_state(0, 1);
      set_coil_state(1, 0);
     }else if(coil == 2){
      set_coil_state(0, 0);
      set_coil_state(1, 1);
     }else if(coil == 3){
      set_coil_state(0, 1);
      set_coil_state(1, 1);
     }
      Serial.println("_coil_res_ok_");


  }else if (getValue(readString, '_', 1) == UBC_COMMAND_SERVO) {
    int pos = getValue(readString, '_', 2).toInt();

    myServo.attach(servo_pin);
    myServo.write(pos);
    delay(50);
    myServo.detach();
     Serial.println("_servo_res_ok_");
  }




  
  readString = "";
  digitalWrite(STATE_LED,LOW);
}



//EINBRENNEN VERHINDERN

}
