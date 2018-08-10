#include <N:\_Bill\Arduino_Files\Counter\Counter.ino>

AverageCounter AC;
int led = 13;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() 
{
    count++;
    if (count > 200) 
    {
        delay(1000);
        count = 0;
    }
    delay(10);
//    digitalWrite(led, HIGH);
    delay(10);
//    digitalWrite(led, LOW);
    AC.addTime(millis());
//    Serial.println(AC.status);
    Serial.println(AC.getElapsedMillis());
    //pinMode(led, OUTPUT);
}
