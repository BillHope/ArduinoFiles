// Msg

class clMsg
{
private:
	static const unsigned short int MaxMsgLength = 300;
	unsigned short int currentLength;
	char Msg [MaxMsgLength + 1]; // extra slot for 0 terminator

public:
	clMsg();
	~clMsg();
	void addChar(char c);
	void addString(char* s);
	char* getString();
};

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
	currentLength = 0;
	Msg[0] = 0;
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