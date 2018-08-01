
#include <RCSwitch.h>


RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2
}

void loop() {
  if(mySwitch.available()){
  Serial.println(mySwitch.getReceivedValue());
  Serial.println(mySwitch.getReceivedBitlength());
  Serial.println(mySwitch.getReceivedProtocol());
  }
    //output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol());
    mySwitch.resetAvailable();
  
}
