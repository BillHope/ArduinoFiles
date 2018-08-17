// remove this to switch off debugging messages
#define DEBUG
/* ------------------------------------------------------------------
-- This keeps in tune with all other files
------------------------------------------------------------------ */
//#ifdef DEBUG
//  const bool debug = true;
//#else
//  const bool debug = false;
//#endif

#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassPin.cpp"
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\ClassMsg.cpp"
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\const.h"

/* ------------------------------------------------------------------
-- The pinters for the 'single' objects per arduino
------------------------------------------------------------------ */
clMsg* msg;
clMsgList* ml;
clPinList* pins;
clPin* pin;

/* ------------------------------------------------------------------
-- control the polling within the normal loop using millis
------------------------------------------------------------------ */
unsigned long prevMillis = 0;
unsigned long curMillis = 0;
unsigned long targetMillis = 0;
int pollingInterval = 100; // 1/10 of a second
// -------------------------------------------------------


void setup() {

  
  /* ------------------------------------------------------------------
  -- Set up the hardware serial for coms with the Pi
  ------------------------------------------------------------------ */
  Serial.begin(9600);
  while(!Serial);
  
  /* ------------------------------------------------------------------
  -- Set the delay for the loop rather than an interupt
  ------------------------------------------------------------------ */
  prevMillis = millis();
  curMillis = prevMillis;
  targetMillis = prevMillis + pollingInterval;

  /* ------------------------------------------------------------------
  -- Create the objects that will last for the program
  ------------------------------------------------------------------ */
  ml = new clMsgList();
  pins = new clPinList();

}

/* ------------------------------------------------------------------
-- variables used in the main loop
------------------------------------------------------------------ */
int numOfMsgs = 0;

void loop() {

  /* ------------------------------------------------------------------
  -- Check and get all messages
  ------------------------------------------------------------------ */
  while(Serial.available()){
    ml->addChar(Serial.read());
  }

  /* ------------------------------------------------------------------
  -- Action all the messages that are ready
  -- this would include 'things' managed here
  ------------------------------------------------------------------ */
  
  // ------------------------------------------------------------------
  // -- Check the number of messages
  // ------------------------------------------------------------------
  if (ml->howManyMsgs() > numOfMsgs)
  {
    numOfMsgs++;
    Serial.println(numOfMsgs);
  }

  if (ml->howManyMsgs() > 1)
  {
    //numOfMsgs++;
    ml->printMsgWords();
    ml->removeCurrentMsg();
  }
  /* ------------------------------------------------------------------
  -- Check all the inputs and send messages as required
  ------------------------------------------------------------------ */
  /*
   * 
   do{
    curMillis = millis();
  } while((curMillis < targetMillis));    
  targetMillis = curMillis + pollingInterval;
  prevMillis = curMillis;
  */
   
}

