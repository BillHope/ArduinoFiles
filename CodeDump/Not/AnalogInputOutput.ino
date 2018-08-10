// Controlling the modulation of an LED using a potentiometer (variable resistor)

int ledpin = 3;                           // light connected to digital pin 9
int potpin = 3;                           // analog pin used to connect the potentiometer
int val;                                  // variable to read the value from the analog pin 
 
void setup() 
{ 
  // nothing for setup 
} 
 
void loop() 
{ 
  val = analogRead(potpin);               // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 1023, 0, 254);        // scale it to use it with the LED (value between 0 and 255) 
  analogWrite(ledpin, val); 
  delay(10);   
} 
 


