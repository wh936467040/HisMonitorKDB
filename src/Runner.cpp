#include "iostream"
#include "log.h"
#include "MyUtil.h"
#include "MySendFactory.h"
#include "Thread.h"
#include "MainDbIpGet.h"
#include "string.h"
#include "Runner.h"
#include "Metric.h"
#include "Parameter.h"
//#include "CollectLog.h"
#include "MetricDao.h"

using namespace std;
string Runner::node = MainDbIpGet::getMainNode();
Runner::Runner(string time) 
{
	statTime = time;
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
	i = metric_db_long_session;
	for(i = begin; i <= end; i++)
	{
		metricDao->read(metrics, i);	
		metricDao->send(metrics,i,time);
		metricDao->clear(metrics,i);
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

class SwitchDb: public Thread
{
public:
    void run()
    {
	Runner::switchDb();
    }
};


void * Runner :: switchDb()
{
        string newNode = MainDbIpGet::getMainNode();
        if(node != newNode)
        {
		cout << " alarm: db switch " << node << " --> " << newNode << endl;
		Runner::sendSwitchAlarm(node,newNode);
		node = newNode;
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

void * Runner::switchDbUseThread()
{
	SwitchDb th;
	th.start();
	cout << "thread run" << endl;
}
