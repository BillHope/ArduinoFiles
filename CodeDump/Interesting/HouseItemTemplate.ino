/*
prototype for house arduino units :-)

communication with system via network and mqtt

to  do

Each unit needs an id ??ip address??

Also a list of the items to change for different units

*/
#include <string.h>
#include <stddef.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
// global variables
  byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  IPAddress ip(192, 168, 1, 200);
  IPAddress server(192, 168, 1, 30); // this is the development server
  EthernetClient ethClient;
  PubSubClient client(ethClient);
  const char* HallLight       = "House/Lights/btnHallLight";
  const char* LivingRoomLight = "House/Lights/btnLivingRoomLight";
  const char* DiningRoomTableLight = "House/Lights/btnDiningRoomTableLight";


void callback(char* topic, byte* payload, unsigned int length) {
  /*
    Wild Cards
  */

  // convert payload to string
  char c[100];
  for(int i=0; i<length; i++){
    c[i] = (char)payload[i];  
  }
  c[length] = 0;

  Serial.print(topic);
  int rc = 1;
  // check for Hall Light
  rc = strcmp(topic,HallLight);
  if (rc == 0){
      Serial.print("Hall light detected\n");
      Serial.println((char*)c);
      rc = strcmp((char*)c,"on");
      if(rc == 0){
        digitalWrite(5, HIGH);
        Serial.print("Hall light on\n");
      }else{
        digitalWrite(5, LOW);
        Serial.print("Hall light off\n");
      }     
  }

  // check for Living Room Light
  rc = strcmp(topic,LivingRoomLight);
  if (rc == 0){
      Serial.print("Living Room light detected\n");
      Serial.println((char*)c);
      rc = strcmp((char*)c,"on");
      if(rc == 0){
        digitalWrite(6, HIGH);
        Serial.print("Living Room light on\n");
      }else{
        digitalWrite(6, LOW);
        Serial.print("Living light off\n");
      }     
  }

  rc = strcmp(topic,DiningRoomTableLight);
  if (rc == 0){
      Serial.print("Dining Room Table light detected\n");
      Serial.println((char*)c);
      rc = strcmp((char*)c,"on");
      if(rc == 0){
        digitalWrite(7, HIGH);
        Serial.print("Dining Room Table light on\n");
      }else{
        digitalWrite(7, LOW);
        Serial.print("Dining Room Table light off\n");
      }     
  }

  char s[100];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe
      // client.subscribe("House/Lights/btnHallLight");
      // client.subscribe("House/Lights/btnLivingRoomLight");
      client.subscribe("House/Lights/#");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  }

void setup(){
  // trying to slow down the setup to allow the ethernet shield to complete 1st???
  delay(5000);
  // this has increased the voltage but it is still only outputting around 2 volts

  // set a conservative baud rate
  Serial.begin(9600);
  
  // set up the ethernet stuff
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);

  // Allow the hardware to sort itself out
  delay(1500);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.print("At the end of set up\n");
  Serial.print("--------------------\n");
  }

int counter;
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}