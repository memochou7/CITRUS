#ifndef __temperature_h__
#define __temperature__

#include <math.h>
#include <Wire.h>


const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k     // Grove - Temperature Sensor connect to A0




 


 
 
 

namespace temperature{
int func(int pin)
{

    int a = analogRead(pin);
 
    float R = 1023.0/a-1.0;
    R = R0*R;
 
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
    return (temperature);
 

}
}
#endif