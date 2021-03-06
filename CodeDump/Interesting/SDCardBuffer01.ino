#include <SD.h>

//****************************************************************
// variables for using the SD Card
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;    
const int SDpinCS = 10;

String s;
File f;
/*
Note for the other pins
11 = mosi
12 = miso
13 = sclk
These are set on the SD.H lib
*/
//***************************************************************

/****************************************************************
SD Card Buffer Set up (CB) for Card Buffer


****************************************************************/
/*
const int CB_ArraySize = 100;

class TCardBuffer  // CB for Card Buffer
{
  private:
    String sBuf[CB_ArraySize];
    int ElementLastRead;
    int ElementLastWrote;
    int CycleLastRead;   //number of times the buffer has been filled
    int CycleLastWrote;
    String tester;
    boolean InUse;
  public:
  TCardBuffer();
  //~TCardBuffer();
  void    StringAdd(String *s);  
  boolean StringGet(String *s);
  void SwitchOff(){InUse = false;};
  
};

// -- Constructor --
TCardBuffer::TCardBuffer()
{
  ElementLastRead  = -1;
  ElementLastWrote = -1;
  CycleLastRead    = 0;
  CycleLastWrote   = 0;
  InUse = true;
};

void TCardBuffer::StringAdd(String *s)
{
  //Serial.print("In strint add: incoming string  = ");
  //Serial.print(*s);
  //Serial.println(" <I really was here>");
  if(!InUse) return;
  if ((ElementLastWrote + 2) >= CB_ArraySize) // the array has been filled
  {  
    ElementLastWrote = -1;              // reset for next fill cycle
    CycleLastWrote++;                   // capture num of times array has been filled
  }; 
  //delay(100);
  sBuf[++ElementLastWrote] = *s;         // add string, inc array pointer
  //tester = *s;
};

boolean TCardBuffer::StringGet(String *s)
{
  if(!InUse) return false;
  // convert the cycle plus position into linear number
  int lRead  = (CycleLastRead  * CB_ArraySize) + ElementLastRead;
  int lWrote = (CycleLastWrote * CB_ArraySize) + ElementLastWrote;
  // Check we haven't failed to clear the buffer before being overwritten
  if((lWrote - lRead) > CB_ArraySize)
  {
    //Serial.println("Too slow emptying buffer");
    ElementLastRead = ElementLastWrote;
    CycleLastRead = CycleLastWrote;
    return false;
  }
  // check there is something to read
  if(lRead < lWrote){ // there is something to read
    if ((ElementLastRead + 2) >= CB_ArraySize)
    {
      // prep for next cycle
      ElementLastRead = -1;
      CycleLastRead++;
    }
    //Serial.println(ElementLastRead);
    *s = sBuf[++ElementLastRead];
    //Serial.println(sBuf[ElementLastRead]);
    //*s = sBuf[0];
  }
  else
  {
    *s = "false";
    return false;
  }
  //*s = tester;
  // increment to next element to read
  return true;
};

TCardBuffer CB;
*/
//String TS = "Added by Timer";
//String *ptrTS = &TS;
String LS = "Not set yet";
String *ptrLS = &LS;
int count = 0;
/*********** end of SD Card Buffer **********************/
//ISR(TIMER1_COMPA_vect)
//{//timer1 interrupt  currently at 60 frames/sec 
  //Serial.println("In Timer");
  //CB.StringAdd(ptrTS);
  //CB.StringAdd(ptrTS);
  
//}

void setup()
{
  //***** set up the serial connection  ******
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for Leonardo only
  // }

  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(SDpinCS, OUTPUT);     // change this to 53 on a mega
  digitalWrite(SDpinCS,LOW);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
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
  

  /* --- old set up of serial card
  pinMode(pinCS,OUTPUT);
  digitalWrite(pinCS,LOW);
  // check card is ready
  if (!SD.begin(pinCS))
  {
    Serial.println("card failed");
    return;
  }
  Serial.println("Success - Card ready");
  */
  
  //Serial.println("6 second delay starting");
  //delay(4000); //delay 6 seconds  before everything kicks off
  
  // set up the timer interupt at 60 times per second
//  cli();//stop interrupts
  //Serial.println("6 second delay complete");
  //set timer1 interrupt at 1Hz
  /*
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 260;// = (16*10^6) / (60*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //TIMSK1 = 0; // disable the timer1 interrupt
*/
//  sei();
  Serial.println("Starting.....");

}

void loop()
{
  
//  if (count++ < 10) 
/*  if (false) 
  {
    for(int n = 1;n <= 60;n++)
    {
      LS = "Dummy Entry";
      CB.StringAdd(ptrLS);    
    }
    // open text file, write the entire buffer then close file
    f = SD.open("TEST.TXT", FILE_WRITE);
    if(f)
    {
      Serial.println("..Writing to file..");
      while(CB.StringGet(ptrLS))
      {
        f.println(s);
      }   
      f.close();
    }
    else
    {
      Serial.println("-- Failed to create file pointer --");
    } 
  }
*/     
}
