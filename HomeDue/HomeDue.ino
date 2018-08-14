#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassPin.cpp"
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassMsg.cpp"

clMsg Msg;
clPinList Pins;
int ticker = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  Msg.addString("This is fun");
  delay(5000);
  Serial.println(Msg.getString());
  Pins.addPin(13,"John",ptDigitalOutput);
  Msg.printWords();
}

void loop() {
  // Check for any new messages:
  Msg.clear();
  while(Serial.available()){
    Msg.addChar(Serial.read());
  }
  if(Msg.isPopulated()){ // get this to do any further work necessary for the message
    Msg.printWords();    // Process the message based on msg type (switch)
  }
  if(ticker == 0)
  {
    ticker = 1;
    Pins.setPinValue("John",100);
  }
  else{
    ticker = 0;
    Pins.setPinValue(13,0);
  }
  delay(1000);
}
