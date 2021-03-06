
char* HallLight       = "hmm/House/Lights/btnHallLight";
char aMsg [4][30];

char* house = "House";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println(HallLight);
  delay(2000);
  strSplit(HallLight,aMsg, '/');
  
}

//-----------------------------------------------
// works but need to check for max lengths
//
void strSplit (char* str,char ar [4][30], char delim)
{
  int iStrLen = 0;
  for(int elem = 0; elem < 4; elem++)
  {
    Serial.println(elem);
    int x = 0;
    while( (str[iStrLen] != delim) && (str[iStrLen] != 0) )
    {
       Serial.print("While Loop:");
       Serial.print(str[iStrLen]);
       Serial.println(x);
       ar[elem][x++] = str[iStrLen++];   
    }
    ar[elem][x] = 0; // got all of this param, and end terminator
    iStrLen++;
    Serial.println(ar[elem]);
  }

  if(isMatch(house,aMsg[1]))
    Serial.println("Its house");
  if(isMatch(house,aMsg[0]))
    Serial.println("Its house");
  else
    Serial.println("Elem 0 is not house");
};
//-------------------------------------------------

int isMatch(char*a, char*b)
{
  int i = 0;
  while ((a[i] == b[i]) && (a[i] != 0))
  {
    i++;
  }
  if(a[i] == b[i])
    return 1;
  else
    return 0;
}

//-------------------------------------------------


void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("in main loop");
}
