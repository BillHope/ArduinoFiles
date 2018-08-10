int ledPin = 13;                // choose the pin for the LED
int touchPin = 0;               // choose the input pin
int touchVal = 0;               // variable for reading the pin

void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
}

void loop(){
  touchVal = analogRead(touchPin);
  
  if(touchVal < 5)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
}
