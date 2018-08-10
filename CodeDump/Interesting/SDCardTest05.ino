// location of 'library' ?
// C:\arduino-1.0.5-r2\hardware\arduino\cores\arduino

//-------------------------------------- imported comments for timer ----------------------------
//timer interrupts
//by Amanda Ghassaei
//June 2012
//http://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable all three arduino timer interrupts.
//timer1 will interrupt at 1Hz
// -------------------------- end of imported comments for timer --------------------------------


// Includes -------------------------------------------------------------------------------------
// SD Card Setup -- C:\arduino-1.0.5-r2\libraries
#include <SD.h>
//Note if not in library the full path is in ???
#include <N:\_Bill\Arduino_Files\ClsMsg\ClsMsg.ino>
// end of includes ------------------------------------------------------------------------------

// Arduino Pins
// 10 - used for SD Card = pinCS

// from pins_arduino.h
// static const uint8_t SS   = 10;
// static const uint8_t MOSI = 11;
// static const uint8_t MISO = 12;
// static const uint8_t SCK  = 13;


//SD Card Variables
//	int pinCS = 10;
	String s;
	File f;
	boolean writing = false;

	//storage variables
	int counter = 0;
	int setupcomplete = 0;


void setup(){
  
 //SD Card 
 //M.AddChar('$');

  //set pins as outputs ???
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //pinMode(13, OUTPUT);

  Serial.begin(9600);
  // Serial.begin(115200);
  
  Serial.println("Init card");
  Serial.println(SS);
  //delay(2000);
  //return;
  //SD Code - SS needs to be low to write to card
  pinMode(SS,OUTPUT);
  digitalWrite(SS,LOW);
//  pinMode(MOSI,OUTPUT);
  pinMode(SCK, OUTPUT);
// check card is ready
if (!SD.begin(SS))
{
Serial.println("card failed");
return;
}
Serial.println("card ready");

f = SD.open("Data10.txt", FILE_WRITE);

setupcomplete = 1;
Serial.print("setup is complete = ");
Serial.println(setupcomplete);


delay(2000);

cli();//stop interrupts
//set timer1 interrupt at 1Hz
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
sei();//allow interrupts
}//end setup

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  
   Serial.println(setupcomplete);
   if(setupcomplete == 1)
    {
    counter = counter ++;
    Serial.println(counter);
    //f = SD.open("Data.txt", FILE_WRITE);
    if(f && writing)
    {
      f.println(counter);
      //f.close();
    }
    //else
    //{
     //Serial.println("Write Failed");
    //}
   }
}
  

void loop(){
  //do other things here
  delay(10000);
  writing = false;
  f.close();
  
  Serial.println("Write Complete");
  delay(500);
  cli();
  
  
}

