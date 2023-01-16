#ifndef __INTERFACE_H__
#define __INTERFACE_H__

class interface{
  public:
    bool connectPHP(); 
    unsigned long obtainConfig(bool runtimeConfig, bool overrideTrigger);
    void runFinish(bool runtimeConfig, unsigned long duration);

};
#endif
