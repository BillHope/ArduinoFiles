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
// Includes:
#include <SD.h> //SD Card Library
#include <I2Cdev.h> //Library for IDE used by MPU6050
#include <Wire.h> //required by the i2CDev Library
#include <MPU6050_6Axis_MotionApps20.h> //library to produce real world values from MPU6050


/* MPU6050 variables */
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu(0x68);
//MPU6050 mpu(0x69); // <-- use for AD0 high

#define OUTPUT_READABLE_YAWPITCHROLL

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
float ypr[3];           // [yaw, pitch, roll] Yaw pitch and roll values values


/* SD Card Variables  */
int pinCS = 10; 
String s;
File f;
int counter = 0; //legacy variable from teh original data writing test
int setupcomplete = 0; //flag to see when the setup of the SD card is complete.
boolean WriteEnabled = false; //Flag to signal the SD card is available to be written to.


void setup(){

  
//SD Card 
Serial.begin(115200);
Serial.println("Init card");
pinMode(pinCS,OUTPUT);
digitalWrite(pinCS,LOW);
// check card is ready
if (!SD.begin(pinCS))
{
Serial.println("card failed");
return;
}
Serial.println("card ready");
Serial.print("WriteEnabled1 = ");
Serial.println(WriteEnabled);

f = SD.open("Datalog.txt", FILE_WRITE);
if(f)
 {
  f.print("Index");
  f.print("Yaw,");
  f.print("Pitch,");
  f.println("Roll");
 
  //f.close();
 }
  else
 {
  Serial.println("Writing headers failed");
 }
 
setupcomplete = 1;
Serial.print("setup is complete = ");
Serial.println(setupcomplete);

Serial.println("6 second delay starting");
delay(4000); //delay 6 seconds  before everything kicks off

cli();//stop interrupts
delay(2000);
Serial.println("6 second delay complete");
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
  //TIMSK1 = 0; // disable the timer1 interrupt
 
sei();  
 
/*MPU6050 Code */
 //*********************** NO IDEA WHAT THIS PIECE OF CODE DOES **********
 // join I2C bus (I2Cdev library doesn't do this automatically)        //*
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE                    //*
        Wire.begin();                                                   //* 
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)          //*
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE              //*
        Fastwire::setup(400, true);                                     //*
    #endif                                                              //*
 //***********************************************************************

 // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    
 // Verify Connections   
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

 // load and configure the DMP (Digital Motion Processor -
 // basically a chip on board the MPU6050 that does the heavvy lifting
    Serial.println(F("Initializing DMP..."));
 // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);           // attach the interupt
    devStatus = mpu.dmpInitialize();   
    
    
 // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip  
    
 // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
    {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      Serial.println("Test!!!!");
      delay(1000);
      mpu.setDMPEnabled(true);

 
 
 // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

 // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
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
    
      
      //TIMSK1 |= (1 << OCIE1A); //enable the timer1 interrupt
      Serial.println("timer1 enabled");
      Serial.print("setupcomplete = ");
      Serial.println(setupcomplete);
      Serial.print("WriteEnabled = ");
      Serial.println(WriteEnabled);
      
     WriteEnabled =true; //setup complete enable writing to the card
      
     
}//end setup


// ================================================================
// ===               MPU6050 INTERRUPT  ROUTINE                 ===
// ================================================================



volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  getData();
  Serial.print("getdata worked");
}
  
void getData()
{
   return; 
   mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
      {
        // reset so we can continue cleanly
        mpu.resetFIFO();  
      }
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
     else if (mpuIntStatus & 0x02) 
     {
        // wait for correct available data length, should be a VERY short wait
        Serial.println("Checking packet size.... nice");
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        //REAL WORLD YAW PITCH ROLL CAPTURE
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
     }
    
    
}



// ================================================================
// ===                    TIMER INTERRUPT LOOP                  ===
// ================================================================
ISR(TIMER1_COMPA_vect)
{//timer1 interrupt  currently at 60 frames/sec 

   if((setupcomplete == 1 ) && (WriteEnabled == true)) //if the setup has completed successfully and the card is ready
    {
      //Serial.print("WriteEnabled = " || WriteEnabled);
      counter = counter ++;
      Serial.println(counter );
   
  
      /*f.print(counter); //add the index value
      f.print(", ");
      
      f.print((ypr[0] * 180/M_PI)*1.12); //Yaw
     
      f.print(", ");
      //f.print((ypr[1] * 180/M_PI)*1.12); //Pitch
      f.print((100 * 180/M_PI)*1.12);
      f.print(", ");
      f.println((ypr[2] * 180/M_PI)*1.12); //Roll
      */
      Serial.println("Waiting for interupt");
      //delay(1000);
      //while (!mpuInterrupt);
      //mpuInterrupt = false;
      //getData();
      
      //write values to screen
 /*     Serial.print((ypr[0] * 180/M_PI)*1.12);
      Serial.print(", ");
      Serial.print((ypr[1] * 180/M_PI)*1.12);
      Serial.print(", ");
      Serial.print((ypr[2] * 180/M_PI)*1.12);
      Serial.print(", ");
      Serial.println(ypr[0]);  */
    }
    //else
   // {
    // Serial.println("Write Failed");
   // }
   
}
  

void loop(){
  //do other things here
  delay(4000);
  WriteEnabled = false; //Stop the timer interrupt code from attempting to write to the SD Card
  //f.close();
  
  Serial.println("Write Complete");
  delay(500);
  
  //cli();
  
}
