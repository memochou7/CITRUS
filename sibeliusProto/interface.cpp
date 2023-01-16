#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "interface.h"

extern bool cncNum;
extern bool overrideTrigger;
extern bool runtimeConfig;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //randomly assigned
IPAddress server(172, 17, 170, 88); //laptop ip address nt used tho

EthernetClient client;
//true as of etc then have an upd button in the main pg

bool interface::connectPHP() {
  //connects to client

  Serial.print("Obtaining IP...");
  bool i = Ethernet.begin(mac);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  return i;
}
unsigned long interface::obtainConfig(bool runtimeConfig, bool overrideTrigger) {
  //updates limitConfig, runtimeCoolant and runtimeWater

  String buffer = "";
  unsigned long runtime;
  if (client.connect(server, 80)) {
    Serial.println("Connection success");
    client.print("GET /index.php/?type=download"); //change this path if you need
    client.print("&cnc=");
    client.print(cncNum);
    client.print("&cfg=");
    client.print(runtimeConfig);
    client.print("&override=");
    client.print(overrideTrigger);
    
    client.println(" HTTP/1.1");
    client.print("Host: "); //change this to your own
    client.println(server);
    client.println();

    while (client.connected()) {
      if (client.available()) {
        static bool append = false;
        char c = client.read();
        
        //Serial.print(c);

        if (c == 35) { //is #
          append = true;
        }

        if (append == true) {
          if (isDigit(c) == true) {
            buffer += c;
          }
          if (c == 60) { //<
            runtime = buffer.toInt();
          }
        }

        if (c == 62) { //is >

          client.flush();
          client.stop();
          append = false;
          buffer = "";

        }
      }
    }
  } else {
    Serial.println("Failed to download");
    runtime = 0;
  }
  return runtime;
}

//still in production
void interface::runFinish(bool runtimeConfig, unsigned long duration) {
  if (client.connect(server, 80)) {

    client.print("GET /index.php/?type=upload");//change this path if you need
    client.print("&duration=");
    client.print(duration);
    client.print("&runtimeConfig=");
    client.print(runtimeConfig);
    client.print("&cnc=");
    client.println(cncNum);
    client.println(" HTTP/1.1");
    client.print("Host: "); //change this to your own
    client.println(server);
    client.println();

//    while (client.connected()) {
//      if (client.available()) {
//        char c = client.read();
//        Serial.print(c);
//      }
//    }

  } else {
    Serial.println("Failed to upload");
  }


}
