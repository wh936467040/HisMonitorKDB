#ifndef METRIC_DAO_H
#define METRIC_DAO_H

#include <vector>
#include <map>

#include "dcidefine.h"
#include "dcisg.h"
#include "Metric.h"
using namespace std;
class MetricDao
{
private:
	CDci localDb;
	static int connectMark ;
	static int connectAlarmMark;
	ErrorInfo_t error1;
	char sql[SQL_LENGTH_MAX];
	string local_db_port;
	/*
	map<char*, char*> last_stattime_cpu_map;
	map<char*, char*> last_stattime_mem_map;
	map<char*, char*> last_stattime_fs_map;
	map<char*, char*> last_stattime_disk_map;
	map<char*, char*> last_stattime_net_map;	
	char sql_start_time[32];
	*/
public:
	MetricDao();
	~MetricDao();
	bool exist(const char* table_name, const char* where_text);
	void read(vector<Metric*>& metrics, int metricType);
	//void save(vector<Metric*>& metrics, int metricType, HisNode* hisDb, const char* stat_time);
	void send(vector<Metric*>& metrics,int& metricType,string& state_time);
	void clear(vector<Metric*>& metrics, int metricType);
};

#endif
