// Msg

// 00000000011111111112222222222333333333344444444445555555555666666666677777777778
// 12345678901234567890123456789012345678901234567890123456789012345678901234567890
// 
// mtAddPin
// addpin,Control/SittingRoom/Lights/LED,13,digitalOutput
// 
#ifndef CLASSMSG_CPP
#define CLASSMSG_CPP
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\const.cpp" 

/* ------------------------------------------------------------------
-- Error handling will print to serial when debug on
------------------------------------------------------------------ */

/* ------------------------------------------------------------------
-- Life of a message

enum MsgStatus
{
	msUnknown,
	msCreated, Adding chars or a string
	msComplete, All characters added, list of words created
	msProcessed, Message has been dealt with (normally just deleted0)
	msInvalid = 100
};


1: Add chars or a string
2: when complete
------------------------------------------------------------------ */

class clMsg
{
private:
	// constants
	static const unsigned short int MaxMsgLength = 300;
	static const unsigned short int MaxWords = 130;
	unsigned short int currentLength; // length of string
	char Msg [MaxMsgLength + 1]; // extra slot for 0 terminator
	char msgForWords [MaxMsgLength + 1]; // extra slot for 0 terminator
	char* wrds[MaxWords]; 
	int numOfWords;
	int msgStatus;

	int createWords();
	void clear();
public:
	// bool getWord(int i,char* s);
	clMsg* msgNext;

	clMsg();
	~clMsg();
	bool completeMsg();
	int getStatus();
	void printWords();
	void addString(char* s);
	char* getString();
	FuncReturn addChar(char c);
	//static void setDebug(bool b);
};
/* ------------------------------------------------------------------
-- Private Methods for msg class
------------------------------------------------------------------ */

int clMsg::createWords(){
  strcpy(msgForWords,Msg);
  if (debug)
  {
  	Serial.println(Msg);
  	Serial.println(msgForWords);
  }
  numOfWords = -1;
  char * pch;
  pch = strtok (msgForWords," ,.-"); // check on these tokens later
  while (pch != NULL)
  {
  	wrds[++numOfWords] = pch;
    pch = strtok (NULL, " ,.-");
    if (debug)
    {
    	Serial.println(pch);
    }
  }
  numOfWords++;
  return numOfWords;
}

/* ------------------------------------------------------------------
-- Public Methods for msg class
------------------------------------------------------------------ */
clMsg::clMsg(){
	clear();
}

clMsg::~clMsg(){
	clear();
}

bool clMsg::completeMsg(){
	if (createWords() > 0){
		msgStatus = msComplete;
		return true;
	}else{
		if (debug)
		{
			Serial.println("No words created string was only delimiters?");
		}
		return false;
	}
}

int clMsg::getStatus(){
	return msgStatus;
}

void clMsg::printWords(){
	if (msgStatus >= msComplete)
	{
		Serial.println("---Message---");
		for (int i = 0; i < numOfWords; i++)
		{
			if (wrds[i] != NULL)
			{
			 	Serial.println(wrds[i]);
			} 
		}
		Serial.println("--- End ---");
	}else if (debug)
	{
		Serial.println("err 002 Tried to print words when message not complete");
	}
}

void clMsg::addString(char* s){
	if (msgStatus == msCreated)
	{
		if (strlen(s) < MaxMsgLength)
			strcpy(Msg,s);
		currentLength = strlen(Msg);
		//createWords();
	}
	else
	if (debug) // (debug)
	{
		Serial.println("err 003 Trying to add string when msg is not created...");
	}
}

char* clMsg::getString(){
	return Msg;
}


void clMsg::clear(){
	currentLength = 0;
	Msg[0] = 0;
	msgForWords[0] = 0;
	numOfWords = 0;	
	msgNext = NULL;
	msgStatus = msCreated;
	for (int i = 0; i < MaxWords; i++)
	{
		wrds[i] = NULL;
	}
}

FuncReturn clMsg::addChar(char c){
	if (msgStatus == msCreated)
	{
		if (debug) Serial.print(c);
		if (c == 13 || c ==10 || c == 0)
		{
			// end of message
			Msg [currentLength] = 0; // should already have been set
			if (currentLength == 0)
			{
				return FR_BlankMsg;
			}
			createWords();
			return FR_EndOfMsg;
		}
		if (currentLength < (MaxMsgLength))
		{
			Msg [currentLength++] = c;
			Msg [currentLength] = 0;
			return FR_OK;
		}
		if (debug)
		{
			Serial.println("*** issue trying to add char, too many chars??? Num is:");
			Serial.println(currentLength);
		}
	}else if (debug)
	{
		Serial.println("err 004 trying to add characters when msg not in created status");
	}
	return FR_NG;
}

/* ------------------------------------------------------------------
-- Msg List
This works as add to the bottom, take off the top.
------------------------------------------------------------------ */

class clMsgList
{
private:
//#ifdef DEBUG
//	static const bool debug = true;
//#else
//	static const bool debug = false;
//#endif
	clMsg* msgBuilding;
	clMsg* msgCurrent;
	clMsg* msgLast;
	int numOfMsgs;
	void createMsg();
	void promoteBuildingMsg();
	void removeAllMsgs();
public:
	clMsg* removeCurrentMsg();
	clMsgList();
	~clMsgList();
	void addString(char* s);
	int howManyMsgs();
	char* getString();
	char* getString(int i);
	void printMsgWords(int i);
	void printMsgWords();
	void addChar(char c);
	void delCurrentMsg();
};
/* ------------------------------------------------------------------
-- Methods for message list
------------------------------------------------------------------ */

clMsgList::~clMsgList(){
	removeAllMsgs();
}

void clMsgList::addChar(char c){
	if (msgBuilding == NULL)
	{
		switch (c){
			case 0:
				if (debug)
					Serial.println("got a 0 as 1st char for message");
				return;
				break;
			case 10:
				if (debug)
					Serial.println("got a 10 as 1st char for message");
				return;
				break;
			case 13:
				if (debug)
					Serial.println("got a 13 as 1st char for message");
				return;
				break;	
			default:
				createMsg();
				break;
		}
	}
	switch (msgBuilding->addChar(c)){
		case FR_OK: break; // still adding chars
		case FR_EndOfMsg:
			promoteBuildingMsg(); 
			break;
		case FR_BlankMsg:
			if (msgBuilding != NULL)
			{
				delete msgBuilding;
				msgBuilding = NULL;
			}
			if (debug)
			{
				Serial.println("Err, Problem Blank msg");
			}
			break;
		default:
			if (msgBuilding != NULL)
			{
				delete msgBuilding;
				msgBuilding = NULL;
			}
			if (debug)
			{
				Serial.println("Err, Problem adding char to msg");
			}
	}
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
		numOfMsgs--;
	}
	return msgCurrent;
}
// ------------------------------------------------------------------
// -- Get the string from a specific msg or the current :-)
// ------------------------------------------------------------------
char* clMsgList::getString(){
	getString(0);
}
char* clMsgList::getString(int i){
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

void clMsgList::printMsgWords(){
	printMsgWords(1);
}

void clMsgList::printMsgWords(int i){
	//Serial.println(numOfMsgs);
	if ((numOfMsgs > 0) and (i < numOfMsgs))
	{
		if (i == 1)
		{
			msgCurrent->printWords();
			Serial.println("should have printed words");
			return;
		}else{
			clMsg* m = msgCurrent->msgNext;
			int n = 1;
			while(n <= i){
				m = m->msgNext;
				i++;
			}
			return m->printWords();
		}
	}
	if (debug)
	{
		Serial.println("Err 001 couldnt print message words");
	}
	return;
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
	msgCurrent = NULL;
	msgBuilding = NULL;
	msgLast = NULL;
	numOfMsgs = 0;
}

void clMsgList::createMsg(){
	if (msgBuilding == NULL)
	{
		msgBuilding = new clMsg();
	}
	else
		if (debug)
		{
			Serial.println("*** building message was already there? ***");
		}
}

void clMsgList::promoteBuildingMsg(){
	if (msgBuilding->completeMsg()){
		numOfMsgs++;
		if (msgCurrent == NULL)
		{
			msgCurrent = msgBuilding;
			msgLast = msgCurrent;
		}
		else
		{
			// add it to the end
			msgLast->msgNext = msgBuilding;
			msgLast = msgLast->msgNext;
		}
	}else{
		delete msgBuilding;
		if (debug)
		{
			Serial.println("discarded building message");
		}
	}
	msgBuilding = NULL;
}

void clMsgList::delCurrentMsg(){
	// check last pointer, if there add on end
	if (msgCurrent != NULL)
	{
		clMsg* temp = msgCurrent->msgNext;
		delete msgCurrent;
		msgCurrent = temp;
		numOfMsgs--;
	}
}

#endif