#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassPin.cpp"
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassMsg.cpp"

clMsg Msg;
clPinList Pins;

// control the polling within the normal loop using millis
unsigned long prevMillis = 0;
unsigned long curMillis = 0;
unsigned long targetMillis = 0;
int pollingInterval = 1000; // half a second
// -------------------------------------------------------


void setup() {

  
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  
  Msg.addString("This is fun");
  delay(3000);
  Serial.println("About to print message");
  //Serial.println(Msg.getString());
  Serial.println("Message has been printed, maybe...");
  delay(2000);
  // Pins.addPin(13,"John",ptDigitalOutput);
  Msg.printWords();
  pinMode(13,OUTPUT);
  // set up the timing measures
  prevMillis = millis();
  curMillis = prevMillis;
  targetMillis = prevMillis + pollingInterval;
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
  Pins.setPinValue(13,0);
  digitalWrite(13,HIGH);
  // delay loop until we have reached the interval
  
   do{
    curMillis = millis();
  } while((curMillis < targetMillis));    
  targetMillis = curMillis + pollingInterval;
  prevMillis = curMillis;
  
digitalWrite(13,LOW);

  Pins.setPinValue(13,100);
  
   do{
    curMillis = millis();
  } while((curMillis < targetMillis));    
  targetMillis = curMillis + pollingInterval;
  prevMillis = curMillis;
  
}

