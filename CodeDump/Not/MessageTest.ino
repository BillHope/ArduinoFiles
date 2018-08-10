//#include <N:\_Bill\Arduino_Files\ClsMsg.ino>
/*
There seems to be an issue with the number
of chars that can be sent in one burst, about 60 chars
*/

const bool Msg_Debug = true;
const int Msg_MaxStrLen = 1000;
const int Msg_MaxSections = 10;

const int Msg_OK = 0;
const int Msg_Error = 1;

// range 10 - 99 internal errors 
const int Msg_UnexpectedMode  = 10;
const int Msg_OutOfSpace      = 11;
const int Msg_TooManySections = 12;
const int Msg_StringTooLong   = 13;

// range 100 - 199 format issue
const int Msg_StartExpected     = 100;
const int Msg_LeftChevExpected  = 101;
const int Msg_RightChevExpected = 102;

const int Msg_Complete          = 200;
const int Msg_InComplete        = 201;
const int Msg_SectionNotFound   = 202;

const int Msg_ValueDifferent    = 220;

const int Msg_Section_Instruction = 'I'; // to take an action
const int Msg_Section_Request     = 'R'; // request for information
const int Msg_Section_Param0      = '0';
const int Msg_Section_Param1      = '1';
const int Msg_Section_Param2      = '2';
const int Msg_Section_Param3      = '3';
const int Msg_Section_Param4      = '4';
const int Msg_Section_Param5      = '5';
const int Msg_Section_Param6      = '6';
const int Msg_Section_Param7      = '7';
const int Msg_Section_Param8      = '8';
const int Msg_Section_Param9      = '9';

/*
$ = start of message
Recipient of Message
# = end of message
< = end of last section, start of new section
$<?>....<?>.....<?>....#
? =
S = Source of message
I = Instruction
R = Recipient of message
1 = Parameter number 1
2 = Parameters number 2 etc
to be extended

Target
Instruction
Parameter

*/
class Msg
{
	private:
		//char cStatus;
		char msg[Msg_MaxStrLen];  // 0 becomes the delimiter
		char*  SectionStart[Msg_MaxSections];
		char SectionType[Msg_MaxSections]; 
		char cMode; 
		// waiting $
		// adding <
		// adding section type 'T'
		// adding >
		// adding section string (add char, end section, end message)
		//
		int iMsgPos;       // same as above, remove???
		int iMsgSection;   // 
		//char sLastMessage[100];
		String ErrorMsg;
	public:
		Msg();          // basically calls reset
		String GetError();
		void Reset();	// reset msg
		int AddChar(char c);			
		char* GetSection(char s);
		int Compare(char s,char * s1);

};

Msg::Msg()
{
  Reset();
}

String Msg::GetError()
{
	return ErrorMsg;
}

void Msg::Reset()
{
	// to do yet
  iMsgPos     = -1; // will be set correctly when 1st data is detected
  iMsgSection = -1;
  msg[0] = 0;
  //sLastMessage[0] = 0;
  cMode = '$';
};
//---------------------------------------------------------------------------------
int Msg::Compare(char s,char * s1)
{
	if(cMode != '#') return Msg_InComplete;
	char * s2;
	s2 = GetSection(s);
	if(s2 == 0) return Msg_SectionNotFound;
	for(int i = 0 ;i < Msg_MaxStrLen; i++)
	{
		if (s1[i] != s2[i]) return Msg_ValueDifferent;
		if (s1[i] == 0) return Msg_OK;
	}
	return Msg_StringTooLong;
}
//----------------------------------------------------------------------------------
int Msg::AddChar(char c) // handle of \n ??
{
     // use the relevent mode
	if (Msg_Debug) 
	{
		Serial.println(c);	
		Serial.println(cMode);
		if(c == '+') Serial.println(GetSection('+'));		
	}	
	switch (cMode) // this is what we are expecting
	{
	case '$': // --- waiting to start, expecting a $------------------------------
		if (c == '$') // this is what we are expecting :-)
		{
			Reset(); 
			cMode = '<';
			if (Msg_Debug) 
			{
				Serial.println("Started Message:");	
				Serial.println(cMode);	
			}
		}
		else // not what we were expecting, still no need to reset
			return Msg_StartExpected;
		break;
	case '<': // --- this should be the < enclosing the section type --------------
		if (c == '<') // this is what we are expecting
		{
			cMode = 'T'; // set to looking for section type
		}
		else
		{
			Reset();
			return Msg_LeftChevExpected;
		}
		break;
	case 'T': // --- Receiving the section type now ------------------------------
		// any char is acceptable here ???
		if (iMsgSection < (Msg_MaxSections - 1))
		{
			SectionType[++iMsgSection] = c;
			cMode = '>';
		}
		else 
		{
			Reset();
			return Msg_TooManySections;
		}
		break;
	case '>': // --- Type confirmed, prep for data ------------------------------
		if (c == '>') // this is what we are expecting
		{
			cMode = 'D'; // set to looking for section data
			msg[++iMsgPos] = 0;   // increment counter to leave the previous zero terminator
			SectionStart[iMsgSection] = &msg[iMsgPos];
		}
		else
		{
			Reset();
			return Msg_RightChevExpected;
		}
		break;	
	case 'D':
		switch (c)
		{
			case '<': // this ends the previous section and starts a new one
				cMode = 'T';
				break;
			case '#': // this ends the Message
				cMode = '#';
				break;
			default: // add char to message section
				if (iMsgPos < (Msg_MaxStrLen - 1))
				{
					msg[iMsgPos++] = c;
					msg[iMsgPos]   = 0;
				}
				else 
				{
					Reset();
					return Msg_OutOfSpace;
				}
			break;
		}
		break;
	case '#':
		return Msg_Complete;
	default:
		Reset();
		return Msg_UnexpectedMode;
		break;
	}
	if (Msg_Debug) Serial.println("Added a char: ");
    return Msg_OK;
}; // int Msg::AddChar(char c)
//----------------------------------------------------------------------------------    
char* Msg::GetSection(char s)
{
	char* r = 0;
	if (Msg_Debug) Serial.println("In Get Section");
	if (cMode != '#') return 0;
	if (Msg_Debug) Serial.println(iMsgSection);
	for(int i = 0;i <= iMsgSection;i++)
	{
		if (Msg_Debug) Serial.println(i);
		if(SectionType[i] == s) r = SectionStart[i];
	}
	return r;
}
//----------------------------------------------------------------------------------

Msg M;

void setup()
{
  Serial.begin(9600);
  M.AddChar('$');
}

void loop()
{
	while (Serial.available() > 0) 
	{
		int iError = M.AddChar(Serial.read());
		if( iError != 0)
		{
			Serial.println("Issue with Message Object");
			Serial.println(iError);
		}
	}
	Serial.println(M.GetSection('i'));
	Serial.println(M.GetSection('1'));
	delay(2000);
}

