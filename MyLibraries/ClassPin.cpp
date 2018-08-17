/* ------------------------------------------------------------------
-- Pin Class

This includes an object that handles the pins as a list.

This is specifically for the Due although it may work on others

Currently the pins are digital only, input and output.
Still to do are the analoque in and out, then the PWM pins

------------------------------------------------------------------ */
#ifndef CLASSPIN_CPP
#define CLASSPIN_CPP
// note the pin creation expects a text version of these
#include "N:\10_General\40_Projects\50_ArduinoFiles\MyLibraries\const.cpp" 

/* ------------------------------------------------------------------
-- All the variables for this code will be prefixed with Class_Pin_
------------------------------------------------------------------ */

const int ClassPin_MaxLenPinName = 100;
bool Class_Pin_debug = false;

/* ------------------------------------------------------------------
-- Pin Code
------------------------------------------------------------------ */

class clPin
{
private:
	PinType pt;   // this will control the behavour of the object
	int pinValue;
	clPin* Prev;
	clPin* Next;
	unsigned short int pinNum;
	char pinName[ClassPin_MaxLenPinName + 1]; 
	void setUp(int num,char* name,PinType t);
public:
	clPin(int num,char* name,PinType t);
	clPin(int num,char* name,char* t);
	~clPin();
	clPin* getPrev();
	clPin* getNext();
	void setPrev(clPin* pin);
	void setNext(clPin* pin);
	int getValue();
	int getNum();
	void setValue(int i);     // set the value of this pin
	bool isName(char* name);  // check the name of this pin
};

clPin::clPin(int num,char* name,char* t){
	PinType pt = ptUnknown; // default value
	if (strcmp(t,"ptDigitalInput")) pt = ptDigitalInput;
	if (strcmp(t,"ptDigitalOutput")) pt = ptDigitalOutput;
	if (strcmp(t,"ptPWM")) pt = ptPWM;
	if (strcmp(t,"ptAnalogueInput")) pt = ptAnalogueInput;
	if (strcmp(t,"ptAnalogueOutput")) pt = ptAnalogueOutput;
	setUp(num,name,pt);
}	

clPin::clPin(int num,char* name,PinType t){
	setUp(num,name,t);
}

void clPin::setUp(int num,char* name, PinType t)
{
	Prev = NULL;
	Next = NULL;
	pinNum = num;
	pinValue = -1;
	pt = t;
	switch (pt){
		case ptDigitalInput:
			pinMode(pinNum, INPUT); // arduino const
			break;
		case ptDigitalOutput:
			pinMode(pinNum, OUTPUT); // arduino const
			break;
		default:
			if(Class_Pin_debug)
				Serial.println("*** Pin Created with unknown type ***");
			break;
	}
	if (strlen(name) <= ClassPin_MaxLenPinName)
	{
		strcpy(pinName,name);
	}
	else pinName[0] = 0;
}
// ------------------------------------------------------------------
clPin::~clPin(){
	//pt = PT;
}
// ------------------------------------------------------------------
clPin* clPin::getNext(){
	return Next;
}
// ------------------------------------------------------------------
void clPin::setNext(clPin* p){
	Next = p;
}
// ------------------------------------------------------------------
void clPin::setPrev(clPin* p){
	Prev = p;
}
// ------------------------------------------------------------------
clPin* clPin::getPrev(){
	return Prev;
}
// ------------------------------------------------------------------
int clPin::getValue(){
	return pinValue;
}
// ------------------------------------------------------------------
int clPin::getNum(){
	return pinNum;
}
// ------------------------------------------------------------------
void clPin::setValue(int i){
	switch (pt){
		case ptDigitalInput: // invalid
			break;
		case ptDigitalOutput:
		if (i <= 0)
			digitalWrite(pinNum,LOW);
		else
			digitalWrite(pinNum,HIGH);
			break;
		default:
			break;
	}
}
// ------------------------------------------------------------------
bool clPin::isName(char* name){
	if (strcmp(name,pinName) == 0)
	{
		return true;
	} else return false;
}

/* ------------------------------------------------------------------
-- Pin List code
------------------------------------------------------------------ */
	
class clPinList
{
private:
	clPin* pList;
	clPin* lastPin;
	unsigned short int numOfPins;
public:
	clPinList();
	~clPinList();
	void addPin(int num,char* name,PinType t);
	void addPin(int num,char* name,char* pt);
	void addPinSub(int num,char* name,PinType t);
	void setPinValue(int num,int value);
	void setPinValue(char* name,int value);
};



clPinList::clPinList(){
	pList = NULL;
	lastPin = NULL;
	numOfPins = 0;
}

clPinList::~clPinList(){
	// needs to destroy the pins in the list
}

void clPinList::addPin(int num,char* name,PinType t){
	clPin* Pin = new clPin(num,name,t);
	numOfPins++;
	if (pList == NULL)
	{
		pList = Pin;
		lastPin = Pin;
		// prev and next are still both NULL
	}
	else
	{
		lastPin->setNext(Pin);
		Pin->setPrev(lastPin);
		lastPin = Pin;
	}
}

void clPinList::addPin(int num,char* name,char* pt){
	clPin* Pin = new clPin(num,name,pt);
	numOfPins++;
	if (pList == NULL)
	{
		pList = Pin;
		lastPin = Pin;
		// prev and next are still both NULL
	}
	else
	{
		lastPin->setNext(Pin);
		Pin->setPrev(lastPin);
		lastPin = Pin;
	}
}



void clPinList::setPinValue(int num,int value){
	clPin* p = pList;
	while(p != NULL){
		if(p->getNum() == num) // found the pin
		{
			p->setValue(value);
			return;
		}
		p = p->getNext();
	}
}

void clPinList::setPinValue(char* name,int value){
	clPin* p = pList;
	while(p != NULL){
		if(p->isName(name)) // found the pin
		{
			p->setValue(value);
			return;
		}
		p = p->getNext();
	}
}

#endif