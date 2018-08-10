#include <SD.h>
#define MOSI 8
#define SOMI 9
#define SCK  10
#define CS   11

File dataFile;
String sData;

void setup()
{
  Serial.begin(9600);
  Serial.println("Initilising card");
  
  
  pinMode(MOSI, OUTPUT);  
  pinMode(CS, OUTPUT);
  pinMode(SOMI, INPUT);
  
  if(!SD.begin(MOSI))
  {
    Serial.println("Card failed");
    return;
  }
  
  dataFile = SD.open("Bill.txt",FILE_WRITE);

  if(!dataFile)
  {
    Serial.println("open data file failed");
    return;
  }
}


void loop()
{
  sData = "Hello";
  dataFile.println(sData);
  dataFile.close();
  Serial.println(sData);
  return;
}

