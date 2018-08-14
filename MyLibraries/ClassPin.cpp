// Pin Class

enum PinType
{
	ptDigitalInput,
	ptDigitalOutput,
	ptPWM,
	ptAnalogueInput,
	ptAnalogueOutput
};

// List for the pins
const int ClassPin_MaxLenPinName = 100;

class clPin
{
private:
	PinType pt;
	int pinValue;
	clPin* Prev;
	clPin* Next;
	unsigned short int pinNum;
	char pinName[ClassPin_MaxLenPinName + 1]; 
public:
	clPin(int num,char* name, PinType t);
	~clPin();
	clPin* getPrev();
	clPin* getNext();
	void setPrev(clPin* pin);
	void setNext(clPin* pin);
	int getValue();
	int getNum();
	void setValue(int i);
	bool isName(char* name);
};

// The pin
	
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
	void setPinValue(int num,int value);
	void setPinValue(char* name,int value);
};



clPinList::clPinList(){
	pList = NULL;
	lastPin = NULL;
	numOfPins = 0;
}

clPinList::~clPinList(){

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

clPin::clPin(int num,char* name,PinType t){
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
			break;
	}
	if (strlen(name) <= ClassPin_MaxLenPinName)
	{
		strcpy(pinName,name);
	}
	else pinName[0] = 0;

}

clPin::~clPin(){
	//pt = PT;
}

clPin* clPin::getNext(){
	return Next;
}

void clPin::setNext(clPin* p){
	Next = p;
}

void clPin::setPrev(clPin* p){
	Prev = p;
}

clPin* clPin::getPrev(){
	return Prev;
}

int clPin::getValue(){
	return pinValue;
}

int clPin::getNum(){
	return pinNum;
}

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

bool clPin::isName(char* name){
	if (strcmp(name,pinName) == 0)
	{
		return true;
	} else return false;
}