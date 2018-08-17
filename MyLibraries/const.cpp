#ifndef CONST_H
#define CONST_H

// the static keyword seems to give scope of these to e
#ifdef DEBUG
	static const bool debug = true;
#else
	static const bool debug = false;
#endif

enum FuncReturn
{
	// standard returns
	FR_OK,
	FR_NG,

	// adding char to msg
	FR_EndOfMsg,
	FR_BlankMsg,
	// last one
	FR_Unknown = 16000	
};

enum MsgType
{
	mtUnknown,
	mtAddPin
	
};

enum MsgStatus
{
	msUnknown,
	msCreated,
	msComplete,
	msProcessed,
	msInvalid = 100
};

enum PinType 
{
	ptUnknown,
	ptDigitalInput,
	ptDigitalOutput,
	ptPWM,
	ptAnalogueInput,
	ptAnalogueOutput
};

const char* pinTypeTxt [] =
{
	"ptUnknown",
	"ptDigitalInput",
	"ptDigitalOutput",
	"ptPWM",
	"ptAnalogueInput",
	"ptAnalogueOutput"
};


#endif