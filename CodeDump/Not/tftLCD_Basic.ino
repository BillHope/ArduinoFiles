
// Learning the basics of adafruits TFT LCD Touch
// adafruit's 2.8" TFT Touch Screen 16bit color 320x240 pixels
// more info at http://www.ladyada.net/products/tfttouchbreakout/#bitmaps
// tested using Arduino IDE 0022 and code base from here https://github.com/adafruit/TFTLCD-Library
// Code by Nathan@sobisource.com 3/5/2012





#include <Adafruit_TFTLCD.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
/* For the 8 data pins:
 Duemilanove/Diecimila/UNO/etc ('168 and '328 chips) microcontoller:
 D0 connects to digital 8
 D1 connects to digital 9
 D2 connects to digital 2
 D3 connects to digital 3
 D4 connects to digital 4
 D5 connects to digital 5
 D6 connects to digital 6
 D7 connects to digital 7
 For Mega's use pins 22 thru 29 (on the double header at the end)
 */


#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0
#define LCD_RESET A4 // you can also just connect RESET to the arduino RESET pin. *if so just //comment line out

#define YP A3  // must be an analog pin, use "An" notation!  A1 for shield
#define XM A2  // must be an analog pin, use "An" notation!  A2 for shield
#define YM 9   // can be a digital pin-----uno=9 mega=23      7 for shield
#define XP 8   // can be a digital pin-----uno=8 mega=22      6 for shield


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);   

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940



// Color definitions - in 5:6:5
#define	BLACK           0x0000
#define	BLUE            0x001F
#define RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//--End of preSetup-------------------------------------------------------------------




//--start of setup-=-=-=--=---=-=--=-=---=-=---
void setup(void) {
   Serial.begin(9600);  //This turns on serial monitor
  tft.reset(); //clears LCD Ram
  digitalWrite(35, HIGH); //on mega 35 I use this for backlight. pin 10 for Shield i think.. or //comment out and run backlight pin to 5v
  
  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-==--=-=-=-=-=-=-=
   // All this idenifies which LCD driver chip your using and tells Ada's GFX LIB's 
  uint16_t identifier = tft.readID();  

  if(identifier == 0x9325) {
//    progmemPrintln(PSTR("Found ILI9325 LCD driver"));
    Serial.println(PSTR("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
//    progmemPrintln(PSTR("Found ILI9328 LCD driver"));
    Serial.println(PSTR("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    // progmemPrintln(PSTR("Found HX8347G LCD driver"));
    Serial.println(PSTR("Found HX8347G LCD driver"));
  } else {
//    progmemPrint(PSTR("Unknown LCD driver chip: "));
    Serial.print(PSTR("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(identifier);
    return;
  }
  tft.begin(identifier);  // this returns the above results. below you can see how I replaced all this once i know which one I have... 
  //tft.begin(0x9328); //<---Here I bypassed the above. cause I already checked...
 //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==--=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 
  
  tft.setRotation(3); // 3=landscape mode -w- row pins on right...
  
  
  
  tft.fillScreen(BLACK);  //Set's LCD Back Ground as Black
  
  
  
  //=-=--Button 1=-=-=-=-=--=--==-=--=-=-=-=-=-=--=-=-=-=--=
  tft.fillRect(25, 70, 100, 75, BLUE);   //our Rectangle box for Button 1 ( x, y, w, h, color)
  tft.setTextSize(2); //Sets all text font size till called again  (Fontsize 1-5) defualt is "1" 
  tft.setTextColor(WHITE);  //Sets all text color till called again
  tft.setCursor(30, 100); //sets cursor to start writing text from.. (x, y)
  tft.println("Button 1");  // Text string to write on lcd
  
  
  
  
  //==-=-=-Button 2=-=-=-=-=-=--=--=--=-=-=--=--=--=-=-==-=-=-
  tft.fillRect(170, 70, 100, 75, BLUE);  // Button 2 box
  //Since were using same text size we dont need to call tft.setTextSize() again..
  tft.setTextColor(RED);  //setting Text color from WHITE to RED   
  tft.setCursor(175, 100);  //changing cursor from last button location to button 2 location   
  tft.println("Button 2");  //button 2 text 
  
}// End of setup

#define MINPRESSURE 10       //check for any touch 0 would mostly be too senitive.
#define MAXPRESSURE 1000




//--Start of loop-----------------------------------
void loop(){
  
   

 

  
  Point p = ts.getPoint(); //checks touch x/y min-max



  // if you're sharing pins, you'll need to fix the directions of the touchscreen pins!
  //pinMode(XP, OUTPUT);    //normally not needed..when used as defualt setup..
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);   //normally not needed..when used as defualt setup..



     // turn from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);//default is (240, 0) [default puts touch cord. 0=x/y upper right.
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);//default is (320, 0) [I change these cause i like 0=xy bottom left.
   

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {    //checks IF theres any touch action then continues
  }

/*     //checks touch x/y if pressed and sends them your serial monitor
    if (p.z > ts.pressureThreshhold) {
   Serial.print("Y = "); Serial.print(p.y);      
   Serial.print("\tX = "); Serial.print(p.x);  //  \t= space
   Serial.print("\tPressure = "); Serial.println(p.z);
   delay(800); //just to slow down the readings a little bit
   }
 
*/
   if (p.y > 28 && p.y < 128 && p.x > 88 && p.x < 166) {  //looks for touch within x/y box area of button 1
     
        // if touch detected within box.  what code to carry out goes next....
        tft.setTextSize(3);
        tft.setTextColor(RED);
        tft.setCursor(12, 213);
        tft.println("button 1 Pressed");  //writes the text "button 1 pressed"
        delay(1000);    //length of time to display message 
        
        //Using fill Rectangle to clear message
        tft.fillRect(12, 213, 290, 21, GREEN);  //Shows the wipe area

        delay(1000); //time between fills so you can see it happen
        tft.fillRect(12, 213, 290, 21, BLACK);  //fills text area with bkgrd color to clear message
      } 

     if (p.y > 177 && p.y < 276 && p.x > 90 && p.x < 167) {  //looks for touch within x/y box area
     
           // if touch detected within box.  what code to carry out goes next....
         tft.setTextSize(3);
        tft.setTextColor(RED);
        tft.setCursor(12, 213);
         tft.println("button 2 Pressed");  //writes the text "button 1 pressed"
        delay(1000);    //length of time to display message 
        
           //write the same letters again using backgrnd colors to clear message
        tft.setTextColor(BLACK);
        tft.setCursor(12, 213);   
        tft.println("button 2 Pressed");
      }
  
      

}// end of loop







