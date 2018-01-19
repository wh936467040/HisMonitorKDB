#include "Util.h"
#include "vector"
#include "iostream"
#include "Metric.h"
#include "sendinfo.h"
#include "MySendFactory.h"
#include "MyUtil.h"
#include "Parameter.h"
#include "alarmConfig.h"
#include "alarmSet.h"
using namespace std;


string sessionFlag;
string sessionAlarmStartTime;
string memAlarmStartTime;
string memFlag;
string redoFlag;
string redoAlarmStartTime;
string trxFlag;
string trxAlarmStartTime;
string activeSessionFlag;
string activeSessionAlarmStartTime;
vector<AlarmTable *> alarmSet;

int sessionAlarm = 0;
int memAlarm = 0;
int redoAlarm = 0;
int trxAlarm =0;
int activeSessionAlarm =0;

int writeFile(string fileName,string data,string time);
Metric::Metric()
{
	//char buf[20] = {0};
	//m_statTime = string(Util::getTime(buf));
}

Metric::Metric(string dbid):m_dbid(dbid)
{
	//char buf[20] = {0};
	//m_statTime = string(Util::getTime(buf));
}
DbSqlMetric::DbSqlMetric(char* start_time, long exec_time, char* client_ip, char* db_name, char* login_name, char* app_name, char* sess_addr, long tran_id, char* sql_text)
{	
	strcpy(m_start_time, start_time);
	m_exec_time=exec_time;
	strcpy(m_client_ip, client_ip);
	strcpy(m_db_name, db_name);
	strcpy(m_login_name, login_name);
	strcpy(m_app_name, app_name);
	strcpy(m_sess_addr, sess_addr);
	m_tran_id= tran_id;
	strcpy(m_sql_text, sql_text);
}

DbBufMetric::DbBufMetric(string dbid, int buf_id, string buf_name, long buf_bytes, long logic_read_count, long phy_read_count, double rat_hit)
	:Metric(dbid), m_buf_id(buf_id), m_buf_name(buf_name), m_buf_bytes(buf_bytes), m_logic_read_count(logic_read_count), m_phy_read_count(phy_read_count), m_rat_hit(rat_hit)
{}

DbBufMetric::DbBufMetric(string dbid, long counter1, long counter2, long counter3, long counter4, long counter5, long counter6, long counter7)
	:Metric(dbid), m_counter1(counter1), m_counter2(counter2), m_counter3(counter3), m_counter4(counter4), m_counter5(counter5), m_counter6(counter6), m_counter7(counter7)
{}

DbMemMetric::DbMemMetric(string dbid, long mem_pool_total_bytes, long mem_pool_used_bytes, long os_mem_used_bytes)
	:Metric(dbid), m_mem_pool_total_bytes(mem_pool_total_bytes), m_mem_pool_used_bytes(mem_pool_used_bytes), m_os_mem_used_bytes(os_mem_used_bytes)
{}
DbMemMetric::DbMemMetric(string dbid, long counter1, long counter2, long counter3, long counter4)
	:Metric(dbid), m_counter1(counter1), m_counter2(counter2), m_counter3(counter3), m_counter4(counter4)
{}


DbMetric::DbMetric(string dbid,float cpuUsed, long session_count,long trxCount,long active_session_count,long redo_count,string start_time)
	:Metric(dbid),m_cpu_used(cpuUsed),m_session_count(session_count),m_trx_count(trxCount),m_active_session_count(active_session_count),m_redo_count(redo_count),m_start_time(start_time)
{}


DbTablespaceMetric::DbTablespaceMetric(string datname, string spcname, long availableSize)
	:Metric("0"),m_datname(datname), m_spcname(spcname), m_available_size(availableSize)
{}

DbInfoMetric::DbInfoMetric(string dbid,long session_max_count,string version,string port)
	:Metric(dbid),m_session_max_count(session_max_count),m_version(version),m_port(port)
{}
DbSessionMetric::DbSessionMetric(string dbid,string schame,string user,string clientIp,long count)
	:Metric(dbid),m_schame(schame),m_user(user),m_clientIp(clientIp),m_count(count)
{}

DbTrxMetric::DbTrxMetric(string dbname,long cx,long ax)
	:databaseName(dbname),currentXid(cx),ageXid(ax)
{}


/*
DbRlogFileMetric::DbRlogFileMetric(string dbid, int file_num, string current_file, int total_bytes, int free_bytes, double use_ratio)
	:Metric(dbid), m_file_num(file_num), m_current_file(current_file), m_total_bytes(total_bytes), m_free_bytes(free_bytes), m_use_ratio(use_ratio)
{}
*/

DbIoMetric::DbIoMetric(string dbid, long read_count, long write_count, long read_bytes, long write_bytes)
	:Metric(dbid), m_read_count(read_count), m_write_count(write_count), m_read_bytes(read_bytes), m_write_bytes(write_bytes)
{}


DbLongSessionMetric::DbLongSessionMetric(string db,string user,string ip,string start,long c,string sql)
{
	dbName = db;
	userName = user;
	host = ip;
	queryStart = start;
	cost = c;
	querySql = sql;
}

/*
DbSessionMetric::DbSessionMetric(string dbid, char* sess_addr, char* create_time, char* login_name, char* client_app, char* client_ip, char* status, char* sql_text, long cpu_time, long cpu_time_call, long logic_reads, long logic_reads_call, long mem_used)
	:Metric(dbid), m_cpu_time(cpu_time), m_cpu_time_call(cpu_time_call), m_logic_reads(logic_reads), m_logic_reads_call(logic_reads_call), m_mem_used(mem_used)
{
	strcpy(m_sess_addr, sess_addr);
	strcpy(m_create_time, create_time);
	strcpy(m_login_name, login_name);
	strcpy(m_client_app, client_app);
	strcpy(m_client_ip, client_ip);
	strcpy(m_status, status);
	strcpy(m_sql_text, sql_text);
}
*/
void  DbIoMetric::sendMetric(vector<Metric*>& metrics,string &statTime)
{
	if(metrics.empty())
	{
		return;
	}
	
	string read_count="";
	string write_count="";
	string read_bytes="";
	string write_bytes="";

	char t_read_count[128];
	char t_write_count[128];
	char t_read_bytes[128];
	char t_write_bytes[128];
	memset(t_read_count,0,sizeof(t_read_count));
	memset(t_write_count,0,sizeof(t_write_count));
	memset(t_read_bytes,0,sizeof(t_read_bytes));
	memset(t_write_bytes,0,sizeof(t_write_bytes));


	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbIoMetric* io = static_cast<DbIoMetric*>(metric);
		sprintf(t_read_count,"%ld\0",io->m_read_count);
		sprintf(t_write_count,"%ld\0",io->m_write_count);
		sprintf(t_read_bytes,"%ld\0",io->m_read_bytes);
		sprintf(t_write_bytes,"%ld\0",io->m_write_bytes);	
		read_count=read_count+";"+t_read_count;
		write_count=write_count+";"+t_write_count;
		read_bytes=read_bytes+";"+t_read_bytes;
		write_bytes=write_bytes+";"+t_write_bytes;
	}
	read_count=read_count.substr(1,read_count.size()-1);
	write_count=write_count.substr(1,write_count.size()-1);
	read_bytes=read_bytes.substr(1,read_bytes.size()-1);
	write_bytes=write_bytes.substr(1,write_bytes.size()-1);

	struct DB_IO_STAT ioStat;
	ioStat.read_count=read_count; //脗露脙聛脗麓脙沤脙艩脙陆
	ioStat.write_count=write_count; //脙聬脗麓脗麓脙沤脙艩脙陆
	ioStat.read_bytes=read_bytes; //脗露脙聛脗麓脙鲁脙聬脗隆
	ioStat.write_bytes=write_bytes; //脙聬脗麓脗麓脙鲁脙聬脗隆
	/*
	cout<<"----------DB_IO--------------"<<endl;
	cout<<read_count<<endl;
	cout<<write_count<<endl;
	cout<<read_bytes<<endl;
	cout<<write_bytes<<endl;
	cout<<"-----------DB_IO--------------"<<endl;
	*/
//	writeFile("DBIOStat",read_count+" "+write_count+" "+read_bytes+" "+write_bytes+" ",statTime);
	MySendFactory::sendInfo->sendDBIoStatInfo(Parameter::nodeId,statTime,ioStat);
}


void DbTablespaceMetric::sendMetric(vector<Metric*>& metrics,string &statTime)
{
	if(metrics.empty())
	{
		return;
	}
	string vecDatname = "datname:";
 	string vecSpcname = "spcname:";
	string vecAvlSize = "avlsize:";
	vector<string> vecSend;
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbTablespaceMetric* table= static_cast<DbTablespaceMetric*>(metric);
		vecDatname = vecDatname + table -> m_datname + ";";
		vecSpcname = vecSpcname + table -> m_spcname + ";";
		vecAvlSize = vecAvlSize + MyUtil::ltos(table -> m_available_size) + ";";
		if(table -> m_available_size < 40 * 1024)
		{
			struct ALARM_INFO_D5000 alarmInfo;
			alarmInfo.itemid="00020043";
			alarmInfo.data="";
			alarmInfo.data=alarmInfo.data+"HisDB数据库表空间剩余" + table -> m_datname + " " + table -> m_spcname + " " + MyUtil::ltos(table -> m_available_size) + " MB";
			MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
		}
	}
	vecDatname = vecDatname.substr(0,vecDatname.length()-1);
	vecSpcname = vecSpcname.substr(0,vecSpcname.length()-1);
	vecAvlSize = vecAvlSize.substr(0,vecAvlSize.length()-1);
	vecSend.push_back(vecDatname);
	vecSend.push_back(vecSpcname);
	vecSend.push_back(vecAvlSize);
	//cout << "vecDatname---" << vecDatname << endl;
	//cout << "vecSpcname---" << vecSpcname << endl;
	//cout << "vecAvlSize---" << vecAvlSize << endl; 
	MySendFactory::sendInfo->sendAllInfo("40016",Parameter::nodeId,statTime,vecSend);
}


void DbLongSessionMetric::sendMetric(vector<Metric*>& metrics,string&statTime)
{
	if(metrics.empty())
	{
		return;
	}
	vector<string> vecSend;;
	string dbNameStr="schameValue:";
	string usernameStr="usernameValue:";
	string clientIpStr="clientIpValue:";
	string queryStart = "queryStart:";
	string costTimeStr ="countValue:";
	string sqlStr  = "querySql:";
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbLongSessionMetric * db = static_cast<DbLongSessionMetric *>(metric);
		
		dbNameStr = dbNameStr + db -> dbName + ";";
		usernameStr = usernameStr + db-> userName +";";
		clientIpStr = clientIpStr + db-> host +";";
		queryStart = queryStart + db ->queryStart + ";";
		costTimeStr = costTimeStr + MyUtil::second2String(db -> cost) + ";";

		string sql = db ->querySql;
		sql = MyUtil::replaceString(sql,"'"," ");
		sql = MyUtil::replaceString(sql,";"," ");
		sqlStr = sqlStr + sql + ";";	
	}
	dbNameStr=dbNameStr.substr(0,dbNameStr.length()-1);
	usernameStr=usernameStr.substr(0,usernameStr.length()-1);
	clientIpStr=clientIpStr.substr(0,clientIpStr.length()-1);
	queryStart=queryStart.substr(0,queryStart.length()-1);
	costTimeStr=costTimeStr.substr(0,costTimeStr.length()-1);
	sqlStr=sqlStr.substr(0,sqlStr.length()-1);
	
	vecSend.push_back(dbNameStr);
	vecSend.push_back(usernameStr);
	vecSend.push_back(clientIpStr);
	vecSend.push_back(queryStart);
	vecSend.push_back(costTimeStr);
	vecSend.push_back(sqlStr);
	MySendFactory::sendInfo->sendAllInfo("10056",Parameter::nodeId,statTime,vecSend);

}


void DbMemMetric ::sendMetric(vector<Metric*>& metrics,string &statTime)
{
	if(metrics.empty())
	{
		return;
	}
	string counter1;
	string counter2;
	string counter3;
	string counter4;
	string nullString="";

	char t_counter1[128]="";
	char t_counter2[128]="";
	char t_counter3[128]="";
	char t_counter4[128]="";

	memset(t_counter1,0,sizeof(t_counter1));
	memset(t_counter2,0,sizeof(t_counter2));
	memset(t_counter3,0,sizeof(t_counter3));
	memset(t_counter4,0,sizeof(t_counter4));

	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		int useRatio=100;
		Metric* metric = *it;
		DbMemMetric * mem= static_cast<DbMemMetric *>(metric);
		if(mem->m_counter1>1)
		{
			useRatio=100*mem->m_counter2/mem->m_counter1;
		}
		//cout<<"memuseRatio"<<mem->m_counter2<<"/"<<mem->m_counter1<<" result "<<useRatio<<endl;
		if(useRatio > Parameter::memPoolThreshold)
		{
			if(memAlarm==0)
			{
				struct ALARM_INFO_D5000 alarmInfo;
				alarmInfo.itemid="00020009";
				alarmInfo.data="";
				alarmInfo.data=alarmInfo.data+"数据库缓存池" + " 使用率过高,当前值 " + MyUtil::itos(useRatio)+"%";
				//alarmInfo.level="4";
				MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
				memAlarm=1;
				memFlag=alarmInfo.data;
				memAlarmStartTime=statTime;
				cout<<"数据库缓存池告警"<<endl;
			}
		}
		else
		{
			if(memAlarm==1)
			{
				MySendFactory::sendAlarm -> sendD5000DisAlarmInfo(Parameter::nodeId,"00020009",memAlarmStartTime,statTime,memFlag);
				cout<<"发送取消告警 mem "<<  Parameter::memPoolThreshold <<" 当前值 "<<useRatio<<endl;
				memAlarm=0;
			}
		}

		sprintf(t_counter1,"%ld\0",mem->m_counter1);
		sprintf(t_counter2,"%ld\0",mem->m_counter2);
		sprintf(t_counter3,"%ld\0",mem->m_counter3);
		sprintf(t_counter4,"%ld\0",mem->m_counter4);
		counter1=counter1+";"+t_counter1;
		counter2=counter2+";"+t_counter2;
		counter3=counter3+";"+t_counter3;
		counter4=counter4+";"+t_counter4;
		nullString =nullString+";"+"0";
	}

	counter1=counter1.substr(1,counter1.size()-1);
	counter2=counter2.substr(1,counter2.size()-1);
	counter3=counter3.substr(1,counter3.size()-1);
	counter4=counter4.substr(1,counter4.size()-1);
	nullString=nullString.substr(1,nullString.size()-1);
	
	struct DB_MEM_STAT dbMem;
	dbMem.mem_total_bytes=counter1;
	dbMem.mem_used_bytes=counter2;
	dbMem.os_used_bytes=nullString;
	dbMem.buf_bytes=counter3;
	dbMem.log_buf_bytes=counter4;
	/*
	cout<<"--------------DB_MEM---------------------"<<endl;
		cout<<counter1<<endl;
		cout<<counter2<<endl;
		cout<<counter3<<endl;
		cout<<counter4<<endl;
	cout<<"--------------DB_MEM---------------------"<<endl;
	*/
	MySendFactory::sendInfo->sendDBMemStatInfo(Parameter::nodeId,statTime,dbMem);
//	writeFile("DBMemStat.txt",dbMem.mem_total_bytes+" "+dbMem.mem_used_bytes+" "+dbMem.os_used_bytes+" "+dbMem.buf_bytes+" ",statTime);
}



void DbMetric:: sendMetric(vector<Metric*>& metrics,string&statTime)
{
	if(metrics.empty())
	{
		return;
	}
	vector<string> vecSend;
	string sessionCount="sessionCount:";
	string cpuUsed="cpuUsed:";
	string trxCount="trxCount:";
	string activeSessionCount = "activeSessionCount:";
	string redoCount = "redoCount:";
	string startTime = "startTime:" ;
	
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
   	{
		Metric* metric = *it;
		DbMetric * db= static_cast<DbMetric *>(metric);
		sessionCount = sessionCount  + MyUtil::ltos(db -> m_session_count)+ ";";
		cpuUsed = cpuUsed  + MyUtil::f2tos(db -> m_cpu_used) + ";";
		trxCount = trxCount + MyUtil::ltos(db -> m_trx_count) + ";";
		activeSessionCount = activeSessionCount + MyUtil::ltos(db -> m_active_session_count) + ";";
		redoCount = redoCount + MyUtil::ltos(db -> m_redo_count) + ";";
		startTime = startTime + db -> m_start_time + ";";
		if(db->m_session_count> Parameter::sessionThreshold)
		{
			if(sessionAlarm==1)
			{
				continue;
			}
			struct ALARM_INFO_D5000 alarmInfo;
			alarmInfo.itemid="00020008";
			alarmInfo.data="";
			alarmInfo.data=alarmInfo.data+"数据库会话数" + "过高,当前值" + MyUtil::ltos(db -> m_session_count);
			//alarmInfo.level="4";
			MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
			sessionAlarm=1;
			sessionFlag=alarmInfo.data;
			sessionAlarmStartTime=statTime;
			cout<<"数据库连接告警"<<endl;
		}
		else
		{
			if(sessionAlarm==1)
			{
				MySendFactory::sendAlarm -> sendD5000DisAlarmInfo(Parameter::nodeId,"00020008",sessionAlarmStartTime,statTime,sessionFlag);
				cout<<"发送取消告警 seesioncount "<< Parameter::sessionThreshold  <<" 当前值 "<<db->m_session_count<<endl;
				sessionAlarm=0;
			}
		}

		if(db->m_redo_count < Parameter::redoThreshold)  //default threshold 5
		{
			if(redoAlarm == 1) continue;
			else 
			{
				struct ALARM_INFO_D5000 alarmInfo;
				alarmInfo.itemid="00020040";
				alarmInfo.data="";							
				alarmInfo.data=alarmInfo.data+"数据库redo次数" + "过低,当前值" + MyUtil::ltos(db -> m_redo_count); 
				MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
				redoAlarm=1;
				redoFlag=alarmInfo.data;
				redoAlarmStartTime=statTime;
				cout<<"数据库redo告警"<<endl;
			}
		}
		else
		{
			if(redoAlarm  == 1)
			{
				MySendFactory::sendAlarm -> sendD5000DisAlarmInfo(Parameter::nodeId,"00020040",redoAlarmStartTime,statTime,redoFlag);
				cout<<"发送取消redo告警 当前值 "<<db->m_redo_count<<endl;
				redoAlarm=0;
			}
		}

		if(db->m_trx_count/10000.0> Parameter::trxThreshold)  //default threshold 5ww
		{
			if(trxAlarm == 1) continue;
			else													                 
			{
				struct ALARM_INFO_D5000 alarmInfo;
				alarmInfo.itemid="00020041";
				alarmInfo.data="";
				alarmInfo.data=alarmInfo.data+"数据库事务数" + "过高,当前值" + MyUtil::ltos(db -> m_trx_count);
				MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
				trxAlarm=1;
				trxFlag=alarmInfo.data;
				trxAlarmStartTime=statTime;
				cout<<"数据库事务数告警"<<endl;
			}
		}
		else
		{
			if(trxAlarm  == 1)
			{
				MySendFactory::sendAlarm -> sendD5000DisAlarmInfo(Parameter::nodeId,"00020041",trxAlarmStartTime,statTime,trxFlag);
				cout<<"发送取消事务告警 当前值 "<<db->m_trx_count<<endl;
				trxAlarm=0;
			}
		}
		if(db->m_active_session_count > Parameter::activeSessionThreshold)
		{
			if(activeSessionAlarm == 1) continue;
			else
			{
				struct ALARM_INFO_D5000 alarmInfo;
				alarmInfo.itemid="00020042";
				alarmInfo.data="";
				alarmInfo.data=alarmInfo.data+"数据库活动会话数" + "过高,当前值" + MyUtil::ltos(db -> m_active_session_count);
				MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId, statTime,alarmInfo);
				activeSessionAlarm=1;
				activeSessionFlag=alarmInfo.data;
				activeSessionAlarmStartTime=statTime;
				cout<<"数据库活动连接数告警"<<endl;
			}
		}
		else
		{
			if(activeSessionAlarm  == 1)
			{
				MySendFactory::sendAlarm -> sendD5000DisAlarmInfo(Parameter::nodeId,"00020042",activeSessionAlarmStartTime,statTime,activeSessionFlag);
				cout<<"发送取消数据库活动连接告警当前值 "<<db->m_active_session_count<<endl;
				activeSessionAlarm=0;
			}
		}
	}	
	sessionCount=sessionCount.substr(0,sessionCount.size()-1);
	cpuUsed=cpuUsed.substr(0,cpuUsed.size()-1);
	activeSessionCount = activeSessionCount.substr(0,activeSessionCount.size()-1);
	redoCount = redoCount.substr(0,redoCount.size()-1);
	//cout <<"vec" <<redoCount << endl;
	//cout << "vec" << activeSessionCount << endl;
	trxCount = trxCount.substr(0,trxCount.size()-1);
	startTime=startTime.substr(0,startTime.size()-1);
	vecSend.push_back(sessionCount);
	vecSend.push_back(cpuUsed);
	vecSend.push_back(trxCount);
	vecSend.push_back(activeSessionCount);
	vecSend.push_back(redoCount);
	vecSend.push_back(startTime);
	MySendFactory::sendInfo->sendAllInfo("10015",Parameter::nodeId,statTime,vecSend);	
}


void DbSqlMetric::sendMetric(vector<Metric*>& metrics,string&statTime)
{	
	if(metrics.empty())
	{
		return;
	}
	

	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{	
		struct DB_SQL_STAT db;
		Metric* metric = *it;
		DbSqlMetric * dbSql= static_cast<DbSqlMetric *>(metric);

		db.start_time=dbSql->m_start_time;
		char t_exec_time[128]="";
		memset(t_exec_time,0,sizeof(t_exec_time));

		sprintf(t_exec_time,"%ld\0",dbSql->m_exec_time);
		db.exec_time=t_exec_time;
		db.client_ip=dbSql->m_client_ip;
		db.db_name=dbSql->m_db_name;
		db.login_name=dbSql->m_login_name;
		db.app_name=dbSql->m_app_name;
		db.sess_addr=dbSql->m_sess_addr;
		char t_tran_id[128]="";
		memset(t_tran_id,0,sizeof(t_tran_id));

		sprintf(t_tran_id,"%ld\0",dbSql->m_tran_id);
		db.tran_id=t_tran_id;
		db.sql_text=dbSql->m_sql_text; 
		/*		
		cout<<"---------DB_sql--------------------"<<endl;
		cout<<"m_start_time:    "+db.start_time<<endl;
		cout<<"m_start_time:    "+db.exec_time<<endl;
		cout<<db.client_ip<<endl;
		cout<<db.login_name<<endl;
		cout<<db.sess_addr<<endl;
		cout<<db.tran_id<<endl;
		cout<<db.sql_text<<endl;
		cout<<"---------DB_sql--------------------"<<endl;
		*/
	//	MySendFactory::sendInfo->sendDBSqlStat(Parameter::nodeId, statTime,db);
	}
		return; 

}

void DbInfoMetric::sendMetric(vector<Metric*>& metrics,string&statTime)
{	
	if(metrics.empty())
	{
		return;
	}
	struct DB_STATIC_STAT dbStatic;
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbInfoMetric * db= static_cast<DbInfoMetric *>(metric);
		if(db->m_session_max_count < 0 || db->m_session_max_count > 2048 )
		{
			return;
		}
		dbStatic.version=db->m_version;
		char tmp[1024];
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"%ld",db->m_session_max_count);
		dbStatic.max_session_count=tmp;
		dbStatic.port=db->m_port;
	}	
	MySendFactory::sendInfo->sendDBStaticStat(Parameter::nodeId,statTime,dbStatic);
	return; 
}
void DbSessionMetric::sendMetric(vector<Metric*>& metrics,string&statTime)
{
	if(metrics.empty())
	{
		return;
	}
	vector<string> vecSend;;
	string schameStr="schameValue:";
	string usernameStr="usernameValue:";
	string clientIpStr="clientIpValue:";
	string countStr ="countValue:";
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbSessionMetric * db= static_cast<DbSessionMetric *>(metric);
		schameStr=schameStr + db->m_schame + ";";
		usernameStr=usernameStr + db->m_user +";";
		clientIpStr=clientIpStr + db->m_clientIp +";";
		countStr=countStr + MyUtil::itos(db->m_count) +";";
	}
	schameStr=schameStr.substr(0,schameStr.length()-1);
	usernameStr=usernameStr.substr(0,usernameStr.length()-1);
	clientIpStr=clientIpStr.substr(0,clientIpStr.length()-1);
	countStr=countStr.substr(0,countStr.length()-1);
	
	vecSend.push_back(schameStr);
	vecSend.push_back(usernameStr);
	vecSend.push_back(countStr);
	vecSend.push_back(clientIpStr);
	/*
	cout << schameStr << endl;
	cout << usernameStr << endl;
	cout << countStr << endl;
	cout << clientIpStr << endl;
	*/
	MySendFactory::sendInfo->sendAllInfo("10036",Parameter::nodeId,statTime,vecSend);
}

void DbTrxMetric::sendMetric(vector<Metric*>& metrics,string&statTime)
{
	if(metrics.empty())
	{
		return;
	}
   	vector<string> vecSend;
	string dbName = "dbName";
	string currentXid = "currentXid:";
	string ageXid = "ageXid:";
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
	{
		Metric* metric = *it;
		DbTrxMetric * db = static_cast<DbTrxMetric *>(metric);
		dbName=dbName + db -> databaseName + ";";
		currentXid = currentXid  + MyUtil::itos(db->currentXid) +";";
		ageXid= ageXid + MyUtil::itos(db -> ageXid) +";";
	}
	dbName=dbName.substr(0,dbName.length()-2);
	currentXid=currentXid.substr(0,currentXid.length()-2);									
	ageXid=ageXid.substr(0,ageXid.length()-2);

	vecSend.push_back(dbName);
	vecSend.push_back(currentXid);
	vecSend.push_back(ageXid);
	MySendFactory::sendInfo->sendAllInfo("10057",Parameter::nodeId,statTime,vecSend);
}


int writeFile(string fileName,string data,string time)
{
	FILE * fp=NULL;
	char path[128] = "";
	sprintf(path, "%s/data/auto_monitor/%s",getenv("HOME"), fileName.c_str());
	fp=fopen(path,"a");
	if(fp==NULL)
	{
		cout<<"open File failed"<<endl;
		return 0;
	}
	char space[16]="       ";
	char Enter[2]="";
	Enter[0]='\n';
	Enter[1]='\0';
	 
	fputs(data.c_str(),fp);
	fputs(space,fp);
	fputs(time.c_str(),fp);
	fputs(Enter,fp);
	fclose(fp);
	return 1;
}
