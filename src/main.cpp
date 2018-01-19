/***************************************************
file : mainclass of hisMonitor2.2 for dm
author: wanghao200911@163.com
date: 2017-08-18
version:2.2
update : refactor code;
	 add log function;
	 fix bug when connect db;
***************************************************/
#include "iostream"
#include "MyUtil.h"
#include "Parameter.h"
#include "log.h"
#include "Runner.h"
#include "Metric.h"
#include "MySendFactory.h"
using namespace std;

int main()
{
	string D5000_HOME = MyUtil::getD5000HomePath();	
	string conf = D5000_HOME + "/conf/auto_monitor.conf";
	string alarmConf = D5000_HOME + "/conf/AlarmInfo.conf";
	
	//single pattern or static class
	Parameter * para = new Parameter(conf,alarmConf);
	para -> print();
	MySendFactory mySend = MySendFactory(para);
	string logFile = para -> myDbLog;
	char logMode[32] = "NaN";
	Parameter::sleepTime = 1;
	Log::get_instance()->init((const char*)logFile.c_str(), 8192,20000000 , 0,logMode); // logBufferSize,logFileSize,cacheSize
	Runner* runner = new Runner(MyUtil::getTime(Parameter::sleepTime));
	//runner -> collectStaticInfo();
	//runner -> switchDbUseThread();
	LOG_INFO("%s : %s","monitor hisdb start",runner -> statTime.c_str());
	int count = 0;
	string dailyTime = "00:30:00";
	while(1)
	{
		count ++ ;
		runner -> statTime =  MyUtil::getTime(Parameter::sleepTime);
		cout << "----statTime :" << runner -> statTime << endl;
		if(count % 60 ) // task  do every 30 min
		{

		}
		else if (count % 240) // task do every 2 h
		{

		}
		else if (count % 2880)  //task do every day
		{
			count =0; //reset every day
		}

		runner -> collectHisDb();
		runner -> switchDb();


		if(MyUtil::compareDailyTimer(runner-> statTime,dailyTime) == 0); // run daily timer task
		{
			// collect tableSpace message;
		}

		Log::get_instance()->flush();
		sleep(1);
	}
	return 1;	
}
	
