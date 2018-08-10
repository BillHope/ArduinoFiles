#include <U8glib.h>

// D0=12, D1=11, CS=10, DC=9, Reset=8
U8GLIB_SH1106_128X64 u8g(12, 11, 10, 9, 8);


unsigned long tNow  = millis();
unsigned long tThen = millis();
int iScreenSaver = 0;

// loading data from the central server to populate the
const int L = 0;
const int R = 1;

const int ROT_START  = 3;
const int ROT_RIGHT  = 2;
const int ROT_MID    = 0;
const int ROT_LEFT   = 1;
// st int ROT_START  = 3;

int iBtnPress[2]     = {0,0};
int iBtn[2]          = {LOW,LOW};
int iBtnPressPrev[2] = {0,0};
//unsigned long currentTime;

int pinCLK[2]   = {25,23};  // green 48
int pinData[2]  = {24,22};  // green 48
int pinBtn[2]   = {26,27}; // green 49


int iRotaryValue[2] = {50,0}; // default value

// these can be loaded from an SD card, or central server
int rotMax[2]      = {100,4};
int rotMin[2]      = {0,0};
int rotStep[2]     = {4,1};
// int rotDir[2]      = {0,0};
int rotPrevRead[2] = {3,3}; // start position
int rotBounch[2]   = {0,0};
//
int iRotaryTemp = 0; 
int iRotaryPosition = ROT_START;

int stItem; // 0 = off, 1 = On
int iRedrawScreen = 0;

class T_item
{
public:
//  item();
//  ~item();
  char name[30];
};

const int itemMax = 5;
T_item item[itemMax];
int itemCur = 0; 

void checkBtn(int i)
{

  iBtnPress[i] = digitalRead(pinBtn[i]);
  if (iBtnPress[i] != iBtnPressPrev[i])
  {
    if (iScreenSaver )
    {
      iScreenSaver = 0;
      tNow  = millis();
      tThen = millis();
      return;
    }    //Serial.println("Btn has changed");
    // btn has changed
          tThen = millis();
    if (iBtnPress[i] == 0) //then the button has just been released
    {
      iRedrawScreen = 1;
      iBtn[i] = !iBtn[i];
    }
    iBtnPressPrev[i] = iBtnPress[i];
  }
}
//
void rotaryEncoder (int i)
{

/*
 * start 1-1 = 3 ROT_START
 *       1-0 = 2
 *       0-0 = 0
 *       0-1 = 1
 *       1-1 = 3
 */
// get rotary position
  iRotaryTemp = 0;
  if (digitalRead(pinCLK[i])  == HIGH) iRotaryTemp = iRotaryTemp + 1; // add 1
  if (digitalRead(pinData[i]) == HIGH) iRotaryTemp = iRotaryTemp + 2; // add 2

  if (iRotaryTemp != rotPrevRead[i])
  {
    if (iScreenSaver )
    {
      iScreenSaver = 0;
      tNow  = millis();
      tThen = millis();
      return;
    }
          tThen = millis();
    //Serial.println("Dial change");
    switch (iRotaryTemp)
     {
       case ROT_START: // it's landed from a movement
         if (rotPrevRead[i] == ROT_LEFT) rotBounch[i]++; else rotBounch[i]--;
         switch (rotBounch[i])
         {
            case 4: 
              iRotaryValue[i] = iRotaryValue[i] + rotStep[i];
              iRedrawScreen = 1;
              break;
            case -4: 
              iRotaryValue[i] = iRotaryValue[i] - rotStep[i];
              iRedrawScreen = 1;
              break;
         }
         // set values
         if(iRotaryValue[i] > rotMax[i])iRotaryValue[i] = rotMax[i];
         if(iRotaryValue[i] < rotMin[i])iRotaryValue[i] = rotMin[i];
         rotBounch[i] = 0;
         //switch(i)
         //{
         //  case L: Serial.print("Left : ");
         //  break;
         //  case R: Serial.print("Right: ");
         //}
         //Serial.println(iRotaryValue[i]);
         break;
       case ROT_RIGHT: // check direction isnt already set
         if (rotPrevRead[i] == ROT_START) rotBounch[i]++; else rotBounch[i]--;
         //if (rotDir[i] == 0) rotDir[i] = rotStep[i];
         break;
       case ROT_LEFT: // check direction isnt already set
         if (rotPrevRead[i] == ROT_MID) rotBounch[i]++; else rotBounch[i]--;
         //if (rotDir[i] == 0) rotDir[i] = -rotStep[i];
         break;
       case ROT_MID: // check direction isnt already set
         if (rotPrevRead[i] == ROT_RIGHT) rotBounch[i]++; else rotBounch[i]--;
         //if (rotDir[i] == 0) rotDir[i] = rotStep[i];
         break;
     }
     rotPrevRead[i] = iRotaryTemp;
  } 
}

void redrawScreen()
{
  char buf[30];
  int P = 0;
  // This draws the screen
  iRedrawScreen = 0;
  // u8g.setRot90();
   u8g.firstPage();  
  do {
    // font MUST be set
    u8g.setFont(u8g_font_unifont); 
//    u8g.setPrintPos(0,4);
//    u8g.print("Line 1");
    u8g.setPrintPos(0,12);
    //u8g.print("- Kitchen Lights");
    u8g.print(item[iRotaryValue[R]].name);

    // Print Left Dial Info --
    u8g.setPrintPos(0,38);
    u8g.print("Left Dial :");
    if (iRotaryValue[L] < 10) P = 116;
    if (iRotaryValue[L] > 9 and iRotaryValue[L] < 100) P = 108;
    if (iRotaryValue[L] > 99) P = 100;
    u8g.setPrintPos(P,38);
    itoa(iRotaryValue[L],buf,10);
    u8g.print(buf);

    // Print Right Dial info
    u8g.setPrintPos(0,51);
    u8g.print("Right Dial:");
    if (iRotaryValue[R] < 10) P = 116;
    if (iRotaryValue[R] > 9 and iRotaryValue[R] < 100) P = 108;
    if (iRotaryValue[R] > 99) P = 100;    
    u8g.setPrintPos(P,51);
    itoa(iRotaryValue[R],buf,10);
    u8g.print(buf);

    
    u8g.setPrintPos(0,64);
    u8g.print("Sun. 15/01/2017");
    //u8g.drawStr(0, iRotaryValue[L], "Bill Rules OK");
   // u8g.drawStr(0,iRotaryValue[L], "Hum: ");
    //draw();
  } while( u8g.nextPage() );
}

int iWhere = 12;
void drawScreenSaver()
{
  u8g.firstPage();  
  do {
    // font MUST be set
    u8g.setFont(u8g_font_unifont); 
    //u8g.setPrintPos(2,iWhere);
    u8g.drawStr(2, iWhere, "Sun. 15/01/2017");
    //draw();
  } while( u8g.nextPage() );
    // change pos for next time
    switch (iWhere)
    {
      case 12:iWhere = 25;break;
      case 25:iWhere = 38;break;
      case 38:iWhere = 51;break;
      case 51:iWhere = 64;break;
      default: iWhere = 12;
    }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinBtn[L],INPUT);
  pinMode(pinBtn[R],INPUT);
  pinMode(13,OUTPUT);
  // rotary encoder
  pinMode(pinCLK[L],INPUT);
  pinMode(pinCLK[R],INPUT);
  pinMode(pinData[L],INPUT);
  pinMode(pinData[R],INPUT);

  strcpy(item[0].name,"- All Lights ON");
  strcpy(item[1].name,"- All Lights OFF");
  strcpy(item[2].name,"- Kitchen Lights");
  strcpy(item[3].name,"- Door Lights");
  strcpy(item[4].name,"- table Lights");
  //iBtnPress = 0;
  //iBtnPressPrev = 0;
  // currentTime = millis();
}

void loop() 
{
  checkBtn(L);
  checkBtn(R);
  rotaryEncoder(L);
  rotaryEncoder(R);
  if (iRedrawScreen) redrawScreen();
  tNow  = millis();
  if (tNow < tThen) tNow = tThen; // for overflow

  if ((tNow - tThen) > 10000)
  {
    iScreenSaver = 1;
    tThen = tNow;
    drawScreenSaver();
  }
}
