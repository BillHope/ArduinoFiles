//const int Msg_MaxStrLen = 1000;
/*const int Msg_MaxSections = 10;

const Msg_OK = 0;
const Msg_Error = 1;

// range 10 - 99 internal errors 
const Msg_UnexpectedMode  = 10;
const Msg_OutOfSpace      = 11;
const Msg_TooManySections = 12;
const Msg_StringTooLong   = 13;


// range 100 - 199 format issue
const Msg_StartExpected     = 100;
const Msg_LeftChevExpected  = 101;
const Msg_RightChevExpected = 102;

const Msg_Complete         = 200;
const Msg_InComplete       = 201;
const Msg_SectionNotFound  = 202;


const Msg_ValueDifferent   = 220;

const Msg_Section_Instruction = 'I'; // to take an action
const Msg_Section_Request     = 'R'; // request for information
const Msg_Section_Param0      = '0';
const Msg_Section_Param1      = '1';
const Msg_Section_Param2      = '2';
const Msg_Section_Param3      = '3';
const Msg_Section_Param4      = '4';
const Msg_Section_Param5      = '5';
const Msg_Section_Param6      = '6';
const Msg_Section_Param7      = '7';
const Msg_Section_Param8      = '8';
const Msg_Section_Param9      = '9';

*/
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
		char Msg[Msg_MaxStrLen];  // 0 becomes the delimiter
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
	public:
		Msg();          // basically calls reset

		void Reset();	// reset msg
		int AddChar(char c);			
		char* GetSection(char s);
		int Compare(char s,char * s1);

};

Msg::Msg()
{
  Reset();
}

void Msg::Reset()
{
	// to do yet
  iMsgPos     = -1; // will be set correctly when 1st data is detected
  iMsgSection = -1;
  Msg[0] = 0;
  cMode = '$';
};
//---------------------------------------------------------------------------------
int Msg::Compare(char s,char * s1)
{
	if(cMode != '#') return Msg_InComplete;
	char * s2;
	s2 = GetSection(s);
	if(s2 == 0) return Msg_SectionNotFound;
	for(int i = 0,i ++, i < Msg_MaxStrLen)
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
	switch (cMode) // this is what we are expecting
	{
	case '$': // --- waiting to start, expecting a $------------------------------
		if (c == '$') // this is what we are expecting :-)
		{
			Reset(); 
			cMode = '<';
		}
		else // not what we were expecting, still no need to reset
			return Msg_StartExpected;
		break;
	case '<': // --- this should be the < enclosing the section type --------------
		if (c == '<') // this is what we are expecting
			cMode = 'T'; // set to looking for section type
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
		{
		else 
		{
			Reset();
			return Msg_TooManySections;
		}
		break;
	case '>': // --- Type confirmed, prep for data ------------------------------
		if (c == '>') // this is what we are expecting
		}
			cMode = 'D'; // set to looking for section data
			Msg[++MsgPos] = 0;   // increment counter to leave the previous zero terminator
			SectionStart[iMsgSection] = &Msg[iMsgPos];
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
					Msg[iMsgPos++] = c;
					Msg[iMsgPos]   = 0;
				}
				else 
				{
					Reset();
					return Msg_OutOfSpace;
				}
			break;
		}
	case '#':
		return Msg_Complete;
	default:
		Reset();
		return Msg_UnexpectedMode;
		break;
	}
    return Msg_OK;
}; // int Msg::AddChar(char c)
//----------------------------------------------------------------------------------    
char* Msg::GetSection(char s)
{
	char* r = 0;
	if (cMode != '#') return 0;
	for(i = 0;i < iMsgSection;i++)
	{
		if(SectionType[i] = s) r = SectionStart[i];
	}
	return r;
}
//----------------------------------------------------------------------------------
