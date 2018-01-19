/***************************************************
file : mainclass of hisMonitor2.4 for kingbase
author: wanghao200911@163.com
date: 2017-09-20
version:2.4
update : update some sql;
	 monitor trx cout;
	 monitor active session count;
	 monitor hisdb,ems space ;
***************************************************/
#include "iostream"
#include "MyUtil.h"
#include "Parameter.h"
#include "log.h"
#include "Runner.h"
#include "Metric.h"
#include "MySendFactory.h"
using namespace std;
#define VERSION  "exe2_his_kingbase_2.4"

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
	Log::get_instance()->init((const char*)logFile.c_str(), 8192,20000000 , 0,logMode); // logBufferSize,logFileSize,cacheSize
	Runner* runner = new Runner(MyUtil::getTime(Parameter::sleepTime));
	//runner -> collectStaticInfo();
	//runner -> switchDbUseThread();
	LOG_INFO("%s : %s %s","monitor hisdb start ",VERSION,runner -> statTime.c_str());
	while(1)
	{
		//Parameter::refresh();  // refresh parameter before run;
		runner -> statTime =  MyUtil::getTime(Parameter::sleepTime);
		runner -> collectHisDb();
		Log::get_instance()->flush();   //flush log from cache to file
		sleep(1);
	}
	return 1;	
}
	
