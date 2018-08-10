/*
  Telnet client

Base House Master Module
------------------------

ver 0.0.0

Pins
0       Hardware Serial (0) RX      :Used for WiFi
1       Hardware Serial (0) TX      :Used for WiFi
2 – 3   Interupts 4 and 5, also PWM :Don’t use yet
4 – 12  PWM                         :9 x dimmers????
  +++ Check if the ethernet connection uses any of these +++
13      Internal led and PWM        :Use for testing.
14 – 17 Software Serial 2 and 3     :drive 2 slave boards
18 – 19 Interupts 2 and 3           :Don’t use yet
20 – 21 SDA,SCL, interupts 0 and 1  :Don’t use yet
22 – 43 Std Dig Pins                :22 standard Digital pins
44 – 46 PWM pins                    :Don’t use yet
47 – 49 Std Dig Pins                :3 standard Digital pins
50 – 53 MISO,MOSI,SCK,CS Mirrored ICSP pins
  +++ used for the SD card reader +++
Don’t use yet, maybe for card reader.



A0 – A15
Analogues in
Use for (16) analogue sensors

A6 + a7 difficult to get to due to board
Only 8 available easily???

Revision 0.0.0
---------------
pin 10 set high
Net mask set
Pin 4 will need to be set for the SD Card use????
Now responding to ping.
Completed

For Revision 0.0.1
------------------

Set up the SD card
Print details of sd card and contents

For Revision 0.0.2
------------------

Dynamic set up for Arduino Module
- Network card
- Module Id
- Ip Address

For Revision 0.0.3
------------------

POC of IOT

For Revision 0.0.4
------------------

POC for controlling slave modules
--> Create Slave Module


For Revision 0.0.5
------------------

Start building the Arduino Module Functionality
including controlling slave Modules
 */



#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// These two includes failed
//#include <SdFat.h>
//#include <SdFatUtil.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Enter the IP address of the server you're connecting to:
// IPAddress server(1, 1, 1, 1);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
// EthernetClient client;

// the dns server ip
IPAddress dnServer(8, 8, 8, 8);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 254);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

//the IP address is dependent on your network
// IPAddress ip(192, 168, 0, 2);

  
void setup() {

  pinMode(10,OUTPUT);
  digitalWrite(4,HIGH);
  
  // start the Ethernet connection:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  // Ethernet.begin(mac, ip);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("In Setup");

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
 // if (client.connect(server, 10002)) {
 //   Serial.println("connected");
 // } else {
 //   // if you didn't get a connection to the server:
 //   Serial.println("connection failed");
 // }
}

void loop() {
// // if there are incoming bytes available
// // from the server, read them and print them:
// if (client.available()) {
//   char c = client.read();
//   Serial.print(c);
// }

// // as long as there are bytes in the serial queue,
// // read them and send them out the socket if it's open:
// while (Serial.available() > 0) {
//   char inChar = Serial.read();
//   if (client.connected()) {
//     client.print(inChar);
//   }
// }

// // if the server's disconnected, stop the client:
// if (!client.connected()) {
//   Serial.println();
//   Serial.println("disconnecting.");
//   client.stop();
//   // do nothing:
//   while (true);
// }
}




