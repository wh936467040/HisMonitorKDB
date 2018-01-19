#include "iostream"
#include "log.h"
#include "MyUtil.h"
#include "MySendFactory.h"
#include "Thread.h"
#include "RunTableSpaceShell.h"
#include "MainDbIpGet.h"
#include "string.h"
#include "Runner.h"
#include "Metric.h"
#include "Parameter.h"
//#include "CollectLog.h"
#include "MetricDao.h"
using namespace std;

int Runner::count = 59;
string Runner::nodeName ="";
Runner::Runner(string time) 
{
	statTime = time;
//	nodeName = MainDbIpGet::getMainNode();
}

int Runner::collectSqlUseThread(){
	//creat a thread collectSql();
}

void * Runner::collectHisDb()
{
	string time = statTime;   //avoid statTime changed when use thread 
	int begin, end;
	begin = metric_db_mem;
	end = metric_db_long_session;
	MetricDao* metricDao = new MetricDao();
	vector<Metric*> metrics;
	int i;
//	this -> switchDb();
	for(i = begin; i <= end; i++)
	{
		//need to update with switch-case 
		if(i == metric_db_long_session)
		{
			count ++ ;
			if(count != 60)  // collect per half hour
			{
				continue;
			}
			else 
			{
				metricDao->read(metrics, i);
				metricDao->send(metrics,i,time);
				metricDao->clear(metrics,i);	
				count = 0;
			}
		}else if(i == metric_db_tablespace)
		{
			if(MyUtil::compareDailyTimer(time,"00:20:00") == 1) // run collect TableSpace shell when time is "00:20:00";
			{
				string shellCmd = "./../kdbDataMonitor/tablespaceCollect.sh";
				LOG_INFO("%s ","run collect tablespace shell...");
				runShell(shellCmd);
			}
			else if(MyUtil::compareDailyTimer(time, "03:50:00") == 1) // read and send data before 4:00 AM;
			{
				metricDao->read(metrics, i);
				metricDao->send(metrics,i,time);
				metricDao->clear(metrics,i);
				LOG_INFO("%s","send tablesapce data");
			}
		}else
		{
			metricDao->read(metrics, i);	
			metricDao->send(metrics,i,time);
			metricDao->clear(metrics,i);
		}
	}
	delete metricDao;
	metricDao=NULL;
	//return;
}

/*
void * Runner::collectSql()
{
	vector<Metric*> metrics;
	SqlCollector* dbSql =new SqlCollector();
	while(1)
	{
		dbSql->run(metrics);
		DbSqlMetric::sendMetric(metrics,statTime);
		MetricDao::clear(metrics,metric_db_sql);
		sleep(Parameter::sleepTime);
	}
	delete dbSql;
}
*/
void * Runner::collectStaticInfo ()
{
	vector<Metric*> metrics;
	MetricDao* metricDao = new MetricDao();
	metricDao -> read(metrics, metric_db_info);
	int type = metric_db_info;
	metricDao -> send(metrics,type,statTime);
	metricDao -> clear(metrics, metric_db_info);
	delete metricDao;
}


void * Runner::sendStartAlarm()
{
	struct ALARM_INFO_D5000 alarmInfo;
        alarmInfo.itemid="00020041";
        alarmInfo.data="数据库监视程序启动";
        int ret = MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, MyUtil::getTime(Parameter::sleepTime),alarmInfo);
        if(ret <= 0 )
        {
        	LOG_ERROR("%s; %s","send alarm failed",alarmInfo.data.c_str());
        }
        else
        {
			LOG_INFO("%s; %s","send alarm ok",alarmInfo.data.c_str());
        }
}


void * Runner :: switchDb()
{
	string newNode = MainDbIpGet::getMainNode();
	if(nodeName != newNode)
	{
		cout << " alarm: db switch " << nodeName << " --> " << newNode << endl;
		Runner::sendSwitchAlarm(nodeName,newNode);
		nodeName = newNode;
	}
	else
	{
		//node = newNode;
	}
}

void * Runner::sendSwitchAlarm(string oldNode,string newNode)
{
        struct ALARM_INFO_D5000 alarmInfo;
        alarmInfo.itemid = "00020042";
        alarmInfo.data = "数据库切机" ;
		alarmInfo.data = alarmInfo.data + oldNode + " --> " + newNode;
        int ret = MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId,  MyUtil::getTime(Parameter::sleepTime),alarmInfo);
        if(ret <= 0 )
        {
                LOG_ERROR("%s; %s","send alarm failed",alarmInfo.data.c_str());
        }
        else
        {
                LOG_INFO("%s; %s","send alarm ok",alarmInfo.data.c_str());
        }
}
