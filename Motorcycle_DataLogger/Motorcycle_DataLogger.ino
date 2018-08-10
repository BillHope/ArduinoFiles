
//===================================================================================================================================//
// Programmer:    Ian Cox                                                                                                            //
// Date:          27/07/2014                                                                                                         //
// Program Name:  Motorcycle Data Logger                                                                                             //
// Purpose:       This program is designed to measure and record various data values from an arduino platform. used primarily for    //
//                Data capture on a motorcycle for track days etc.                                                                   //
//===================================================================================================================================//
// Stage Number:  1                                                                                                                  //
// Details:       Stage one of the code is to return meaningful values from the MPU6050 (GY-521) 6 axis accelerometer and gyroscope  //
//                and record them onto an SD card                                                                                    //
//===================================================================================================================================//
// Stage Number:  2                                                                                                                  //
// Details:       Try to integrate the SD.h library to write the values to the SD card in a basic format                             //                                                                                                                   //
//===================================================================================================================================//
// Stage Number:  3                                                                                                                  //
// Details:       Create a wrap around timer control structure based on a timer interrupt. This stage will also perform an initial   //
//                setup routine to create a new CSV file with the headers in place ready to start recording once the GPS has a fix.  //                                                  
//===================================================================================================================================//
//
//
//


//=== Library Accreditations
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include <SD.h>

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu(0x68);
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
 * ========================================================================= */

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
//#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL




#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
boolean cardready = false;
//SD vars
int pinCS = 10;
String s;
File f;



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() 
{
 //Timer interrupt setup - change ref 3 *********************
 //cli();//stop interrupts

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  //OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536) //Need to change this to get 30 frames per second as a starter. 
  OCR1A = 260;// = (16*10^6) / (60*1024) - 1 (must be <65536) //set this up to 60 frames per second. 
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


 // sei();//allow interrupts //commented out as it starts the interru
 //************ End of Change 03 ********************
  
  
  
  //SD Card Section **********************************************   
  Serial.begin(115200);
 pinMode(pinCS,OUTPUT);
 digitalWrite(pinCS, LOW); 
 delay(500);
 /* check card is ready */
 if (!SD.begin(pinCS))
 {
  Serial.println("card failed");
  return;
 }
 Serial.println("card ready");
 cardready = true;

/// Now create the file and populate the header information
f = SD.open("Datalog.csv", FILE_WRITE);
            if(f)
            {
             f.print("Yaw,");
             f.print("Pitch,");
             f.println("Roll");
             f.close();
            }
            else
            {
              Serial.println("Write Failed");
            }
  
  
//6 Axis Acceleromter section**********************************************
  
  //*********************** NO IDEA WHAT THIS PIECE OF CODE DOES **********
  // join I2C bus (I2Cdev library doesn't do this automatically)        //*
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE                    //*
        Wire.begin();                                                   //* 
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)          //*
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE              //*
        Fastwire::setup(400, true);                                     //*
    #endif                                                              //*
  //***********************************************************************

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
   
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection ********  DONT THINK WE NEED THIS ANYMORE - REPLACED BY TIMER INTERRUPT //
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        //mpuIntStatus = mpu.getIntStatus();
        //************************************************************************************************************//
        
        
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
         sei();//allow interrupts
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}




// ================================================================
// ===                    TIMER INTERRUPT LOOP                  ===
// ================================================================

ISR(TIMER1_COMPA_vect)
{//timer1 interrupt  currently at 60 frames/sec 
  if(cardready)
{  

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180/M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180/M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180/M_PI);
            
             //write the information to a CSV file
            f = SD.open("Datalog.csv", FILE_WRITE);
            if(f)
            {
             f.print((ypr[0] * 180/M_PI)*1.12);
             f.print(",");
             f.print((ypr[1] * 180/M_PI)*1.12);
             f.print(",");
             f.println((ypr[2] * 180/M_PI)*1.12);
             f.close();
            }
            else
            {
              Serial.println("Write Failed");
            }
            
            
        #endif

        #ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
            
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
        #endif
    

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}







// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================


void loop() 
{
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    //while (!mpuInterrupt && fifoCount < packetSize) 
   {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
      
    }

}
