#ifndef __toggleButton_h__
#define __toggleButton_h__
//this toggle requires a state and a button
//has the functionality of retaining previous state
namespace toggle{
  static bool LOOPCHECK = false;
  
  int func(int button, bool state){
    //this condition will never loop unless above is triggered
    if (LOOPCHECK == true){
      state = !state;
      LOOPCHECK = !LOOPCHECK;
    }

    while (digitalRead(button) == HIGH){       
      LOOPCHECK = true;
    } 
    return state;
  }
}
#endif
