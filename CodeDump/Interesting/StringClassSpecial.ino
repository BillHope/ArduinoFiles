int incomingByte = 0;   // for incoming serial data
char c;
const char cr = '\n';

/*
$ = start of message
Recipient of Message
# = end of message
< = end of last section, start of new section
$<?>....<?>.....<?>....#

Target
Instruction
Parameter

*/
class Str
{
  public:
  char Msg[256];    // 255 chars + a zero
  int MsgLen;       // always contains the masg length in chars
  int MsgPos;       // same as above, remove???
  Str();
  boolean AddChar(char c);
  void Reset();
};

Str::Str()
{
  Reset();
}

void Str::Reset()
{
  MsgLen = 0;
  MsgPos = 0;
  Msg[0] = 0;
};

boolean Str::AddChar(char c) // handle of \n ??
{
  if(MsgPos < 255)
  {
    Msg[MsgPos] = c;
    Msg[MsgPos + 1] = 0;
    MsgPos++;
    MsgLen++;
    return true;
  }
  else return false;
};
    
Str s;
int i;

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        i = 1;
}

void loop() {
  if (Serial.available() > 0){
      Serial.println("----------------");
      Serial.println("Incoming Message");
      Serial.println("----------------");
        // send data only when you receive data:
        while (Serial.available() > 0) {
                // read the incoming byte:
//                incomingByte = Serial.read();
                c = Serial.read();
                s.AddChar(c);
                // say what you got:

                if (s.MsgLen > 5)
                  Serial.println(s.Msg);
        }
  }  
  i = (i<<1);
  if (i != 0)  {Serial.println(i);}
}
