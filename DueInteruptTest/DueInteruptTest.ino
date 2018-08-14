#include "DueTimer.h"
// #include "SomeSerial.h"
#include <Adafruit_GPS.h>

volatile int pollCount = 0;
const int myLed = 13;
#define GPSSerial Serial1;
bool ledOn = false;
#define GPSECHO true;

String s;
int iSerialInput = 0;
char c;
Adafruit_GPS GPS(&Serial1);
/**************************************************************
Interupt that polls the GPS unit, every 100 milli seconds
This is on timer 3 (not for any particular reason)
**************************************************************/
void GPSTimer(){
  ledOn = !ledOn;
  Serial.println("This is not in use");
  //digitalWrite(myLed, ledOn); // Led on, off, on, off...
}

/**************************************************************
Interupt that polls all the other input, every 20 milli seconds
This is on timer 3 (not for any particular reason)
**************************************************************/
void OtherTimer(){
  //ledOn = !ledOn;
  //Serial.println("Other data will be here");
  //digitalWrite(myLed, ledOn); // Led on, off, on, off...
  // if(pollCount++ > 3){
  if(true){
    pollCount = 0;
   // Serial.println("GPS data will be here");

    if((c = Serial1.read()) != 0)
    {
     // loop while there is something to get
      while(c != 10) // loop while there is something to get Relying on 10?
      {
        Serial.write(c);
        c = Serial1.read();
      }
      Serial.print("\n");
      // Serial.write(c);
    }
  }
}

/****************************************************************
Set Up Routine

****************************************************************/
void setup(){
  pinMode(myLed, OUTPUT);

  Timer2.attachInterrupt(OtherTimer);
  Timer3.attachInterrupt(GPSTimer);
  
  //Timer3.start(100000); // Calls every 100ms
  Serial1.begin(9600);   // standard hardware (USB) serial
  //while(Serial1.available() > 0);
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
   GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
  // 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
  // Note the position can only be updated at most 5 times a second so it will lag behind serial output.
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
  //GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  Serial.begin(9600);   // standard hardware (USB) serial
  //while(Serial.available() > 0);
  delay(10000);   // give things a chance to init and catch up
  Serial.println(PMTK_Q_RELEASE);
  Serial.println("Starting.........");
  Timer2.start( 100000); // Calls every  100ms (10 times a second)
}
/****************************************************************
Nothing in the loop at the moment :-)

****************************************************************/

void loop(){

  while(1){
    // I'm stuck in here! help me...
  }
  
}









