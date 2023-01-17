#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "interface.h"

extern bool cncNum;
extern bool overrideTrigger;
extern bool runtimeConfig;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //randomly assigned
IPAddress server(172, 17, 170, 74); //change this to your server's private ip

EthernetClient client;

bool interface::connectPHP() {
  Serial.print("Obtaining IP...");
  bool i = Ethernet.begin(mac);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  return i;
}

unsigned long interface::obtainConfig(bool runtimeConfig, bool overrideTrigger) {
  String buffer = "";
  unsigned long runtime;
  if (client.connect(server, 80)) {
    Serial.println("Connection success");
    client.print("GET /CITRUS/main.php/?type=download"); //change this path if you need to
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
        //Serial.print(c); //uncomment this to print out the website for debugging

        if (c == 35) { //is #, signifies start of text to read
          append = true;
        }

        if (append == true) {
          if (isDigit(c) == true) {
            buffer += c;
          }
          
          if (c == 60) { //is <, signifies end of text to read
            runtime = buffer.toInt();
          } 
        }

        if (c == 62) { //is >, signifies end of html page
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

void interface::runFinish(bool runtimeConfig, unsigned long duration) {
  if (client.connect(server, 80)) {
    client.print("GET /CITRUS/main.php/?type=upload"); //change this path if you need
    client.print("&duration=");
    client.print(duration);
    client.print("&runtimeConfig=");
    client.print(runtimeConfig);
    client.print("&cnc=");
    client.println(cncNum);
    client.println(" HTTP/1.1");
    client.print("Host: "); 
    client.println(server);
    client.println();

//    while (client.connected()) { // uncomment this to print out website for debugging
//      if (client.available()) {
//        char c = client.read();
//        Serial.print(c);
//      }
//    }

  } else {
    Serial.println("Failed to upload");
  }


}
