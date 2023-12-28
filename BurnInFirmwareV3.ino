#include <ArduinoSTL.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include "src/eeprom_wear_level.hpp"
#include "src/util.hpp"
#include "src/controller.hpp"
#include "src/burn_timer.hpp"
#include "src/ntc.hpp"
#include "src/PID_AutoTune_v0.h"

using namespace std;
int WindowSize = 5000;
unsigned long windowStartTime;
unsigned long serialTime=1000;
unsigned long lastSerial=0;

byte ATuneModeRemember=2;
double input, output=10, setpoint=45;
double kp=2,ki=0.5,kd=2;


double aTuneStep=50, aTuneNoise=1, aTuneStartValue=100;
unsigned int aTuneLookBack=20;

boolean tuning = false;
unsigned long  modelTime;

PID pid(&input, &output, &setpoint,kp,ki,kd, DIRECT);
PID_ATune aTune(&input, &output);

ntc tempSensor(PinNumber(TempPin3),NTC1_A,NTC1_B,NTC1_C);

void setup(){
  analogReference(DEFAULT);
  // windowStartTime=millis();
  // pid.SetOutputLimits(0,WindowSize);
  // pid.SetMode(AUTOMATIC);
  // if(tuning){
  //   tuning=false;
  //   changeAutoTune();
  //   tuning=true;
  // }
  
  lastSerial=millis();
  // for(int i=0;i<100;i++){
  //   input=tempSensor.Read();
  // }
  Serial.begin(38400);

}

void loop(){
  auto temp=tempSensor.Read();
  //input = tempSensor.Read();
  
  // if(tuning){
  //   byte val = (aTune.Runtime());
  //   if (val!=0){
  //     tuning = false;
  //   }
  //   if(!tuning){ //we're done, set the tuning parameters
  //     kp = aTune.GetKp();
  //     ki = aTune.GetKi();
  //     kd = aTune.GetKd();
  //     pid.SetTunings(kp,ki,kd);
  //     AutoTuneHelper(false);
  //   }
  // }else{
  //   pid.Compute();
  // } 
  // if (millis() - windowStartTime > WindowSize){ //time to shift the Relay Window
  //   windowStartTime += WindowSize;
  // }
  // if (output < millis() - windowStartTime) {
  //   digitalWrite(heatPin1, HIGH);
  // }else{
  //   digitalWrite(heatPin1, LOW);
  // }

  if(millis()-lastSerial>=serialTime){
    lastSerial=millis();
    Serial.print("Temp: ");Serial.println(temp);
    //SerialSend();
  }

}

void serialEvent(){
  if(Serial.available())
  {
   char b = Serial.read(); 
   Serial.flush(); 
   if((b=='1' && !tuning) || (b!='1' && tuning))changeAutoTune();
  }
}

void changeAutoTune()
{
 if(!tuning)
  {
    //Set the output to the desired starting frequency.
    output=aTuneStartValue;
    aTune.SetNoiseBand(aTuneNoise);
    aTune.SetOutputStep(aTuneStep);
    aTune.SetLookbackSec((int)aTuneLookBack);
    AutoTuneHelper(true);
    tuning = true;
  }
  else
  { //cancel autotune
    aTune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}

void AutoTuneHelper(boolean start)
{
  if(start)
    ATuneModeRemember = pid.GetMode();
  else
    pid.SetMode(ATuneModeRemember);
}


void SerialSend()
{
  Serial.print("setpoint: ");Serial.print(setpoint); Serial.print(" ");
  Serial.print("input: ");Serial.print(input); Serial.print(" ");
  Serial.print("output: ");Serial.print(output); Serial.print(" ");
  if(tuning){
    Serial.println("tuning mode");
  } else {
    Serial.print("kp: ");Serial.print(pid.GetKp());Serial.print(" ");
    Serial.print("ki: ");Serial.print(pid.GetKi());Serial.print(" ");
    Serial.print("kd: ");Serial.print(pid.GetKd());Serial.println();
  }
}

// void SerialReceive()
// {
//   if(Serial.available())
//   {
//    char b = Serial.read(); 
//    Serial.flush(); 
//    if((b=='1' && !tuning) || (b!='1' && tuning))changeAutoTune();
//   }
// }

// void DoModel(){
//   //cycle the dead time
//   for(byte i=0;i<49;i++)
//   {
//     theta[i] = theta[i+1];
//   }
//   //compute the input
//   input = (kpmodel / taup) *(theta[0]-outputStart) + input*(1-1/taup) + ((float)random(-10,10))/100;

// }

