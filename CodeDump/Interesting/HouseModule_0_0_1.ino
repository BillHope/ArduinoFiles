/*

Base House Master Module
------------------------

ver 0.0.0

Pins
0       Hardware Serial (0) RX      :Used for WiFi
1       Hardware Serial (0) TX      :Used for WiFi
2 – 3   Interupts 4 and 5, also PWM :Don’t use yet
4 – 12  PWM                         :9 x dimmers????
  +++ Check if the ethernet connection uses any of these +++
13      Internal led and PWM        :Use for testing.
14 – 17 Software Serial 2 and 3     :drive 2 slave boards
18 – 19 Interupts 2 and 3           :Don’t use yet
20 – 21 SDA,SCL, interupts 0 and 1  :Don’t use yet
22 – 43 Std Dig Pins                :22 standard Digital pins
44 – 46 PWM pins                    :Don’t use yet
47 – 49 Std Dig Pins                :3 standard Digital pins
50 – 53 MISO,MOSI,SCK,CS Mirrored ICSP pins
  +++ used for the SD card reader +++
Don’t use yet, maybe for card reader.



A0 – A15
Analogues in
Use for (16) analogue sensors

A6 + a7 difficult to get to due to board
Only 8 available easily???

Revision 0.0.0
---------------
pin 10 set high
Net mask set
Pin 4 will need to be set for the SD Card use????
Now responding to ping.
Completed

For Revision 0.0.1
------------------

Set up the SD card
Print details of sd card and contents
Completed

For Revision 0.0.2
------------------

Dynamic set up for Arduino Module
- Network card
- Module Id
- Ip Address

For Revision 0.0.3
------------------

POC of IOT

For Revision 0.0.4
------------------

POC for controlling slave modules
--> Create Slave Module


For Revision 0.0.5
------------------

Start building the Arduino Module Functionality
including controlling slave Modules
 */



#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// These two includes failed
//#include <SdFat.h>
//#include <SdFatUtil.h>

//****************************************************************
// variables for using the SD Card
// set up variables using the SD utility library functions:
Sd2Card card;
//SdCard Card;
SdVolume volume;
SdFile root;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF
};
IPAddress ip(192, 168, 1, 177);

//  Set up IP address's. This will be dynamic at the end
// the dns server ip
IPAddress dnServer(8, 8, 8, 8);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 254);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

// -- still not sure how to handle these --
const int pinSPI        = 10;
const int pinCard       =  4;
const int pinChipSelect = 53;

void setup() {

  pinMode(pinSPI,  OUTPUT); // 10 is for the ethernet connection
  pinMode(pinCard,      OUTPUT);  // Works without this
  pinMode(pinCard,OUTPUT);  // must be so :-)
  digitalWrite(pinSPI,HIGH); // Has to be set, card? fails without this
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("\nInitializing SD card... ");

  if (!SD.begin(pinSPI)) // no idea at what speed, assume full
  {
  // if (!card.init(SPI_HALF_SPEED, pinChipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    //return;
  } else {
   Serial.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
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
    //return;
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

  Serial.println("Starting Network Connection.....");

  // start the Ethernet connection:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  delay(1000);
  
}


void loop() {

}




