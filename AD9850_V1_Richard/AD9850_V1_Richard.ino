/*
* A simple single freq AD9850 Arduino test script
* Original AD9851 DDS sketch by Andrew Smallbone at www.rocketnumbernine.com
* Modified for testing the inexpensive AD9850 ebay DDS modules
* Pictures and pinouts at nr8o.dhlpilotcentral.com
* 9850 datasheet at http://www.analog.com/static/imported-files/data_sheets/AD9850.pdf
* Use freely
*/
#include <stdio.h>
#include <stdlib.h>
#include <SPI.h>
#define W_CLK 8       // Pin 8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 9       // Pin 9 - connect to freq update pin (FQ)
#define DATA 10       // Pin 10 - connect to serial data load pin (DATA)
#define RESET 11      // Pin 11 - connect to reset pin (RST).

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

String incomingByte; //defines incoming sring
String MSG_type; //Start of input message, first character
String CND_type_0, CND_type_1; //Condition Message type, str or end
long unsigned int freq = 1000;
long unsigned int FreqOld = freq;
String Start_Freq;
String End_Freq;
long unsigned int Start;
long unsigned int End;

// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
 for (int i=0; i<8; i++, data>>=1) {
   digitalWrite(DATA, data & 0x01);
   pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
 }
}

// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
 int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
 for (int b=0; b<4; b++, freq>>=8) {
   tfr_byte(freq & 0xFF);
 }
 tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
 pulseHigh(FQ_UD);  // Done!  Should see output
}

void setup() {
 
 Serial.begin(9600);          //  setup serial
// configure arduino data pins for output
 pinMode(FQ_UD, OUTPUT);
 pinMode(W_CLK, OUTPUT);
 pinMode(DATA, OUTPUT);
 pinMode(RESET, OUTPUT);

 pulseHigh(RESET);
 pulseHigh(W_CLK);
 pulseHigh(FQ_UD);  // this pulse enables serial mode - Datasheet page 12 figure 10
}

void loop() {

  if (Serial.available() > 0) {
           
    
              incomingByte = Serial.readString();
              MSG_type = incomingByte.substring(0,1);
              CND_type_0 = incomingByte.substring(1,2);
              CND_type_1 = incomingByte.substring(5,6);              
              Start_Freq = incomingByte.substring(2,5);
              End_Freq = incomingByte.substring(6,9);
              Serial.println(incomingByte);
              if (MSG_type=="s" && CND_type_0=="S" && CND_type_1=="E" ){ // figure out if the setting is swing or fixed
                Start = Start_Freq.toInt();
                End = End_Freq.toInt();
                Start *= 1000;
                End *= 1000;
                freq = Start;        
                Serial.println(Start);
                Serial.println(End);         
              }
              else if (MSG_type=="f" && CND_type_0=="S"){
                Start = Start_Freq.toInt();
                    Start *= 1000;
                    freq = Start;
              }
              
  }
 
  if (MSG_type=="s") { for (freq = Start; freq<=End; freq += 100){   //this is for sweep frequency
                    sendFrequency(freq);  // freq
                    delay(10);
                  }
                  freq = Start;
  }
  if (MSG_type=="f"){
                  sendFrequency(freq);
  }
// while(1);
//sS000E000
}
