// change the file for a git test
// and again
// and again


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a second
}
/*
Arduino: 1.8.5 (Windows 10), Board: "Arduino/Genuino Mega or Mega 2560, ATmega2560 (Mega 2560)"

Sketch uses 1462 bytes (0%) of program storage space. Maximum is 253952 bytes.
Global variables use 9 bytes (0%) of dynamic memory, leaving 8183 bytes for local variables. Maximum is 8192 bytes.
An error occurred while uploading the sketch

This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.
*/
