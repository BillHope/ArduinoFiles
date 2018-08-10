#include <Adafruit_GPS.h>
#if ARDUINO >= 100
 #include <SoftwareSerial.h>
#else
  // Older Arduino IDE requires NewSoftSerial, download from:
  // http://arduiniana.org/libraries/newsoftserial/
// #include <NewSoftSerial.h>
 // DO NOT install NewSoftSerial if using Arduino 1.0 or later!
#endif
#include <SD.h>

#if ARDUINO >= 100
  SoftwareSerial mySerial(18, 19);
#else
  NewSoftSerial mySerial(18, 19);
#endif
Adafruit_GPS GPS(&mySerial);


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true
// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

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


//***************************************************************

/****************************************************************
SD Card Buffer Set up (CB) for Card Buffer
****************************************************************/
const int CB_ArraySize   = 70;
const int CB_StringSize  = 45;

class TCardBuffer  // CB for Card Buffer
{
  private:
    char buf[CB_ArraySize][CB_StringSize];
    int ElementLastRead;
    int ElementLastWrote;
    int CycleLastRead;   //number of times the buffer has been filled
    int CycleLastWrote;
    long int lRead;   // used for calculating if we can add more
    long int lWrote;  // and if there is anything to get
    String tester;
    void UpdateLReadAndLWrote();
   public:
    TCardBuffer();
    //~TCardBuffer();
    void  ConfirmAdd(char *p);  
    char* StringAddGetLink();  
    char* StringGet();
    int AnythingToGet();
 
};
//---------------------------------------------------------
TCardBuffer::TCardBuffer()
{
  ElementLastRead  = -1;
  ElementLastWrote = -1;
  CycleLastRead    = 0;
  CycleLastWrote   = 0;
};
//---------------------------------------------------------
char* TCardBuffer::StringAddGetLink()
{
  if((lWrote - lRead) >= CB_ArraySize) // no room to add more
    return 0;
  if ((ElementLastWrote + 2) > CB_ArraySize) // the array has been filled
  {  
    ElementLastWrote = -1;              // reset for next fill cycle
    CycleLastWrote++;                   // capture num of times array has been filled
  }; 
  // avoiding incrementing the element number until it's written
  return (char *)&buf[ElementLastWrote + 1][0];
};
//----------------------------------------------------------
void TCardBuffer::ConfirmAdd(char* p)
{
  if(p == (char*) &buf[ElementLastWrote + 1][0])
  {
    ElementLastWrote++;
    lWrote++;
  }
}
//----------------------------------------------------------------------
int TCardBuffer::AnythingToGet()
{
  return (lWrote - lRead);
}
//----------------------------------------------------------------------
char* TCardBuffer::StringGet()
{
  // check there is something to read
  if(lRead < lWrote)
  { // there is something to read
    if ((ElementLastRead + 2) > CB_ArraySize)
    {
      // prep for next cycle
      ElementLastRead = -1;
      CycleLastRead++;
    }
    ElementLastRead++;
    lRead++;
    return (char *) &buf[ElementLastRead][0];
  }
  else return 0;
};
//-----------------------------------------------------------------------------

// -- global variables --
TCardBuffer CB;
long int count = 0;
boolean TimerOn = false;
volatile long int NoBufferAvailable = 0;
volatile long int BigCount = 0;

/*********** end of SD Card Buffer **********************/
ISR(TIMER1_COMPA_vect)
{  
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time.
  
    
  //timer1 interrupt  currently at 120 frames/sec 
  if(TimerOn)
  {
    BigCount++;
    char* s = CB.StringAddGetLink();
    if(s != 0)
    {
      snprintf(s,CB_StringSize,"%010d,Data goes here",BigCount);
      CB.ConfirmAdd(s);
    } else NoBufferAvailable++; 
  }
}


void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}



//-----------------------------------------------------------------------------
void setup()
{
  
    // connect at 115200 so we can read the GPS fast enuf and
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for MTK - some use 4800
  GPS.begin(9600);
  
  // You can adjust which sentences to have the module emit, below
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data for high update rates!
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // uncomment this line to turn on all the available data - for 9600 baud you'll want 1 Hz rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  
  // Set the update rate
  // Note you must send both commands below to change both the output rate (how often the position
  // is written to the serial line), and the position fix rate.
  // 1 Hz update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  //GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
  // 5 Hz update rate- for 9600 baud you'll have to set the output to RMC or RMCGGA only (see above)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
  // 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
  // Note the position can only be updated at most 5 times a second so it will lag behind serial output.
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
  //GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);
  
  delay(1000);
  
  //***** set up the serial connection  ******
  // Serial.begin(9600);
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
long int fails = 0;
char c = '.';
void loop()
{
  if(FirstLoop)
  {
    f = SD.open("TEST30.TXT", FILE_WRITE);
    f.close();
    FirstLoop = false;
    TimerOn = true;
  }

  if (count < 3000) // do it only once 
  {
    if(CB.AnythingToGet() > 10) // let buffer collect about 30 entries before writing to card
    {
      f = SD.open("TEST30.TXT", FILE_WRITE);
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
        // messing on to give debugging output
        count += entries;
        if (entries > MaxEntries) MaxEntries = entries;
        prints++;
        if (fails != NoBufferAvailable)
        {
          c = '+';
          fails = NoBufferAvailable;
        } else c = '.';
        if(prints > 50)
        {
          Serial.println(c);
          prints = 0;
        }
        else Serial.print(c);
        // end of messing on :-)
      }
    } 
  }
  else 
  {
    if (TimerOn)
    {
      Serial.println("");
      Serial.println("..Wrote to file.. number of entries " + String(count));
      Serial.println("..Largest burst.. number of entries " + String(MaxEntries));
      Serial.println("..Failed to Capture................ " + String(NoBufferAvailable));
    }
    TimerOn = false;
  }
}
