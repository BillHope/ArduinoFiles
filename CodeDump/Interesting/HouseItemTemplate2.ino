/*
prototype for simple house arduino (uno) units :-)
ie simple on off commands handled
will control 8 switches on output pins 2 - 9 inclusive
*/
#include <string.h>
#include <stddef.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <SD.h>


/*
Load the variables needed
  Last int of mac address
  Last int of ip address for this arduino
  All 4 values of mqtt server
  mqtt topic for each of the output pins 
*/
// Update these with values suitable for your network.
// global variables
  int loadedMacAddress = 0; 
  byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, loadedMacAddress };
  mac[5] = loadedMacAddress;
  IPAddress ip(192, 168, 1, 200);
  IPAddress server(192, 168, 1, 30); // this is the development server
  EthernetClient ethClient;
  PubSubClient client(ethClient);
  const char* HallLight            = "House/Lights/btnHallLight";
  const char* LivingRoomLight      = "House/Lights/btnLivingRoomLight";
  const char* DiningRoomTableLight = "House/Lights/btnDiningRoomTableLight";
  // set up variables using the SD utility library functions:
  Sd2Card card;
  SdVolume volume;
  SdFile root;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;


void callback(char* topic, byte* payload, unsigned int length) 
{
  /*
    Wild Cards
  */

  // convert payload to string
  char c[100];
  for(int i=0; i<length; i++)
  {
    c[i] = (char)payload[i];  
  }
  c[length] = 0;

  /*
  match topic to number (array + 2)
  if number between 2 and 9
    incoming MessageNum = topic array + 2
  on_off = HIGH or LOW
  digitalWrite(Message Num, on_off);
  */
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
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
    
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