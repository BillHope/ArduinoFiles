int Buzzerpin = 8;
int Photocellpin = 0;
int Photocell_Value = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode( Buzzerpin, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly: 
  Photocell_Value= analogRead ( Photocellpin );
  Serial.println(Photocell_Value);
  digitalWrite(Buzzerpin, HIGH);
  delay(Photocell_Value);
  digitalWrite(Buzzerpin, LOW);
  delay(Photocell_Value);
  
}
