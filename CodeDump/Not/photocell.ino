int potpin=2;//select the input pin for the photocell
int ledpin=13;//select the pin for the LED 
int val=0;// variable to store the value coming from the sensor 

void setup()
{
pinMode(ledpin,OUTPUT);//declare the ledPin as an OUTPUT  
 
Serial.begin(9600);//To set the baud rate 9600
}
void loop()
{
val=analogRead(potpin);//read the value from the sensor
Serial.println(val);
analogWrite(ledpin,val);
delay(10);
}

