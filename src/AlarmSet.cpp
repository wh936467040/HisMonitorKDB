#include "AlarmSet.h"
#include "iostream"
#include "vector"
#include "string.h"
using namespace std;

Alarm :: Alarm(int t,string time ,string d)
{
	this -> startTime = time;
	this -> type = t;
	this -> data = d;
	this -> mark = 0;
}

Alarm :: Alarm(string time ,string d)
{
	this -> startTime = time;
	this -> type = 0;
	this -> data = d;
	this -> mark = 0;
}

Alarm:: Alarm(const Alarm& a)
{
	this -> startTime = a.startTime;
	this -> type = a.type;
	this -> data = a.data;
	//this -> mark = a.mark;
}

int Alarm :: setMark(int i)
{
	this -> mark = i;
}

Alarm::~Alarm()
{
}


vector<Alarm*> AlarmSet::alarmList;

int AlarmSet :: processAlarm(Alarm alarm)
{
	Alarm * res = findAlarmAndSetMark(alarm);
	if(res!=NULL)
	{
		//do nothing;
		res -> setMark(1);
		return 1;
	}
	else
	{
		insertAlarm(alarm);// setMark While insert
		sendAlarm(alarm);
	}
	return 0;
}

int AlarmSet :: processNotExistAlarm()
{
	for(vector<Alarm*>::iterator it = alarmList.begin(); it != alarmList.end(); ++it)
	{
		if((*it) -> mark ==0)
		{
			sendDisAlarm(*(*it));
			deleteAlarm(*(*it));
		}
	}
	for(vector<Alarm*>::iterator it = alarmList.begin(); it != alarmList.end(); ++it)
	{
		(*it) -> setMark(0);
	}	

}

Alarm* AlarmSet::findAlarmAndSetMark(const Alarm alarm)
{
	for(vector<Alarm*>::iterator it = alarmList.begin(); it != alarmList.end(); ++it)
	{
		if( alarm.type == (*it) -> type && alarm.startTime == (*it) -> startTime && alarm.data == (*it) -> data)
		{
			(*it) -> setMark(1);
			return *it;
		}
	}
	return NULL;
}


int AlarmSet::insertAlarm(Alarm alarm)
{
	 Alarm * res = findAlarmAndSetMark(alarm);
	 if(res==NULL)
  	 {
		Alarm * it = new Alarm(alarm);
		it -> setMark(1);
		alarmList.push_back(it);
	 }
	 return 1;
}


int AlarmSet::deleteAlarm(Alarm alarm)
{
 	for(vector<Alarm*>::iterator it = alarmList.begin(); it != alarmList.end(); ++it)
	{
 		if(alarm.type == (*it) -> type && alarm.startTime == (*it) -> startTime && alarm.data == (*it) -> data)
		{
			alarmList.erase(it);
			delete (*it);
			*it=NULL;
			return 1;
		}
	}														
        return -1;
}


int AlarmSet::sendAlarm(Alarm alarm)
{

	
}

int AlarmSet::sendDisAlarm(Alarm alarm)
{

}
