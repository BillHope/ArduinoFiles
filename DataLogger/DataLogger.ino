/*
to do:


Version 0.0.0
Use a blue tooth module to transmit to Race Chrono on a phone
Read the gps
  position
  speed
Read Brake Switch on/off
read 'sync switch for video sync


Modify switch so on is on and off is off :-)

Add snyc button

Capture info from snycbuttton and GPS

## not in this release ##

LED for satelites found




*/

#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

// -- perhaps can be pointed to other serial port
#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
//void useInterrupt(boolean b); // Func prototype keeps Arduino 0023 happy

// ----------------
// GLOBAL VARIABLES
// ----------------
//****************************************************************
// For the SD Card
// ---------------
Sd2Card card;
SdVolume volume;
SdFile root;
//
const int chipSelect      = 53;    
const int SDpinCS         = 53;
const int pinRecordBtn    = 7;
const int ledPower        = 13; //6;
const int ledRecording    = 5;
const int ledGPSconnected = 4;

const int issueGeneral    = 1;
// -----------------
// general variables
// -----------------
String   s;
File     f;
int      n             = 0;
int      i             = 0;
int      iDelay        = 0;
char     c             = '.';
// ----------------------------------
// variable for the file name routine
// ----------------------------------
String       sFileName      = "00000000";
char         cFileName[20];
int          iFileName      = 0;
const String sFileExtension = ".DAT";
// status variables
// ----------------
boolean isRecording    = false;
boolean isRecBtnDown   = false;
boolean isGPSconnected = false;
boolean isIssue        = false;
boolean isPower        = true;
int minEntriesToProcess = 0;

// for capturing GPS data
char SerialInput [1024];
char SerialInputReady [1024];
int iSerialInput;
int SerialCount = 0;


//-------------------------------------------------------------------------------------
// set the name of the data file: format 99999999.DAT
// the process checks the highest 99999999 currently there and adds one to it
// --------------------------------------------------------------------------
void CreateFileName()
{
  File f = SD.open("/");  // set to the root directory on the SD card
  File entry = f.openNextFile();
  while (entry) 
  {
      s = entry.name();
      i = s.length();
      if(i>4)
      {
        if(s.substring(i - 4) == sFileExtension)  // check if this is a .dat
        {
          s = s.substring(0,i-4);                 // get the number value
          n = s.toInt();
          if( n >= iFileName ) iFileName = n;     // set our number to the highest found     
        }
        Serial.println(sFileName);
      }
    entry = f.openNextFile();
  }
  sFileName = String(iFileName + 1);  
  while (sFileName.length() < 8) sFileName = "0" + sFileName;
  sFileName = sFileName + sFileExtension;
  sFileName.toCharArray(cFileName,20);
  entry.close();
  f.close();
}

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
/*********** end of SD Card Buffer **********************/


// -- global variables for timer loop and data collection--
TCardBuffer CB;
volatile long int NoBufferAvailable = 0;
volatile long int BigCount = 0;

// ----------------------------------------------------------
// -- timer loop where data collection and storage is done --
// ----------------------------------------------------------
ISR(TIMER1_COMPA_vect)
{  
  //timer1 interrupt 
  if(isRecording)
  {
    
    while(Serial1.available() > 0)
    {
      iSerialInput = Serial1.read();
      //Serial.write(iSerialInput);
      if(iSerialInput != 13) // start of end of message, ignore
      {
        if((iSerialInput == 10) | (SerialCount > 1000)) // end of message
        {
          SerialInput[SerialCount] = 0;
          // gps has completed a message
          // get other info and add it to the gps
          // gps is the slowest supplier
          BigCount++;
          char* s = CB.StringAddGetLink();
          if(s != 0)
          {
            snprintf(s,CB_StringSize,"%010d,",BigCount);
            // add the sync flag here
            strcat(s,SerialInput);
            CB.ConfirmAdd(s);
          } // else NoBufferAvailable++; 

          Serial.println(SerialInput);
          SerialCount = 0;
//          strcpy(SerialInputReady,SerialInput);
          break;
        }
        else
        {
          SerialInput[SerialCount++] = char(iSerialInput);
        }
      }
    }
    

  }
}

// ------ issue detected --------
void issue(int iError = 0)
{
  isIssue = true;
  switch (iError)
  {
      case issueGeneral: iDelay = 1000;break;
      default          : iDelay = 2000;break;
  }
}
//-----------------------------------------------------------------------------
// ---- SET UP ----
//-----------------------------------------------------------------------------
void setup()
{
  //***** set up the serial connection  ******
  Serial.begin(9600); // up the game for the GPS module
  //Serial.begin(9600);
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  mySerial.begin(9600);
  Serial.print("\nInitializing SD card...");

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!

//#ifdef __arm__
//  usingInterrupt = false;  //NOTE - we don't want to use interrupts on the Due
//#else
//  useInterrupt(true);
//#endif

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

  pinMode(SDpinCS, OUTPUT);   // (10 on most Arduino boards, 53 on the Mega)
  digitalWrite(SDpinCS,LOW);
  
  pinMode(pinRecordBtn,INPUT);  // checking if recordind is switched on
  pinMode(ledPower,    OUTPUT); // set the power light on
  digitalWrite(ledPower,HIGH);
  pinMode(ledRecording,    OUTPUT); // set the power light on
  digitalWrite(ledRecording,LOW);
  pinMode(ledGPSconnected,    OUTPUT); // set the power light on
  digitalWrite(ledGPSconnected,LOW);
  
// -------------------------------------------------------
// - Connect to the SD card and report details to serial -
// -------------------------------------------------------
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

// -------------------------------------------------------
// - Connect to the SD card and report details to serial - end of
// -------------------------------------------------------

  
  // -------------------------
  // set up the timer interupt 
  // -------------------------
  cli();              //stop interrupts
  delay(2000);        // add safety margin
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;         // set entire TCCR1A register to 0
  TCCR1B = 0;         // same for TCCR1B
  TCNT1  = 0;         //initialize counter value to 0
  // set compare match register for 1hz increments
//  OCR1A = 130;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 120 times per sec
//  OCR1A = 260;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 60 times per sec
  OCR1A = 520;// = (16*10^6) / (60*1024) - 1 (must be <65536) ** 30 times per sec
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
  
  SerialInput[0] = 0;
  SerialInputReady[0] = 0;
}

//-----------------------------------------------------------------------------

void loop()
{
//
  if(!isIssue) // avoid any work if issue detected, power down needed
  {
    // -------------------------------------
    // check the recording status is correct
    // -------------------------------------
    i = digitalRead(pinRecordBtn);
    if (i == HIGH) // button is on
    {
      if(!isRecording) // button wasn't pressed before
      { // set up for starting to record
        isRecording  = true; 
        CreateFileName();
        digitalWrite(ledRecording,HIGH);
        minEntriesToProcess = 10; // this is our on going value
      }
    } 
    else
    { 
      if(isRecording) // we were recording, must have just been switched off
      {
        isRecording = false;    // Recording button is not being pressed
        digitalWrite(ledRecording,LOW);
        minEntriesToProcess = 0; // to flush the buffer
      }
    }

    // If there is anything in the buffer write it out
    if(CB.AnythingToGet() > minEntriesToProcess) 
    {
      f = SD.open(cFileName, FILE_WRITE);
      if(f)
      {
        int entries = 0;
        char *s;
        while((s = CB.StringGet()) != 0)
        {
          f.println(s);
          Serial.println(s);
          entries++;
        }   
        f.close();
      }
    } 
  }
    //issue(issueGeneral);             // testing issue function
  else
  {
    if(isPower)
      digitalWrite(ledPower,LOW);
    else
      digitalWrite(ledPower,HIGH);
    delay(iDelay);
    isPower = !isPower;
  }
}

