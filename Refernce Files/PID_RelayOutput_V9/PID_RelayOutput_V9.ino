/********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the
 * window being "Relay Off Time"
 ********************************************************/

#include <PID_v1.h>

#define PIN_INPUT 0
#define RELAY_PIN 11

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;


String incomingByte;
String TSP_byte_0, TSP_byte_1, TSP_byte_2, TSP_byte_3; //data for temperature setpoint (TSP)
String FD_byte_0, FD_byte_1, FD_byte_2, FD_byte_3; //data for temperature setpoint (TSP)
String FR_byte_0, FR_byte_1, FR_byte_2; //data for temperature setpoint (TSP)

int val = 0;           // variable to store the value read
int temperature = 0;           // variable to store the value read
const int PumpSpeed = 9; // Analog output pin that the LED is attached to
int ABdirection = 7;   //fluid flow direction (in corroion cell) from point A to point B
int BAdirection = 8;  //fluid flow direction (in corroion cell) from point B to point A
int PumpSpeedValue = 0;

int piezoBuzzer=6;

void setup()
{
  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 25;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  Serial.begin(9600);          //  setup serial

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PumpSpeed, OUTPUT);
  pinMode(ABdirection, OUTPUT);
  pinMode(BAdirection, OUTPUT);
  pinMode(piezoBuzzer, OUTPUT);
 

  //At start keep the pump off
  
  digitalWrite(ABdirection, LOW); 
  digitalWrite(BAdirection, LOW); 
 // pinMode(piezoBuzzer, LOW);       //keep the buzzer off in start
}

void loop()
{

 //PumpSpeedValue=255;
 //  digitalWrite(ABdirection, HIGH); 
 // digitalWrite(BAdirection, LOW); 

/*Checking for settings from an operator*/
   if (Serial.available() > 0) {
           
    
              incomingByte = Serial.readString();
              TSP_byte_0=incomingByte.substring(0,1);
              TSP_byte_1=incomingByte.substring(1,2);
              TSP_byte_2=incomingByte.substring(2,4);
              if (TSP_byte_0=="t" && TSP_byte_1=="s")
              {
              Setpoint=  TSP_byte_2.toInt();
              }

              
              
              FD_byte_0 = incomingByte.substring(4,5);
              FD_byte_1 = incomingByte.substring(5,6);
              FD_byte_2 = incomingByte.substring(6,7);
              FD_byte_3 = incomingByte.substring(7,8);

              if (FD_byte_0=="f" && FD_byte_1=="d" && FD_byte_2=="0" && FD_byte_3=="1" ){
                 digitalWrite(ABdirection, HIGH); 
                 digitalWrite(BAdirection, LOW); 
              }

              else if (FD_byte_0=="f" && FD_byte_1=="d" && FD_byte_2=="1" && FD_byte_3=="0" ){
                digitalWrite(ABdirection, LOW); 
                digitalWrite(BAdirection, HIGH); 
              }

              else if (FD_byte_0=="f" && FD_byte_1=="d" && FD_byte_2=="0" && FD_byte_3=="0" ){
                digitalWrite(ABdirection, LOW); 
                digitalWrite(BAdirection, LOW); 
              }

              FR_byte_0 = incomingByte.substring(8,9);
              FR_byte_1 = incomingByte.substring(9,10);
              FR_byte_2 = incomingByte.substring(10,11);
             //ts25fd01fr1
              
              if (FR_byte_0=="f" && FR_byte_1=="r" && FR_byte_2=="1" ){
                 
                 PumpSpeedValue=155;
              
              }

              else if (FD_byte_0=="f" && FR_byte_1=="r" && FR_byte_2=="2" ){
                PumpSpeedValue=200; 
                //Serial.println(PumpSpeedValue);
              }

              else if (FD_byte_0=="f" && FR_byte_1=="r" && FR_byte_2=="3"  ){
                PumpSpeedValue=255;
                //Serial.println(PumpSpeedValue);
              }

                // Serial.println(FR_byte_0+"-"+FR_byte_1+"-"+FR_byte_2);

                digitalWrite(piezoBuzzer, LOW);
                delay(100);
                digitalWrite(piezoBuzzer, HIGH);
                delay(100);
                digitalWrite(piezoBuzzer, LOW);
                delay(100);
                digitalWrite(piezoBuzzer, HIGH);
                delay(100);
                digitalWrite(piezoBuzzer, LOW);
                delay(300);
                digitalWrite(piezoBuzzer, HIGH);
             
              
              
              
              
     }

 /*Control pump speed*/
//PumpSpeedValue=255;
digitalWrite(piezoBuzzer, HIGH);
 analogWrite(PumpSpeed, PumpSpeedValue);
     
/*Running PID to contol temperatur*/

  for (int i=0; i<=10000; i++){
    
      val = analogRead(PIN_INPUT);    // read the input pin
    
      temperature=(val-87.782)/1.5845;
      
      Input = temperature;
      myPID.Compute();
    
      /************************************************
       * turn the output pin on/off based on pid output
       ************************************************/
      if (millis() - windowStartTime > WindowSize)
      { //time to shift the Relay Window
        windowStartTime += WindowSize;
      }
      if (Output < millis() - windowStartTime) digitalWrite(RELAY_PIN, HIGH);
      else digitalWrite(RELAY_PIN, LOW);
      
  }
   
   

    Serial.println(temperature);
 

   

   

}



