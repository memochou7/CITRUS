#include <Wire.h>            //to do - what about the mixing of concentration
#include "holdbutton.h"
#include "togglebutton.h"
#include "temperature.h"
#include <EEPROM.h>
#include "rgb_lcd.h"


// pinout setup
const int override_button = A0;
const int select_button = A1;
const int w_button      = A2;  // for water
const int c_button      = A3;  // for coolant
const int led1          = 3;
const int led2          = 4;

const int sol1          = 8;
const int sol2          = 7;
const int w_pump        = 6; 
const int c_pump        = 5;

const int testLCD        = 7;

int conc_safety         = 60; // to be input by user
int c_safety            = 50; // to be input by user
int c_level             = 30; // to be replaced by float sensor
int concentration       = 50; // to be replaced by refractometer      

const int r = 195;
const int g = 255;
const int b = 0;

rgb_lcd lcd;


#define OFF  1 // To note: For P2H1588A0, 1 = OFF, 0 = ON (i.e. reversed). ONLY USE THESE FOR P2H1588A0 RELAY
#define ON   0  

void setup() {
  pinMode(testLCD, OUTPUT);
  pinMode(override_button, INPUT);
  digitalWrite(testLCD, LOW);

  Serial.begin(9600);
  //delay(1000);
    Wire.begin();       //start I2C bus thingy, req for lcd func n library
  lcd.begin(16,2); 
  lcd.setRGB(20,20,20);
  lcd.display(); 
  delay(1000);
  Serial.println("start");
  pinMode(select_button, INPUT);
  pinMode(w_button, INPUT); 
  pinMode(c_button, INPUT);
  pinMode(sol1, OUTPUT);
  pinMode(sol2, OUTPUT);
  pinMode(w_pump, OUTPUT);
  pinMode(c_pump, OUTPUT);


  //lcd.display();
  lcd.clear();

  Serial.println("??");
}

void loop() {
  Serial.println("??");
  


  // CNC selection
  static bool cncNum = EEPROM.read(0); //EEPROM writes to permanent memory. Is a workaround for arduino crashing
  static int  cncSet[2] = {sol1, sol2}; 
  static int  sol;

  cncNum = toggle::func(select_button, cncNum); //to be substituted when IOT is introduced
  EEPROM.update(0, cncNum);
  sol = cncSet[cncNum];
    
  
  if (cncNum == 0){         //to be depreciated when IOT is introduced
    digitalWrite(led2, 0);
    digitalWrite(led1, 1);
  }
  else{
    digitalWrite(led2, 1);
    digitalWrite(led1, 0);
  }

  
  // Relay functions


  static int i; //neat little code that runs once

  
  if (cncNum >= 0){
  digitalWrite(sol1, OFF);
  digitalWrite(sol2, OFF);

     
    //water dispense
    i = 0;
    while (hold::func(w_button) == 1 && concentration < conc_safety) { 
      concentration = concentration; //concentration function
      digitalWrite(c_pump, OFF);
      digitalWrite(sol, ON);
         
      if(i == 0){
        delay(1000);
        i++;      
      }
      digitalWrite(w_pump, ON);
      Serial.println("sumn");
    }
    digitalWrite(w_pump, OFF);
    digitalWrite(sol, OFF);
    
    //coolant dispense
    i = 0;
    while (hold::func(c_button ) == 1 && c_level < c_safety){
      c_level = c_level; //level function      
      digitalWrite(w_pump, OFF);
      digitalWrite(sol, ON);
      if(i == 0){ 
        delay(2000);
        i++;
      }
      
      digitalWrite(c_pump, ON);       Serial.println("sumn");

    }
    digitalWrite(c_pump, OFF);
    digitalWrite(sol, OFF);
  }
}


  
