#include <stdio.h>
#include <stdlib.h>
#define DDS_CLOCK 125000000

#define  CLOCK  8  //pin connections for DDS
#define  LOAD 9 
#define  DATA  10
#define  RESET 11

String incomingByte; //defines incoming sring
String MSG_type; //Start of input message, first character
String CND_type_0, CND_type_1, CND_type_2; //Condition Message type, str or end
int Start_Freq = 0;
int End_Freq = 0;
int Diff;

void setup() {
  Serial.begin(9600);
  pinMode (DATA,  OUTPUT); 
  pinMode (CLOCK, OUTPUT); 
  pinMode (LOAD,  OUTPUT); 
  pinMode (RESET, OUTPUT); 
  AD9850_init();
  AD9850_reset();

}
void AD9850_init()
{

  digitalWrite(RESET, LOW);
  digitalWrite(CLOCK, LOW);
  digitalWrite(LOAD, LOW);
  digitalWrite(DATA, LOW);
}
void AD9850_reset()
{
  //reset sequence is:
  // CLOCK & LOAD = LOW
  //  Pulse RESET high for a few uS (use 5 uS here)
  //  Pulse CLOCK high for a few uS (use 5 uS here)
  //  Set DATA to ZERO and pulse LOAD for a few uS (use 5 uS here)

  // data sheet diagrams show only RESET and CLOCK being used to reset the device, but I see no output unless I also
  // toggle the LOAD line here.

  digitalWrite(CLOCK, LOW);
  digitalWrite(LOAD, LOW);

  digitalWrite(RESET, LOW);
  delay(5);
  digitalWrite(RESET, HIGH);  //pulse RESET
  delay(5);
  digitalWrite(RESET, LOW);
  delay(5);

  digitalWrite(CLOCK, LOW);
  delay(5);
  digitalWrite(CLOCK, HIGH);  //pulse CLOCK
  delay(5);
  digitalWrite(CLOCK, LOW);
  delay(5);
  digitalWrite(DATA, LOW);    //make sure DATA pin is LOW

    digitalWrite(LOAD, LOW);
  delay(5);
  digitalWrite(LOAD, HIGH);  //pulse LOAD
  delay(5);
  digitalWrite(LOAD, LOW);
  // Chip is RESET now
}
void SetFrequency(unsigned long frequency)
{
  unsigned long tuning_word = (frequency * pow(2, 32)) / DDS_CLOCK;
  digitalWrite (LOAD, LOW); 

  shiftOut(DATA, CLOCK, LSBFIRST, tuning_word);
  shiftOut(DATA, CLOCK, LSBFIRST, tuning_word >> 8);
  shiftOut(DATA, CLOCK, LSBFIRST, tuning_word >> 16);
  shiftOut(DATA, CLOCK, LSBFIRST, tuning_word >> 24);
  shiftOut(DATA, CLOCK, LSBFIRST, 0x0);
  digitalWrite (LOAD, HIGH); 
}

void loop() {
  
if (Serial.available() > 0) {
           
    
              incomingByte = Serial.readString();
              MSG_type = incomingByte.substring(0,1);
              CND_type_0 = incomingByte.substring(1,2);
              CND_type_1 = incomingByte.substring(2,3);
              CND_type_2 = incomingByte.substring(3,4);
              if (MSG_type=="s" && CND_type_0=="s" && CND_type_1=="t" && CND_type_2=="r"){ // figure out if the setting is swing or fixed
                Start_Freq = incomingByte.substring(4,10).toDouble();
                End_Freq = incomingByte.substring(13,19).toDouble();
                  
              }
              else if (MSG_type=="f" && CND_type_0=="s" && CND_type_1=="t" && CND_type_2=="r"){
                Start_Freq = incomingByte.substring(4,10).toDouble();
                  for (int n=1; n<=20; n++){
                    SetFrequency(Start_Freq);
                    delay(500);
                  }
              }
              
  } 
 if(Start_Freq > 0 && End_Freq >0){
                  for (int n = Start_Freq; n<=End_Freq; n = n + 100){   //this is for sweep frequency
 
                    SetFrequency(n);  // freq
                    delay(10);
                  }
                  int n = Start_Freq;
 } 
  
                  //while(1)
}
