#ifndef __holdbutton_h__
#define __holdbutton_h__

namespace hold{
  bool state;
  int func(int button){
    if (digitalRead(button) == HIGH){
      state = true;
      return state; 
    } 
    
    else{
      state = false;
      return state; 
    }
}
}
#endif
