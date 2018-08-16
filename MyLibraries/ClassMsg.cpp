// Msg

// 00000000011111111112222222222333333333344444444445555555555666666666677777777778
// 12345678901234567890123456789012345678901234567890123456789012345678901234567890
// 
// mtAddPin
// addpin,Control/SittingRoom/Lights/LED,13,digitalOutput
// 

enum MsgType
{
	mtUnknown,
	mtAddPin
	
};

enum MsgStatus
{
	msUnknown,
	msCreated,
	msWip,
	msMsgRecd,
	msProcessed,
	msInvalid = 100
};

//bool Class_Msg_debug = false;

/* ------------------------------------------------------------------
-- Error handling will print to serial when debug on
------------------------------------------------------------------ */

class clMsg
{
private:
#ifdef DEBUG
	static const bool debug = true;
#else
	static const bool debug = false;
#endif
	static const unsigned short int MaxMsgLength = 300;
	static const unsigned short int MaxWords = 30;
	unsigned short int currentLength;
	char Msg [MaxMsgLength + 1]; // extra slot for 0 terminator
	char msgForWords [MaxMsgLength + 1]; // extra slot for 0 terminator
	char * wrds[MaxWords]; 
	void createWords();
	int numOfWords;
	bool msgOpen;
public:
	// bool getWord(int i,char* s);
	clMsg* msgNext;
	clMsg();
	~clMsg();
	void addChar(char c);
	void addString(char* s);
	char* getString();
	void printWords();
	void clear();
	bool isPopulated();
	void closeMsg();
	//static void setDebug(bool b);
};
/* ------------------------------------------------------------------
-- Methods for msg class
------------------------------------------------------------------ */

//static void clMsg::setDebug(bool b){
//	clMsg::debug = b;
//}

void clMsg::closeMsg(){
	createWords();
	msgOpen = false;
}

bool clMsg::isPopulated(){
	if (currentLength > 0)
	{
		return true;
	}
	return false;
}

void clMsg::printWords(){
	createWords();
	Serial.println("---Message---");
	for (int i = 0; i < numOfWords; i++)
	{
		Serial.println(wrds[i]);
	}
	Serial.println("--- End ---");
}

void clMsg::createWords(){
  strcpy(msgForWords,Msg);
  numOfWords = 0;
  char * pch;
  pch = strtok (msgForWords," ,.-"); // check on these tokens later
  while (pch != NULL)
  {
  	msgForWords[numOfWords++] = pch;
    pch = strtok (NULL, " ,.-");
  }
}

void clMsg::addString(char* s){
	if (msgOpen)
	{
		if (strlen(s) < MaxMsgLength)
			strcpy(Msg,s);
		currentLength = strlen(Msg);
		createWords();
	}
	else
	if (true) // (debug)
	{
		Serial.println("Trying to add string when msg is closed...");
	}
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
	msgNext = NULL;
	msgOpen = true;
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

/* ------------------------------------------------------------------
-- Msg List
This works as add to the bottom, take off the top.
------------------------------------------------------------------ */

class clMsgList
{
private:
#ifdef DEBUG
	static const bool debug = true;
#else
	static const bool debug = false;
#endif
	clMsg* msgBuilding;
	clMsg* msgCurrent;
	clMsg* msgLast;
	int numOfMsgs;
	void createMsg();
	void promoteBuildingMsg()
	clMsg* removeCurrentMsg();
	void removeAllMsgs();
public:
	clMsgList();
	~clMsgList();
	void addString(char* s);
	int howManyMsgs();
	char* getString(int i = 0);
	//static void setDebug(bool b);
};
/* ------------------------------------------------------------------
-- Methods for message list
------------------------------------------------------------------ */

clMsgList::~clMsgList(){
	removeAllMsgs();
}

void clMsgList::removeAllMsgs(){
	if (msgBuilding != NULL){
		delete msgBuilding;
		msgBuilding = NULL;
	}
	clMsg* temp;
	while(msgCurrent != NULL){
		// going to delete the 1st one, 
		// get address of 2nd (if there is one)
		temp = msgCurrent->msgNext;
		delete msgCurrent;
		// number 1 msg is gone attach whats left to the header
		// so number two is now the new  number one
		msgCurrent = temp;
		if (debug)
		{
			Serial.println("Message Deleted from list - single-");
		}
	}
}

// ------------------------------------------------------------------
// -- After removing the current message returns the new current :-)
// ------------------------------------------------------------------
clMsg* clMsgList::removeCurrentMsg(){
	clMsg* temp;
	if(msgCurrent != NULL){
		temp = msgCurrent->msgNext;
		delete msgCurrent;
		msgCurrent = temp;
	}
	return msgCurrent;
}
// ------------------------------------------------------------------
// -- Get the string from a specific msg or the current :-)
// ------------------------------------------------------------------
char* clMsgList::getString(int i = 0){
	if ((numOfMsgs > 0) and (i < numOfMsgs))
	{
		if (i == 0)
		{
			return msgCurrent->getString();
		}else{
			clMsg* m = msgCurrent->msgNext;
			int n = 1;
			while(n < i){
				m = m->msgNext;
				i++;
			}
			return m->getString();
		}
	}
	if (debug)
	{
		Serial.println("*** String requested from none existing message");
	}
	return NULL;
}

int clMsgList::howManyMsgs(){
	return numOfMsgs;
}

void clMsgList::addString(char* s){
	createMsg(); 
	msgBuilding->addString(s);
	promoteBuildingMsg();
}

clMsgList::clMsgList(){
	msgNext = NULL;
	msgLast = NULL;
	numOfMsgs = 0;
}

void clMsgList::createMsg(){
	if (msgBuilding == NULL)
	{
		msgBuilding = new(clMsg());
	}
	else
		if (debug)
		{
			Serial.println("*** building message was already there? ***");
		}
}

void clMsgList::promoteBuildingMsg(){
	// check last pointer, if there add on end
	numOfMsgs++;
	if (msgCurrent == NULL)
	{
		msgCurrent = msgBuilding;
		msgLast = msgCurrent;
		msgBuilding = NULL;
	}
	else
	{
		// add it to the end
		msgLast->msgNext = msgBuilding;
		msgLast = msgLast->msgNext;
		msgBuilding = NULL;
	}
}