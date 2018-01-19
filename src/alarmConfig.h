#include "iostream"
using namespace std;
class AlarmConfig
{
	public:	
		string alarmMainIp;
		int alarmMainPort;
		string alarmIpBak;
		int alarmPortBak;
		
		int memUsedAlarm;
		int tableUsedAlarm;
		int sessionAlarm;

		int getConfInfo();
		int getAlarmConfInfo();
};
