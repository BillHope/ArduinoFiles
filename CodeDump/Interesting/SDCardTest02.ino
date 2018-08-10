#include <SD.h>

int pinCS = 10;
String s;
File f;
/*
Note for the other pins
11 = mosi
12 = miso
13 = sclk
These are set on the SD.H lib
*/

void setup()
{
  Serial.begin(9600);
  Serial.println("Init card");
  
  pinMode(pinCS,OUTPUT);
  digitalWrite(pinCS,LOW);
  // check card is ready
  if (!SD.begin(pinCS))
  {
    Serial.println("card failed");
    return;
  }
  Serial.println("card ready");
}

void loop()
{
  s = "bill is so cool";
  
  f = SD.open("TEST.TXT", FILE_WRITE);
  
  if(f)
  {
    f.println(s);
    f.close();
    Serial.println(s);
  }
  else
  {
    Serial.println("Bummer");
  } 
   delay(5000);
}
