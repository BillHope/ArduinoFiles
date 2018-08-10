int Relay_control = 2;
int Button = 8;
int Value = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(Relay_control, OUTPUT);
  pinMode(Button,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  digitalWrite(Relay_control,LOW);
  Value=digitalRead(Button);
  if(Value==1)
  {
    digitalWrite(Relay_control,HIGH);
    
  }
  delay(100);
}
