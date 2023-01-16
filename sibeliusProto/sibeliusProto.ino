//log
//most of the code(especially any odd ones) in/for loop() is written to optimise the detection of the pump buttons
//sometimes it doesnt compile or serial doesnt run so restart erveyrt

//shud runs despite not connecting

#include <Wire.h>    
#include <EEPROM.h>     
#include "interface.h"

#define OFF     1            // To note: For P2H1588A0, 1 = OFF, 0 = ON (i.e. reversed). 
#define ON      0            // therefore ONLY USE THESE FOR P2H1588A0 RELAY
#define water   0
#define coolant 1

// pinout setup
const int override_button = A0;
const int select_button   = A1;
const int w_button        = A2;  // for water
const int c_button        = A3;  // for coolant
const int lcdPower        = 2;
const int led1            = 3;
const int led2            = 4;
int LED[2] = {led1, led2};

const int sol1            = 8;
const int sol2            = 7;
const int w_pump          = 6; 
const int c_pump          = 5;
int cncSet[2] = {sol1, sol2}; 

//9 - 13 used for ethernet
interface interface;





//troubleshooting function
void show(unsigned long val){Serial.print("------>");Serial.println(val);}
void alloff(){
  digitalWrite(sol1, OFF);
  digitalWrite(sol2, OFF);
  digitalWrite(w_pump, OFF);
  digitalWrite(c_pump, OFF);
}

// Function to switch outputs(cncs)
bool cncNum;
void cncSelect(){

  static bool run = false;
  while (digitalRead(select_button) == HIGH){
    run = true;
  } 
  if (run == true){
    cncNum = !cncNum; 
    EEPROM.update(7, cncNum);
    Serial.print("Switched");
    show(cncNum);
    run = false;

    if (cncNum == 0){         
      digitalWrite(led2, 0);
      digitalWrite(led1, 1);
    }
    else{
      digitalWrite(led1, 0);
      digitalWrite(led2, 1);
    }
    
  }
  
}

// Function to allow exceeding pre-set limits
bool overrideTrigger = false;
void override(){ 
  static bool run;
  static bool i;
  static unsigned long timer;
  while (digitalRead(override_button) == HIGH){
    run = true;
  }
  if (run == true){
    overrideTrigger = !overrideTrigger;

    if (overrideTrigger == false){
      digitalWrite(LED[cncNum], HIGH);
    }
    else{
      i = 0;
    }
    run = false;
  }
  if (overrideTrigger == true && timer < millis()){
    digitalWrite(LED[cncNum], i);
    timer = millis() + 1000;
    i = !i;
  }
}





void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(w_button, INPUT); 
  pinMode(c_button, INPUT);
  pinMode(sol1, OUTPUT);
  pinMode(sol2, OUTPUT);
  pinMode(w_pump, OUTPUT);
  pinMode(c_pump, OUTPUT);
  pinMode(lcdPower, OUTPUT);
  pinMode(override_button, INPUT);
  pinMode(select_button, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  alloff();

  if (interface.connectPHP() == 0){
    Serial.println("Connection failed, program stopped.");
    while (true){
      digitalWrite(led1, !digitalRead(led1));
      digitalWrite(led2, !digitalRead(led2));
      delay(1000);
    }
  }

  EEPROM.get(7, cncNum); 
  digitalWrite(LED[cncNum], HIGH);

  delay(1000);
  Serial.println("Program started");
  Serial.println();
}





void loop() {

  cncSelect();  
  override();                                      

  /* Relay setup */
  static bool check = false;
  static bool run = false;
  static bool runtimeConfig;
  static unsigned long runtime;
  static unsigned long runtimeCoolant = 0;
  static unsigned long runtimeWater = 0;
  static int pinConfig[3];       //sol, pump, button    
  pinConfig[0] = cncSet[cncNum]; 
  if (digitalRead(w_button) == HIGH){
    pinConfig[1] = w_pump;
    pinConfig[2] = w_button;
    runtimeConfig = water;
    Serial.print("Loading...");
    runtimeWater = interface.obtainConfig(runtimeConfig, overrideTrigger); //shud this return a struct insted kusa
    Serial.print("Runtime (water): ");
    Serial.println(runtimeWater);
    check = true;
  }

  if (digitalRead(c_button) == HIGH){ //im removing else if
    pinConfig[1] = c_pump;
    pinConfig[2] = c_button;
    runtimeConfig = coolant;
    Serial.print("Loading...");
    runtimeCoolant = interface.obtainConfig(runtimeConfig, overrideTrigger);
    Serial.print("Runtime (coolant): ");
    Serial.println(runtimeCoolant);
    check = true;
  }

  static unsigned long timeElapsed; //for continuity since millis() changes b/w lines of codes
  if (check == true){
    while (digitalRead(w_button) == HIGH ||digitalRead(c_button) == HIGH){} //catches button holds & runs min once

    if (runtimeConfig == water){
      timeElapsed = millis(); 
      runtime = timeElapsed + runtimeWater; //does not = 0, = timeElapsed
    }
    else{
      timeElapsed = millis();
      runtime = timeElapsed + runtimeCoolant;   
    }
    run = true;
  } 


  /* Relay behavior */
  static bool runOnce = 0; 
  static bool pause;
  static unsigned long timeStart; 
  static int minutesLeft = 0;
  static int secondsLeft = 0;
  static bool enablePumps;

  while (run == true){ 
    if(runOnce == 0){ 
      Serial.print("Running...");
      check = false; //elab why this exist
      enablePumps = true;
      timeStart = timeElapsed; //ENSURES TIMESTART = MS WHEN t = 0
      runOnce++;
    }

    if (digitalRead(pinConfig[2]) == HIGH || timeElapsed >= runtime){
      interface.runFinish(runtimeConfig, (timeElapsed-timeStart)); //originally doesnt run during a fuckup
      Serial.println("Finished");
      Serial.println();
      
      digitalWrite(pinConfig[1], OFF); 
      delay(1000);
      digitalWrite(pinConfig[0], OFF);
   
      run = false; 
      pause = true; 
      enablePumps = false;
    } 

    minutesLeft = (runtime-timeElapsed)/60000;
    secondsLeft = (runtime-timeElapsed)/1000 - ((runtime-timeElapsed)/60000)*60;

    timeElapsed = millis(); //update timeElapsed per loop iteration //THIS NEWER DAWG

    if (enablePumps == true){
      digitalWrite(pinConfig[0], ON);
      delay(1000);
      timeStart = timeElapsed; //the actual timeStart
      digitalWrite(pinConfig[1], ON);
      enablePumps = false;
    }
  }
  runOnce = 0;
  while  (digitalRead(pinConfig[2]) == HIGH && pause == true){} //catches button holds
  pause = false;  

}
    




  
