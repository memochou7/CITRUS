#include <Arduino.h> 
#include "interface.h"
#include "rgb_lcd.h"

extern bool cncNum;
extern bool warning; //=true when limits are reached

// Pulls configuration from Excel. Requires PLX-DAQ-v2.11.xlsm and it's macro to be on
rgb_lcd lcd;


void interface::updateNow(){
  update = true;
}

void interface::timedUpdateChecks(bool force, unsigned int duration){
  static unsigned long timer;
  if (timer < millis()){
      if (force == true){
        update = true;
        interface::store(1, update);
      }
      else{
        update = interface::retrieve(1);
      }
    timer = millis() + duration;
  }
}


void interface::obtainConfig(unsigned long limitConfig[6], bool overrideTrigger){
  if (update == true){
    //cncNum = interface::retrieve(0); do the twopart squiffy

    float buffer; //buffer*1000 into limitConfig converts the 4 s.f. float(from excel) into a 4s.f. integer
    column = 66 + cncNum;

    for (int i = 0; i < 5; i++){ //obtaining [0] to [4] 
      Serial.print("CELL,GET,"); 
      Serial.print(column);
      Serial.println(3+i); //3+i 

      buffer = Serial.readStringUntil(10).toFloat();
      buffer = buffer*100;
      limitConfig[i] = buffer;
    }

    if (overrideTrigger == true){ //obtaining [5]
      limitConfig[5] = limitConfig[1];
    }
    else{ 
      Serial.print("CELL,GET,");
      Serial.print(column);
      Serial.println("8");
      buffer = Serial.readStringUntil(10).toFloat();
      buffer = buffer*100;
      limitConfig[5] = buffer;
    }

    update = false;
    interface::store(1, update);

    interface::statusDisplay(limitConfig);
  }
}

void interface::updateRuntimes(unsigned long water, unsigned long coolant){
  column = 66 + cncNum;
  Serial.print("CELL,SET,");
  Serial.print(column);
  Serial.print("10,");
  Serial.println(water);
  Serial.print("CELL,SET,");
  Serial.print(column);
  Serial.print("11,");
  Serial.println(coolant);
}

void interface::updateCurrents(int concentration, int coolant){
  column = 66 + cncNum;
  float buffer;
  buffer = concentration ;
  buffer = buffer / 100;
  Serial.print("CELL,SET,");
  Serial.print(column);
  Serial.print("5,");
  Serial.println(buffer);  

  buffer = coolant;
  buffer = buffer / 100;
  Serial.print("CELL,SET,");
  Serial.print(column);
  Serial.print("6,");
  Serial.println(buffer); 
}


void interface::statusDisplay(unsigned long limitConfig[6]){ //shud trigger when switvh
  static float buffer;
  if (enabled == true){
    if (warning == true){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("LIMITS REACHED");
      Serial.println("-----------LIMITS REACHED-----------");
      delay(2000);
      lcd.clear();
      warning = false;
    }
    buffer = limitConfig[3];
    buffer = buffer/100;
    lcd.setCursor(0,0);
    lcd.print("lv:");
    lcd.print(buffer);
    buffer = limitConfig[5];
    buffer = buffer/100;
    lcd.setCursor(8,0);
    lcd.print("l:");
    lcd.print(buffer);
    buffer = limitConfig[2];
    buffer = buffer/100;
    lcd.setCursor(0,1);
    lcd.print("Cn:");
    lcd.print(buffer);
    buffer = limitConfig[4];
    buffer = buffer/100;
    lcd.setCursor(8,1);
    lcd.print("l:");
    lcd.print(buffer);
  
  }
}

void interface::runTimeDisplay(int minutesLeft, int secondsLeft){  
  if (enabled == true){
    lcd.setCursor(0,0);
    lcd.print("Time left:");
    lcd.setCursor(0,1);
    lcd.print(minutesLeft);
    lcd.print("min ");
    lcd.print(secondsLeft);
    lcd.print("s ");
  }
}

void interface::startDisplay(int lcdpower, bool enableDisplay){
  enabled = enableDisplay;
  if (enabled == true){
    lcd.begin(16,2); 
    lcd.setRGB(0, 0, 0); //color & brightness
    lcd.display(); 
  }
}
void interface::clearDisplay(){
  if (enabled == true){
    lcd.clear();
  }
}


void interface::store(int address, int val){
  Serial.print("CELL,SET,K");
  Serial.print(address+2); //excel storage starts at J2
  Serial.print(",");
  Serial.println(val);
}

int interface::retrieve(int address){
  Serial.print("CELL,GET,K");
  Serial.println(address+2); //excel storage starts at J2
  int val = Serial.readStringUntil(10).toInt();
  return val;
}





