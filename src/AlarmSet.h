#ifndef __ALARMSQL_H__
#define __ALARMSQL_H__
#include "iostream"
#include "string.h"
#include "vector"
using namespace std;
class Alarm
{
public:
	int type;
	string startTime;
	string data;
	int mark;
	Alarm (string time ,string d);
	Alarm (int t,string time,string d);
        Alarm (const Alarm& a);
	int setMark(int i);
	~Alarm();
};

class AlarmSet
{
public:
	static vector<Alarm*> alarmList;
	static int processAlarm(Alarm alarm);
	static int processNotExistAlarm();
private:
	static Alarm* findAlarmAndSetMark(Alarm alarm);
	static int insertAlarm(Alarm alarm);
	static int deleteAlarm(Alarm alarm);
	static int sendAlarm(Alarm alarm);
	static int sendDisAlarm(Alarm alarm);
};

#endif
