//log
//most of the code(especially any odd ones) in/for loop() is written to optimise the detection of the pump buttons
//list global functionality
//sometimes it doesnt compile or serial doesnt run so restart erveyrt

//individual pump rates
//clear lcd

//rtms same
//conc limit - doesnt pump but doesnt disp warning
//can update upd immediately?

#include <Wire.h>         
#include "interface.h"

#define OFF     1            // To note: For P2H1588A0, 1 = OFF, 0 = ON (i.e. reversed). 
#define ON      0            // therefore ONLY USE THESE FOR P2H1588A0 RELAY
#define water   0
#define coolant 1

const bool enableDisplay        = false; //toggle if you want
const bool force                = false;
const unsigned int updateRate   = 10000;

// pinout setup
const int override_button = 2;
const int select_button   = 3;
const int w_button        = 4;  // for water
const int c_button        = 8;  // for coolant
const int lcdpower        = 7;
const int led1            = 6;
const int led2            = 5;
int LED[2] = {led1, led2};

const int sol1            = 12;
const int sol2            = 11;
const int w_pump          = 10; 
const int c_pump          = 9;
int cncSet[2] = {sol1, sol2}; 

interface interface;



unsigned long limitConfig[6]; // tankCapacity, coolantLvMax, concentration, coolantLv, concentrationLimit, coolantLvLimit

//troubleshooting function
void show(unsigned long val){Serial.print("------>");Serial.println(val);}
void alloff(){
  digitalWrite(sol1, OFF);
  digitalWrite(sol2, OFF);
  digitalWrite(w_pump, OFF);
  digitalWrite(c_pump, OFF);
}

// Function to switch outputs(CNCs)
bool cncNum;
void selectCNC(){

  static bool run = false;
  while (digitalRead(select_button) == HIGH){
    run = true;
  } 
  if (run == true){
    cncNum = !cncNum; 
    interface.store(0, cncNum);
    interface.updateNow(); 
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
    interface.updateNow();
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




//returns runtimes
int flowrate = 200; //0.225
unsigned long volume;
unsigned long optimalVolume;
unsigned long waterLitres;
unsigned long coolantLitres;
unsigned long runtimeCoolant = 0;
unsigned long runtimeWater = 0;
bool warning = false;
void calculations(int runtimeConfig){ 
  if (limitConfig[5] <= limitConfig[3]){ //what a beautiful piece of code
    warning = true;
    runtimeWater = 0;
    runtimeCoolant = 0;
  }
  else{    
    volume = limitConfig[3] * limitConfig[0] /10000; 
    coolantLitres = limitConfig[2] * volume /10000;  
    waterLitres   = (10000-limitConfig[2])*volume/10000; 
    optimalVolume = 10000 * coolantLitres / limitConfig[4]; 

    if (runtimeConfig == coolant){
      runtimeCoolant = (limitConfig[5] * limitConfig[0]) / 10000 ; 
      runtimeCoolant = (runtimeCoolant - volume) / 0.225 * 10; 
      runtimeWater = 0;
    }

  	if (runtimeConfig == water){
      if (limitConfig[2] > limitConfig[4]){
        runtimeWater  = optimalVolume - volume; 
        runtimeWater  = runtimeWater / 0.225 /100 *1000; 
        runtimeCoolant = 0;
      }
    }
 
  }
  interface.updateRuntimes(runtimeWater, runtimeCoolant);
  //math in documention
}

void runFinish(int runtimeConfig, unsigned long timeStart, unsigned long ms){
  unsigned int update_conc = limitConfig[2];
  unsigned int update_cool;
  unsigned int pumped; 
  if (warning == false){
    
    
    pumped = ((ms - timeStart) / 1000 * 0.225 *100); 
    update_cool =  (pumped + volume ) *10000 / limitConfig[0]; 

    if (runtimeConfig == water){
      update_conc = coolantLitres*10000 / (pumped + waterLitres + coolantLitres) ; 
    } 
    limitConfig[2] = update_conc;
    limitConfig[3] = update_cool;
    interface.updateCurrents(update_conc, update_cool);

    calculations(runtimeConfig);
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
  pinMode(lcdpower, OUTPUT);
  pinMode(override_button, INPUT);
  pinMode(select_button, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  alloff();

  if (enableDisplay == true){
    digitalWrite(lcdpower, HIGH);
    interface.startDisplay(lcdpower, enableDisplay);
  }
  else{
    digitalWrite(lcdpower, LOW);
  }  

  delay(1000);

  interface.updateNow();
  cncNum = interface.retrieve(0); //does this even update the local files
  digitalWrite(LED[cncNum], HIGH);
}






void loop() {
  interface.obtainConfig(limitConfig, overrideTrigger); 
  interface.timedUpdateChecks(force, 5000);

  selectCNC();  
  override();                                      
  

  /* Relay setup */
  static bool check = false;
  static bool run = false;
  static int runtimeConfig;
  static unsigned long runtime;
  static int pinConfig[3];       //sol, pump, button    
  pinConfig[0] = cncSet[cncNum]; 
  if (digitalRead(w_button) == HIGH){
    pinConfig[1] = w_pump;
    pinConfig[2] = w_button;
    runtimeConfig = water;
    calculations(runtimeConfig); 
    check = true;
  }

  else if (digitalRead(c_button) == HIGH){
    pinConfig[1] = c_pump;
    pinConfig[2] = c_button;
    runtimeConfig = coolant;
    calculations(runtimeConfig);
    check = true;
  }

  
  static unsigned long ms; //for continuity since millis() changes b/w lines of codes
  while ((digitalRead(w_button) == HIGH && check == true
        ||digitalRead(c_button) == HIGH && check == true)){ //catches button holds & runs min once
    if (runtimeConfig == water){
      ms = millis(); 
      runtime = ms + runtimeWater; //does not = 0, = ms
    }
    else{
      ms = millis();
      runtime = ms + runtimeCoolant;   
    }
    run = true;
  } 


  /* Relay behavior */
  static bool runonce = 0; 
  static bool pause;
  static unsigned long timeStart; 
  static int minutesLeft = 0;
  static int secondsLeft = 0;
  static bool enablepumps;
  while (run == true){ 
    if(runonce == 0){ 
      check = false; //elab why this exist
      enablepumps = true;
      timeStart = ms; //ENSURES TIMESTART = MS WHEN t = 0
      interface.clearDisplay();
      runonce++;
    }


    if (digitalRead(pinConfig[2]) == HIGH 
    || ms >= runtime 
    || runtimeConfig == water && limitConfig[4] >= limitConfig[2] 
    || limitConfig[5] <= limitConfig[3]
    ){
      runFinish(runtimeConfig, timeStart, ms); 

      digitalWrite(pinConfig[1], OFF); 
      delay(1000);
      digitalWrite(pinConfig[0], OFF);
   
      run = false; 
      pause = true; 
      enablepumps = false;

      interface.updateNow();
    } 

    minutesLeft = (runtime-ms)/60000;
    secondsLeft = (runtime-ms)/1000 - ((runtime-ms)/60000)*60;

    ms = millis(); //update ms per loop iteration //THIS NEWER DAWG

    
    if (enablepumps == true){
      digitalWrite(pinConfig[0], ON);
      delay(1000);
      timeStart = ms; //the actual timeStart
      digitalWrite(pinConfig[1], ON);
      enablepumps = false;
    }

    interface.runTimeDisplay(minutesLeft, secondsLeft);

  }
  runonce = 0;
  while  (digitalRead(pinConfig[2]) == HIGH && pause == true){interface.clearDisplay(); } //catches button holds
  pause = false;  

}
    




  

