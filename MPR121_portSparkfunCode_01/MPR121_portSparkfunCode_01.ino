/*Copyright (c) 2010 bildr community
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 ~~~~~~ CIRCUIT ~~~~~~~
 MPR121 <-----> Photon
   3.3v         3V3
    IRQ         D2
    SCL         D1
    SDA         D0
    GND         GND
*/

#include "mpr121.h"
//#include <Wire.h>

int irqpin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor

  Serial.begin(9600);
  Wire.begin();

  mpr121_setup();

  Serial.println("Starting");
  Particle.connect();
}

void loop(){
  readTouchInputs();
}


void readTouchInputs(){
  //if(!checkInterrupt()){

    //read the touch state from the MPR121
    // grabbing 30 bytes, as a payload of a variety of good stuff
    // reference (page 8): https://www.sparkfun.com/datasheets/Components/MPR121.pdf
    byte payload[30];
    int rawTouchValues[12];

    Wire.requestFrom(0x5A,30);
    for(byte i=0;i<30;i++){
      payload[i] = Wire.read();
    }
    // bytes 0 and 1 represent touches
    uint16_t touched = ((payload[1] << 8) | payload[0]);

    // bytes 2 and 3 are out of range bits, ignoring for now
    // bytes 4-28 are LSB and MSB values for each sensor
    // bytes 29-30 are LSB and MSB for mystery 13th electrode (perhaps catchall?)
    for(byte i=0;i<12;i++){
      // offset position by 4, grab in batches of 2
      byte LSB = payload[4+(i*2)];
      byte MSB = payload[4+(i*2)+1];
      rawTouchValues[i] = ((MSB << 8) | LSB);
    }



    //16bits that make up the touch states

    for(byte i=0;i<12;i++){
      Serial.print(rawTouchValues[i]);
      Serial.print("\t");
    }

    Serial.print(payload[1], BIN);
    Serial.print(payload[0], BIN);

    Serial.println("");


    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){

        if(touchStates[i] == 0){
          //pin i was just touched
          /*Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");*/

        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }

        touchStates[i] = 1;
      }else{
        if(touchStates[i] == 1){
          /*Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");*/

          //pin i is no longer being touched
       }

        touchStates[i] = 0;
      }

    }

  //}
}




void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00);

  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);

  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);

  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);

  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);

  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);

  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);

  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);

  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);

  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);

  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);

  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);

  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, 0x5C, 127);
  set_register(0x5A, FIL_CFG, 0x31);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x3C);  // Enables all 12 Electrodes with proximity
  //set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes


  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

  //set_register(0x5A, ELE_CFG, 0x0C);

}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
