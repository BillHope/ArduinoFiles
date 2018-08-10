/*

Notes:

This is to be added to the ethernet/mqtt stuff
Then the pins need a bit more investigation so
  we know how many of each type the arduino can handle (always a mega)


  SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
        Pin 4 used here for consistency with other Arduino examples


 created  28 Mar 2011
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 */

 // digital pins we will use 22 - 49
// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

File myFile;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 4;
const int strSize = 100;

const char* configFileName = "house.cfg";
int digPins[] = {22,23,24,25,26,27,28,29,30,31,
                 32,33,34,35,36,37,38,39,40,41,
                 42,43,44,45,46,47,48,49};
int numOfDigPins = 28;
int curDigPin = 0;



char arduinoName[strSize];
int ip1, ip2, ip3, ip4, ip5;
int MacAddress;

struct item //user defined datatype
{
    char itemType; // same value as 
    char area[100];
    char system[100];
    char name[100];
    int digPinIn;
    int digPinOut;
    int anaPinIn;
    int anaPinOut;
    // this is what I'm looking for
};

const int maxItems = 8;
item items[maxItems];
int curItem = 0;

void setup() {
  // Open serial communications and wait for port to open:
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

  delay(2000);
  
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  
  // s = simple on off switch
  myFile = SD.open(configFileName,FILE_READ);
  if (myFile) {
    Serial.println("Successfully opened house.cfg");
    // read from the file until there's nothing else in it:
    char c;
    char line[100];
    int index = 0;
    int i = 0;
    int n;
    int temp;
    
    while (myFile.available()) {
      c = myFile.read(); // 1st char    int digPinIn;
      Serial.print(c);
      //int digPinOut;
      if (i >= (strSize-1)) i = strSize-2;
      switch(c) {
        case 13 : // ignore this, comes before the 10
          break;
        case 10 : // ok we have a complete 'line' in our line string
          line[i]=0;
          switch (line[0]) {
              case '1': // this is the 1st p[art of the address]
                ip1 = atoi(&line[2]);
                Serial.println(ip1);
                break;
              case '2': // this is the 1st p[art of the address]
                ip2 = atoi(&line[2]);
                break;            
              case '3': // this is the 1st p[art of the address]
                ip3 = atoi(&line[2]);
                break;            
              case '4': // this is the 1st p[art of the address]
                ip4 = atoi(&line[2]);
                break;              
              case '5': // this is the 1st p[art of the address]
                ip5 = atoi(&line[2]);
                break;  
              case '6':
                MacAddress = atoi(&line[2]);
                break;            
              case '?': // arduino name
                strcpy(arduinoName,&line[2]);
                Serial.println(arduinoName);
                break;
              case 'a': // simple on_off switch Only has an output pin
              case 'b': // has dig input for status
                items[curItem].itemType = line[0]; // works for all
                switch (c){
                  case 'a': // note that a does both a and b cases
                    items[curItem].digPinOut = digPins[curDigPin++];
                  case 'b' :
                    items[curItem].digPinIn = digPins[curDigPin++];
                    break;
                  // --- other types to add here ---
                  default:
                    break;
                };
                // set n to be the start of the 'line' after the type of info
                n = 2;
                // -----------------------
                // -- populate the area --
                // -----------------------
                temp = 0;
                while(line[n]!= '/'){
                  items[curItem].area[temp] = line[n];
                  temp++;
                  n++;
                }
                items[curItem].area[temp] = 0;
                // -------------------------
                // -- populate the system --
                // -------------------------
                n++; // was pointing to the  '/' move to 1st char of next bit
                temp = 0;
                while(line[n]!= '/'){
                  items[curItem].system[temp] = line[n];
                  temp++;
                  n++;
                }
                items[curItem].system[temp] = 0;
                // -----------------------
                // -- populate the name --
                // -----------------------
                n++;
                strcpy(items[curItem].name,&line[n]);
                break;
              default: // could be empty string, comment or just unknown 1st char
                break;
          }
          // end of processing the line, reset ready for reading next line
          i=0;
          break;
        default :
          line[i] = c;
          i++;
      }
    }

    // close the file:
    n = 0;
    Serial.println(curItem);
    while(n<curItem){
      Serial.println(items[n].area);
      Serial.println(items[n].system);
      Serial.println(items[n].name);
      n++;
    }
    
    Serial.println("About to close file");
    myFile.close();
    Serial.println("About to close file"); 
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening HOUSE.CFG");
  }
  
}

void loop(void)
{
  Serial.println("In Main Loop");
  delay(1000000);

}