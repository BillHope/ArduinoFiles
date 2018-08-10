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
const int chipSelect = 53;    
const int SDpinCS = 53;

String s;
File f;
File Log;
/*
Note for the other pins
11 = mosi
12 = miso
13 = sclk
These are set on the SD.H lib
*/
#define _writeToLog_ 1
void writeToLog (String s)
{
    Log = SD.open("Log.txt", FILE_WRITE);
    if(Log)
    {
      Log.println(s);
      Log.close();
    }else Serial.println("Write to Log failed");
}

//***************************************************************

/****************************************************************
SD Card Buffer Set up (CB) for Card Buffer


****************************************************************/

const int CB_ArraySize  = 50;
const int CB_StringSize  = 50;

class TCardBuffer  // CB for Card Buffer
{
  private:
    //String sBuf[CB_ArraySize];
    char buf[CB_ArraySize][CB_StringSize];
    int ElementLastRead;
    int ElementLastWrote;
    int CycleLastRead;   //number of times the buffer has been filled
    int CycleLastWrote;
    String tester;
    boolean InUse;
  public:
    TCardBuffer();
    //~TCardBuffer();
    void    ConfirmAdd(char *p);  
//    void    StringAdd(String *s);  
    char*   StringAddGetLink();  
    char* StringGet();
    void SwitchOff(){InUse = false;};
    int AnythingToGet();
 
};

TCardBuffer::TCardBuffer()
{
  ElementLastRead  = -1;
  ElementLastWrote = -1;
  CycleLastRead    = 0;
  CycleLastWrote   = 0;
  InUse = true;
};

char* TCardBuffer::StringAddGetLink()
{
  char *ret = 0;
  if ((ElementLastWrote + 2) > CB_ArraySize) // the array has been filled
  {  
    ElementLastWrote = -1;              // reset for next fill cycle
    CycleLastWrote++;                   // capture num of times array has been filled
  }; 
  // avoiding incrementing the element number until it's written
  return (char *)&buf[ElementLastWrote + 1][0];
};

void TCardBuffer::ConfirmAdd(char* p)
{
  if(p == (char*) &buf[ElementLastWrote + 1][0]) ElementLastWrote++;
}
//----------------------------------------------------------------------
int TCardBuffer::AnythingToGet()
{
  int lRead  = (CycleLastRead  * CB_ArraySize) + ElementLastRead;
  int lWrote = (CycleLastWrote * CB_ArraySize) + ElementLastWrote;
  return (lWrote - lRead);
}
//----------------------------------------------------------------------
char* TCardBuffer::StringGet()
{
  char* ret = 0;
  if(!InUse) return ret;
  // convert the cycle plus position into linear number
  int lRead  = (CycleLastRead  * CB_ArraySize) + ElementLastRead;
  int lWrote = (CycleLastWrote * CB_ArraySize) + ElementLastWrote;
  // Check we haven't failed to clear the buffer before being overwritten
  if((lWrote - lRead) > CB_ArraySize)
  {
    //Serial.println("Too slow emptying buffer");
    ElementLastRead = ElementLastWrote;
    CycleLastRead = CycleLastWrote;
    writeToLog("Too Slow Collecting");
    return ret;
  }
  // check there is something to read
  if(lRead < lWrote){ // there is something to read
    if ((ElementLastRead + 2) > CB_ArraySize)
    {
      // prep for next cycle
      ElementLastRead = -1;
      CycleLastRead++;
    }
    ElementLastRead++;
    return (char *) &buf[ElementLastRead][0];
  }
  else
  {
    return ret;
  }
  return ret;
};
//-----------------------------------------------------------------------------

// -- global variables --
TCardBuffer CB;
long int count = 0;
boolean TimerOn = false;

/*********** end of SD Card Buffer **********************/
ISR(TIMER1_COMPA_vect)
{  
  //timer1 interrupt  currently at 120 frames/sec 
  if(TimerOn)
  {
    char* s = CB.StringAddGetLink();
    if(s != 0)
    {
      snprintf(s,CB_StringSize,"Time in Milli Secs...%d",millis());
      CB.ConfirmAdd(s);
    }  
  }
}
//-----------------------------------------------------------------------------
void setup()
{
  //***** set up the serial connection  ******
  Serial.begin(9600);
  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(SDpinCS, OUTPUT);     // change this to 53 on a mega
  digitalWrite(SDpinCS,LOW);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
//  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
  if (!card.init(SPI_FULL_SPEED, chipSelect)) {
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

  
  // set up the timer interupt at 60 times per second
  cli();//stop interrupts
  delay(2000);
  //Serial.println("6 second delay complete");
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 130;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 60 times per sec
//  OCR1A = 260;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 60 times per sec
//  OCR1A = 1040;// = (16*10^6) / (15*1024) - 1 (must be <65536) ** 15 times per second
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //TIMSK1 = 0; // disable the timer1 interrupt
  sei();

  // OK done all our checks for the SD card, lets begin for file writing
  if (!SD.begin(SDpinCS))
    {
      Serial.println("card failed");
      return;
    }
  Serial.println("------------------------------");
  Serial.println("Starting.....: " + String(millis()));
  Serial.println("------------------------------");
  //TimerOn = true;  
}
//-----------------------------------------------------------------------------
boolean FirstLoop = true;
long int MaxEntries = 0;
int prints = 0;
void loop()
{
  if(FirstLoop)
  {
    f = SD.open("TEST21.TXT", FILE_WRITE);
    f.close();
    FirstLoop = false;
    TimerOn = true;
  }

  if (count < 10000) // do it only once 
  {
    if(CB.AnythingToGet() > 10) // let buffer collect about 30 entries before writing to card
    {
      f = SD.open("TEST21.TXT", FILE_WRITE);
      if(f)
      {
        int entries = 0;
        char *s;
        while((s = CB.StringGet()) != 0)
        {
          f.println(s);
          entries++;
        }   
        f.close();
        //Serial.println("..Writing to file.. number of entries " + String(entries));
        count += entries;
        if (entries > MaxEntries) MaxEntries = entries;
        prints++;
        if(prints > 50)
        {
          Serial.println(".");
          prints = 0;
        }
        else Serial.print(".");
      }
    } 
  }
  else 
  {
    if (TimerOn)
    {
      Serial.println("..Wrote to file.. number of entries " + String(count));
      Serial.println("..Largest burst.. number of entries " + String(MaxEntries));
    }
    TimerOn = false;
  }
}
