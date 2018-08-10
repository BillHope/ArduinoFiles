/*
This uses a large sample of 100 intervals so
any errors should be smoothed
Bill Hope
9/9/2014
*/

class AverageCounter
{
	public:
	unsigned long a[101];
	int S;    // start 0 - 100
	int E;    // end 0 - 100
	char status; // Empty Building Ready
	
	AverageCounter();
	bool addTime(unsigned long t);	
//	bool addCount();
	unsigned long getElapsedMillis();
	unsigned long maxMillis = 3000;


};

AverageCounter::AverageCounter()
{
	S = 0;
	E = 0;
	status = 'E';
}
/*
bool AverageCounter::addCount()
{
	return addTime(millis());
}
*/

bool AverageCounter::addTime(unsigned long t)
{
	switch (status)
	{
	case 'E':
		a[E] = t;
		status = 'B';
		break;
	case 'B':
		a[++E] = t;
		if (E > 99) status = 'R'; // array is now full
		break;
	case 'R':
		if (E > 99) E = -1;
		a[++E] = t;
		S++;
		if (S > 100) S = 0;
		break;
	default: return false;
		break;
	}
}


unsigned long AverageCounter::getElapsedMillis(unsigned long Current)
{
	unsigned long iReturn = 0;
	if (status == 'R')
		if ((Current - a[E]) < maxMillis)
			iReturn = (a[E] - a[S]) / 100;
	
	return iReturn;
};
