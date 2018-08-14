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
  Pins.addPin(13,ptDigitalOutput);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ticker == 0)
  {
    ticker = 1;
    Pins.setPinValue(13,100);
  }
  else{
    ticker = 0;
    Pins.setPinValue(13,0);
  }
  delay(1000);
}
