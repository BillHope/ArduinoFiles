

int sensorPin = A1;    // select the input pin for the potentiometer
int buttonPin = 8; // Button Digital pin
int basePin = 9;

int buttonState = 0;
int selectedColor = 0;
int currentColor[3];


void setColor()
{
  Serial.println("--Set Color");
  for(int i=0; i < 3; ++i)
  {
    Serial.println(currentColor[i]);
    analogWrite(basePin + i, map(currentColor[i], 0, 255, 255, 0));
    //analogWrite(basePin + i,currentColor[i]);
  }  
}

void setup() {
  Serial.begin(9600);

  currentColor[0] = 0;
  currentColor[1] = 0;
  currentColor[2] = 0;

  pinMode(buttonPin, INPUT);

  pinMode(basePin, OUTPUT);
  pinMode(basePin + 1, OUTPUT);
  pinMode(basePin + 2, OUTPUT);

  setColor();
}

void loop() 
{
  // Read color value from sensor and remap to rgb value range
  int actualColor = map(analogRead(sensorPin), 0,1023,1,255);


  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH)
  {    
    delay(300); // wait for button to return to its initial position
    currentColor[selectedColor] = actualColor;
    setColor();
    selectedColor = (selectedColor + 1) % 3;
  }


}




