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

const int CB_ArraySize  = 40;

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
    String* StringGet();
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

void TCardBuffer::StringAdd(String *s)
{
  if(!InUse) return;
  if ((ElementLastWrote + 2) > CB_ArraySize) // the array has been filled
  {  
    ElementLastWrote = -1;              // reset for next fill cycle
    CycleLastWrote++;                   // capture num of times array has been filled
  }; 
  // avoiding incrementing the element number until it's written
  sBuf[ElementLastWrote + 1] = "For Debugging: Cycle.." + String(CycleLastWrote) +
      " Element.." + String(ElementLastWrote +1) +
      " and Value: ," + *s;         // add string, inc array pointer
  ElementLastWrote++;
};

int TCardBuffer::AnythingToGet()
{
  int lRead  = (CycleLastRead  * CB_ArraySize) + ElementLastRead;
  int lWrote = (CycleLastWrote * CB_ArraySize) + ElementLastWrote;
  return (lWrote - lRead);
}

String* TCardBuffer::StringGet()
{
  String* ret = 0;
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
    //Serial.println(ElementLastRead);
    ElementLastRead++;
    /**s = "For Debugging: Cycle.." + String(CycleLastRead) +
      " Element.." + String(ElementLastRead) +
      " and Value: ," + 
      */
    return &sBuf[ElementLastRead];
    //Serial.println(*s);
  }
  else
  {
    return ret;
  }
  return ret;
};

TCardBuffer CB;
String LS = "Not set yet";
String *ptrLS = &LS;
int count = 0;
boolean TimerOn = false;

/*********** end of SD Card Buffer **********************/
ISR(TIMER1_COMPA_vect)
{  //timer1 interrupt  currently at 60 frames/sec 
  //Serial.println("In Timer");
  //CB.StringAdd(ptrTS);
  //CB.StringAdd(ptrTS);
  if(TimerOn)
  {
      LS = "Time... " + String(millis());
//      Serial.println(LS);
      CB.StringAdd(ptrLS);    
  }
}

void setup()
{
  //***** set up the serial connection  ******
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for Leonardo only
  // }
  float flt = 2.13;
  int it = flt * 100;
  Serial.print("\nInitializing SD card..." + String(it));
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
  OCR1A = 260;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 60 times per sec
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
boolean FirstLoop = true;

void loop()
{
  if(FirstLoop)
  {
    FirstLoop = false;
    TimerOn = true;
  }

/*  delay(1000);
  TimerOn = false;
  delay(1000);
    if(CB.AnythingToGet())
    {
      f = SD.open("TEST04.TXT", FILE_WRITE);
      if(f)
      {
        int entries = 0;
        int start = millis();
        Serial.println("About to start writing - " + String(start));
        while(CB.StringGet(ptrLS))
        {
          f.println(*ptrLS);
          entries++;
        }   
        Serial.println("..Written to file.. Millis - (" + String(millis()) + " - " + String(start) + ") / " + String(entries));
        f.close();
      }
    } 
  TimerOn = false;
*/  
//  if (count < 100) count++; // without this the count will roll over (signed?)
  if (count < 399) // do it only once 
  {
    // add 60 entries
//    for(int n = 1;n <= 60;n++)
//    {
//      LS = "Dummy Entry number... " + String(count) + " : " + String(n);
//      Serial.println(LS);
//      CB.StringAdd(ptrLS);    
//    }
    // open text file, write the entire buffer then close file
    if(CB.AnythingToGet() > 5)
    {
      f = SD.open("TEST14.TXT", FILE_WRITE);
      if(f)
      {
        f.println("----------------------------------------------");
        int entries = 0;
        String *s;
        while((s = CB.StringGet()) != 0)
        {
          //f.println("..............................................................");
          f.println(*s);
          entries++;
        }   
        f.close();
        Serial.println("..Writing to file.. number of entries " + String(entries));
        count += entries;
      }
    } 
  }
  else TimerOn = false;
//  delay(500);
}
