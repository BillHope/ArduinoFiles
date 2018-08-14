// Msg

// 00000000011111111112222222222333333333344444444445555555555666666666677777777778
// 12345678901234567890123456789012345678901234567890123456789012345678901234567890
// 
// mtAddPin
// addpin,Control/SittingRoom/Lights/LED,13,digitalOutput
// 

enum MsgType
{
	mtAddPin
	
};

class clMsg
{
private:
	static const unsigned short int MaxMsgLength = 300;
	static const unsigned short int MaxWords = 30;
	unsigned short int currentLength;
	char Msg [MaxMsgLength + 1]; // extra slot for 0 terminator
	char MsgBU [MaxMsgLength + 1]; // extra slot for 0 terminator
	char * wrds[MaxWords];
	void getWords();
	int numOfWords;
public:
	clMsg();
	~clMsg();
	void addChar(char c);
	void addString(char* s);
	char* getString();
	void printWords();
	void clear();
	bool isPopulated();
};

bool clMsg::isPopulated(){
	if (currentLength > 0)
	{
		return true;
	}
	return false;
}

void clMsg::printWords(){
	getWords();
	Serial.println("---Message---");
	for (int i = 0; i < numOfWords; i++)
	{
		Serial.println(wrds[i]);
	}
	Serial.println("--- End ---");
}

void clMsg::getWords(){
  numOfWords = 0;
  char * pch;
  pch = strtok (Msg," ,.-"); // check on these tokens later
  while (pch != NULL)
  {
  	wrds[numOfWords++] = pch;
    pch = strtok (NULL, " ,.-");
  }
}


void clMsg::addString(char* s){
	for (int i = 0; i < MaxMsgLength; ++i)
	{
		Msg[i] = s[i];
		if (s[i] = 0)
		{
			currentLength = i;
			return;
		}
	}
	Msg[MaxMsgLength] = 0; // in case s was full length or more
}

char* clMsg::getString(){
	return Msg;
}

clMsg::clMsg(){
	clear();
}

void clMsg::clear(){
	currentLength = 0;
	Msg[0] = 0;
	numOfWords = 0;	
}

clMsg::~clMsg(){
	currentLength = 0;
	Msg[0] = 0;
}

void clMsg::addChar(char c){
	if (currentLength < (MaxMsgLength))
	{
		Msg [currentLength++] = c;
		Msg [currentLength] = 0;
	}
}