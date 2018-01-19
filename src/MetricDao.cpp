#include <iostream>
#include "Parameter.h"
#include "MetricDao.h"
#include "dcidefine.h"
#include "MyUtil.h"
#include "MySendFactory.h"
#include "MainDbIpGet.h"
#include "Metric.h"
#include "log.h"
using namespace std;
string getDbStartTime();
int getProcNum();
float getCpuUsed(int pid);
long getReadFlow(int procNum);
long getWriteFlow(int procNum);
long getMemUsed(int procNum);
string getDbStartTime();

struct ALARM_INFO_D5000 alarmInfo;
int MetricDao::connectMark = 0;
int MetricDao::connectAlarmMark = -1;
string connectAlarmStartTime = "";
string connectAlarmDetail = "数据库服务停止";
string dbHost = "";
MetricDao::MetricDao()
{


	alarmInfo.itemid="00020041";
	string ip = Parameter::dbIp;
	string user = Parameter::dbUser;
	string pwd = Parameter::dbPwd;
	if(localDb.Connect(ip.c_str(), user.c_str(), pwd.c_str(), false, &error1))
	{
		connectMark = 1;
		cout <<"db connect success" << endl;
		dbHost = MainDbIpGet::getLocalNode();
		if(connectAlarmMark >0)
		{
			string statTime = MyUtil::getTime(Parameter::sleepTime);
			MySendFactory::sendAlarm ->sendD5000DisAlarmInfo(Parameter::nodeId,"00020041",connectAlarmStartTime,statTime,connectAlarmDetail);
			connectAlarmMark = -1;
           		connectAlarmDetail = "数据库服务停止"; //告警信息复位
		}
	}
	else
	{
		LOG_ERROR("%s: %s","db connect failed!",error1.error_info);
		connectMark = -1;
		localDb.DisConnect(&error1);
		cout<<"db connect error: " << error1.error_info << endl;
	}

	if(connectMark < 0 && connectAlarmMark < 0)
	{
		cout << "==========*************************" << endl;
		if("get dmserver start time failed" == getDbStartTime())		
		{
			connectAlarmMark = 1;
			connectAlarmDetail = dbHost + " " + connectAlarmDetail;
        		alarmInfo.data = connectAlarmDetail ;
			connectAlarmStartTime =  MyUtil::getTime(Parameter::sleepTime); 
			int ret = MySendFactory::sendAlarm -> sendD5000AlarmInfo(Parameter::nodeId,connectAlarmStartTime,alarmInfo);
			if(ret <= 0 )
        		{
                		LOG_ERROR("%s; %s","send alarm failed",alarmInfo.data.c_str());
       			}
        		else
       			{
                		LOG_INFO("%s; %s","send alarm ok",alarmInfo.data.c_str());
		        }	
		}
		
	}
}

MetricDao::~MetricDao()
{
	localDb.DisConnect(&error1);
}


void MetricDao::read(vector<Metric*>& metrics, int metricType)
{	
	CDci *dci=&localDb;
	if(dci==NULL ||connectMark != 1 )
	{
		cout<<"connect db error ";
		return;
	}

	int ret_code;
	int procNum=getProcNum();  //get pid
	switch(metricType)
	{
		case metric_db_mem:
		{
			//get block
			const char* sql = "select setting from SAMPLES.SYS_CATALOG.SYS_SETTINGS where Name='shared_buffers'";
			
			char total_mem[256];
			memset(total_mem,0,sizeof(total_mem));
			int rec_num, attr_num;
			char *data_buf; 
			struct ColAttr* attrs;
			ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code<=0)
			{
				cout<<error1.error_info<<endl;
				if(attrs != NULL)
				{
					free(attrs);
				}
				break;
			}
			char* val = (char*)(data_buf);
			memcpy(total_mem, val, strlen(val));
			//	cout<<"atoi mem"<< atoi(total_mem)<<endl;
			long tmp_mem=atoi(total_mem);
			long total_mem_kb=tmp_mem*32*1024;
			//	cout<<"shared_buffers   :"<<total_mem_kb<<endl;
			long used_mem=0;
			used_mem=getMemUsed(procNum);
			//	cout<<"used_mem         :"<<used_mem<<endl;
			//	cout<<"*******************"<<endl;
			dci->FreeReadData(attrs, attr_num, data_buf);
			DbMemMetric* memMetric = new DbMemMetric("0", total_mem_kb, used_mem, 0, 0);
			metrics.push_back(memMetric);
			break;
		}
		case metric_db_info:
		{
		  	const char* sql = "select setting from SAMPLES.SYS_CATALOG.SYS_SETTINGS where Name='max_connections'";	   
		    long sess_max=0;	        
		    int rec_num, attr_num;
		    char *data_buf;
		    struct ColAttr* attrs;
		    ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code <=0)
			{
				cout<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break;
			}
			char* val = (char*)(data_buf);
			sess_max=atoi(val);
			dci->FreeReadData(attrs, attr_num, data_buf);

			int rec_num2, attr_num2;
			char *data_buf2;
			struct ColAttr* attrs2;
			const char* sql2= "select version()";
			string version="";
				
			ret_code = dci->ReadData(sql2, &rec_num2, &attr_num2, &attrs2, &data_buf2, &error1);
			char* val2 = (char*)(data_buf2);
			version=val2;
			//	cout<<"version:  "<<version<<endl;
			//	cout<<"**********************"<<endl;
	
			dci->FreeReadData(attrs2, attr_num2,data_buf2);
			string port= Parameter::dbPort;	
			DbInfoMetric* staticMetric = new DbInfoMetric("0", sess_max,version,port);
			metrics.push_back(staticMetric);
			break;
		}
		case metric_db:
		{
			//const char * sql="select * from SAMPLES.SYS_CATALOG.V$SESSION";
			const char * sql="select count(*) from sys_stat_activity";
			string start_time=getDbStartTime();
			float cpuUsed = getCpuUsed(procNum);
			long trxCount = 0;
			long sess_count=0;
			int rec_num, attr_num;
			char *data_buf;
			struct ColAttr* attrs;
			ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code<=0)
			{
				cout<<"ret_code V$SESSION  :"<<ret_code<<endl;
				cout<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break;
			}
			char* val = (char*)(data_buf);
			memcpy(&sess_count, val, attrs[0].data_size);
		//	cout<<"session_count"<<sess_count<<endl;
			//cout<<"**************************"<<endl;
			string startTime=getDbStartTime();
			//cout<<startTime<<endl;
			dci->FreeReadData(attrs, attr_num, data_buf);

			const char * sql2 = "select max(age(datfrozenxid)) as age_xid from sys_database where datname not in ('GLOBAL','TEMPDB');";
			ret_code = dci->ReadData(sql2, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code<=0)
			{
				cout<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break;													
		       	}
			char * val2 = (char*)(data_buf);
			memcpy(&trxCount, val2, attrs[0].data_size);
			dci->FreeReadData(attrs, attr_num, data_buf);
			/*
			cout << "---------------------------------------" << endl;
			cout << "trxCount:" <<trxCount << endl;
			cout << "---------------------------------------" << endl;
			*/
			long redoCount;
			const char * sql3 = "select count(state) from sys_redologs where state = 'NOACTIVE';";
			ret_code = dci->ReadData(sql3, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code<=0)
			{
				cout<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break;
			}
			char * val3 = (char*)(data_buf);
			memcpy(&redoCount,val3,attrs[0].data_size);
			dci->FreeReadData(attrs, attr_num, data_buf);
			/*
			cout << "--------------------------------------" << endl;
			cout << "redoCount :" << redoCount << endl;
			cout << "--------------------------------------" << endl;
			*/
			long activeSessionCount = 0;
			const char * sql4 = "select count(*) from sys_stat_activity where current_query != '<IDLE>'";
			ret_code = dci->ReadData(sql4, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			if(ret_code<=0)
			{
				cout<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break;
			}
			char * val4 = (char*)(data_buf);
			memcpy(&activeSessionCount,val4,attrs[0].data_size);
			dci->FreeReadData(attrs, attr_num, data_buf);
			/*
			cout << "---------------------------------------- " << endl;
			cout << "activeSessionCount:" << activeSessionCount << endl;
			cout << "------------------------------------- -- " << endl;		
			*/
		
			DbMetric * dbMetric=new DbMetric("",cpuUsed, sess_count,trxCount,activeSessionCount,redoCount,startTime);
			metrics.push_back(dbMetric);
			break;
		}
		case metric_db_tablespace:
		{
			string shellResult = "./result.txt";
			FILE * fp= fopen(shellResult.c_str(),"r");
			if(NULL == fp)
			{
				LOG_ERROR("%s %s %s"," open ",shellResult.c_str(), " failed");
				break;
			}
			char buf[1024];
			string str = "";
			while(fgets(buf, 128, fp) != NULL)
	                {
				if(strlen(buf) < 2) continue;
				string str = string(buf);
				int index = str.find_first_of(" ", 0);
				string datname = str.substr(0,index);
				str = str.substr(index+1, str.length());
				index = str.find_first_of(" ", 0);
				string spcname = str.substr(0,index);
				str = str.substr(index+1, str.length()-1);
				long availableSize = MyUtil::stoi(str.c_str());
				//cout << datname << "-------" << spcname << "-----" << availableSize << endl;
				DbTablespaceMetric* tpMetric = new DbTablespaceMetric(datname,spcname,availableSize);
				metrics.push_back(tpMetric);
			}
		        fclose(fp);	
			break;
		}
		case metric_db_session:
		{
			//const char * sql="SELECT CURR_SCH,USENAME,CLIENT_IP,COUNT(CURR_SCH) FROM SAMPLES.SYS_CATALOG.SYS_SESSION GROUP BY CURR_SCH,USENAME,CLIENT_IP ";
			const char * sql = "SELECT datname,usename,host(client_addr),count(datname) FROM SYS_STAT_ACTIVITY group by datname ,usename,client_addr;";
			int rec_num, attr_num;
			char *data_buf;
			struct ColAttr* attrs;
			ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			//cout<<"ret_code "<<ret_code<<endl;
			if(ret_code<=0)
			{
				cout<<error1.error_no<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break ;
			}
			if(ret_code > 0 && rec_num > 0)
			{
				int offset = 0;
				int len[3];
				len[0]=attrs[0].data_size;
				len[1]=attrs[1].data_size;
				len[2]=attrs[2].data_size;
				len[3]=attrs[3].data_size;
				for(int i = 0; i < rec_num; i++)
				{
					string schame="";
					string user="";
					string clientIp="";
					long count =0;
					char* val;
					for(int col = 0; col < attr_num; col++)
					{
						val = (char*)(data_buf + offset);
						if(col == 0)
						{					
							schame=string(val);
						}
						else if(col == 1)
						{
							user=string(val);
						}
						else if(col == 2)
						{
							clientIp=string(val);
						}
						else if(col == 3)
						{
							memcpy(&count, val, sizeof(val));
						}
						else if(col == 4)
						{
								
						}		
							offset += len[col];
						}
					//	cout<<"schame       "<<schame <<endl;
					//	cout<<"user  "<<user<<endl;
					//	cout<<"clientIp     "<<clientIp<<endl;
					//	cout<<"count      "<<count<<endl;
						if(count>0)
						{
							DbSessionMetric* sessionMetric = new DbSessionMetric("0", schame,user,clientIp,count);
							metrics.push_back(sessionMetric);
						}
				}
				dci->FreeReadData(attrs, attr_num, data_buf);
				//cout<<"**********************************"<<endl;
			}
			break;
		}
		/*
		case metric_db_trx:
		{
			const char * sql="select datname as database_name,age(datfrozenxid) as age_xid from sys_database where datname not in ('GLOBAL','TEMPDB') and age_xid>1000000 order by age_xid desc; ";
			int rec_num, attr_num;
			char *data_buf;
			struct ColAttr* attrs;
			ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			//cout<<"ret_code "<<ret_code<<endl;
			if(ret_code<=0)
			{
				cout << "select trx failed  " << error1.error_no<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break ;
			}
			if(ret_code > 0 && rec_num > 0)
			{
				int offset = 0;
				cout<<"rec_num    "<<rec_num<<endl;
				for(int i = 0; i < rec_num; i++)
				{
					string databaseName="";
					long currentXid;
					long ageXid;

					char* val;
					for(int col = 0; col < attr_num; col++)
					{
						val = (char*)(data_buf + offset);
						if(col == 0)
						{					
							databaseName=string(val);
						}
						else if(col == 2)
						{
							memcpy(&currentXid, val, sizeof(val));
						}
						else if(col == 3)
						{
							memcpy(&ageXid, val, sizeof(val));	
						}		
						DbTrxMetric* trxMetric = new DbTrxMetric(databaseName,currentXid,ageXid);
						metrics.push_back(trxMetric);
					}
				}
				dci->FreeReadData(attrs, attr_num, data_buf);
			}
			break;
		}
		*/
		case metric_db_io:
		{
			if(procNum < 0)
			{
				break;
			}
			long readBytes=0;
			long writeBytes=0;
			readBytes=getReadFlow(procNum);
			writeBytes=getWriteFlow(procNum);
			//cout<<"readBytes    "<<readBytes<<endl;
			//cout<<"writeBytes   "<<writeBytes<<endl;
			DbIoMetric* ioMetric = new DbIoMetric("0", 0, 0, readBytes, writeBytes);
			metrics.push_back(ioMetric);
			break;
		}
		case metric_db_long_session:
		{
			const char * sql = "select datname as DataBase,usename as Username,host(client_addr) as Host,to_char(to_date(query_start)) as Start,DATEDIFF('second',cast(query_start as TIMESTAMP),cast(now() as TIMESTAMP) ) as Elapsed ,current_query as Query from sys_stat_activity where current_query <>'<IDLE>'  and Elapsed >= 1800 order by datname";
			int rec_num, attr_num;
			char *data_buf;
			struct ColAttr* attrs;
			ret_code = dci->ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
			cout<<"ret_code "<<ret_code<<endl;
			if(ret_code<=0)
			{
				cout<< sql  <<error1.error_no<<error1.error_info<<endl;
				if(attrs != NULL)                                          
				{                                                                                                                          
					free(attrs);
				}
				break ;
			}
			if(ret_code > 0 && rec_num > 0)
			{
				int offset = 0;
				cout<<"rec_num    "<<rec_num<<endl;
				for(int i = 0; i < rec_num; i++)
				{
					string databaseName = "";
					string userName = "";
					string host = "";
					string queryStart ="";
					long cost = 0;
					string querySql = "";
					char* val;
					for(int col = 0; col < attr_num; col++)
					{
						val = (char*)(data_buf + offset);
						if(col == 0)
						{		
							databaseName=string(val);
						}
						else if(col == 1)
						{
							userName = string(val);
						}
						else if(col == 2)
						{
							host = string(val);
						}
						else if(col == 3)
						{
							queryStart = string (val);
						}
						else if(col == 4)
						{
							memcpy(&cost,val,sizeof(cost));
						}
						else if(col ==5)
						{
							querySql = string(val);
							//cout << querySql << endl;
						}
						offset += attrs[col].data_size;
					}
					DbLongSessionMetric* longSessionMetric = new DbLongSessionMetric(databaseName, userName, host, queryStart,cost,querySql);
					metrics.push_back(longSessionMetric);
					//cout << databaseName << " " << userName << " " << host << " "  << queryStart << " " << cost << endl;
				}
				dci->FreeReadData(attrs, attr_num, data_buf);
			}
			break;
		}
		default:
		{
			return ;
		}
	}
}

/*
bool MetricDao::exist(const char* table_name, const char* where_text)
{
	sprintf(sql, "select 'x' from %s where %s limit 1", table_name, where_text);

	int rec_num, attr_num, ret_code;
	char *data_buf;
	struct ColAttr* attrs;

	int count = 0;

	ret_code = storage_dci.ReadData(sql, &rec_num, &attr_num, &attrs, &data_buf, &error1);
	if(ret_code > 0 && rec_num > 0)
	{
		count = 1;		
	}
	storage_dci.FreeReadData(attrs, attr_num, data_buf);
	return count > 0;
}
*/

void MetricDao::send(vector<Metric*>& metrics, int& metricType, string& statTime)
{

		switch(metricType)
		{
		case metric_db:
			{
				DbMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_mem:
			{
				DbMemMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_tablespace:
			{
				DbTablespaceMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_io:
			{
				DbIoMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_info:
			{
				DbInfoMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_session:
			{
				DbSessionMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_long_session:
			{
				DbLongSessionMetric::sendMetric(metrics,statTime);
				break;
			}
		case metric_db_trx:
			{
				DbTrxMetric::sendMetric(metrics,statTime);
				break;
			}
		default:
			return;
		}
}
string getDbStartTime()
{
		string dbDir = Parameter::dbProcessName;
		string cmd= "ps -eO lstart | grep -w "+ dbDir+"|grep -v grep |awk '{print $6,$3,$4,$5}'";
		FILE * pp=popen(cmd.c_str(),"r");
		//cout<<cmd<<endl;
		if(pp==NULL)
		{
			pclose(pp);
			cout<<"get kingbase start time failed"<<endl;
			return "get kingbase start time failed";
		}
		char str[1024]="";	
		fgets(str,sizeof(str),pp);
		if(strlen(str)==0)
		{
			pclose(pp);
			cout<<"get kingbase start time failed"<<endl;
			return "get kingbase start time failed";
		}
		string startTime;
		startTime=str;
		startTime=startTime.substr(0,startTime.size()-1);
		pclose(pp);
		return startTime;
}

int getProcNum()
{

	string dbDir = Parameter::dbProcessName;
	string cmd= "ps -ef | grep -w "+ dbDir+"| grep -v grep |awk '{print $2}'";
	//cout<<cmd<<endl;
	FILE *pp=NULL;
	pp=popen(cmd.c_str(),"r");
	if(pp==NULL)
	{
		pclose(pp);
	      	cout<<"get kingbase  proc num  failed"<<endl;
	     	 return -1;
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	fgets(str,sizeof(str),pp);
	if(str==NULL)
	{
		pclose(pp);
		return -1;
	}
	int procNum=0;
	procNum=atoi(str);
	pclose(pp);
	return procNum;
}


long  getMemUsed(int procNum)
{
	if(procNum < 0)
       	{
		return 0;
	}
	string s1="cat /proc/";
	char tmp[64]="";
	sprintf(tmp,"%d",procNum);
	string s2=string(tmp);
	string s3="/status | grep RSS |awk '{print $2}'";
	string cmd =s1+s2+s3;
	FILE *pp=NULL;
	pp=popen(cmd.c_str(),"r");
	if(pp==NULL)
	{
	   	pclose(pp);	
		cout<<"cmd error"<<endl;
		return -1;
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	fgets(str,sizeof(str),pp);
	if(str==NULL)
	{
		pclose(pp);
		return -1;
	}
	long mem=atol(str);
	mem=mem*1024;
	pclose(pp);
	return mem;
}

long getWriteFlow(int procNum)
{
	string s1="cat /proc/";
	char tmp[64]="";
   	sprintf(tmp,"%d",procNum);
	string s2=string(tmp);
    	string s3="/io | grep -w write_bytes |awk '{print $2}'";
    	string cmd =s1+s2+s3;
    	//cout<<cmd<<endl;
   	FILE *pp=NULL;
	pp=popen(cmd.c_str(),"r");
	if(pp==NULL)
	{
		pclose(pp);
	     	cout<<"cmd error"<<endl;
	     	return -1;
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	fgets(str,sizeof(str),pp);
	if(str==NULL)
	{
		pclose(pp);
		return -1;
	}
	long flow=atol(str);
	pclose(pp);
	return flow;
}

long getReadFlow(int procNum)
{
	string s1="cat /proc/";
	char tmp[64]="";
	sprintf(tmp,"%d",procNum);
	string s2=string(tmp);
	string s3="/io | grep -w read_bytes |awk '{print $2}'";
    	string cmd =s1+s2+s3;
    	FILE *pp=NULL;
	pp=popen(cmd.c_str(),"r");
	if(pp==NULL)
	{
		pclose(pp);
		cout<<"cmd error"<<endl;
		return -1;
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	fgets(str,sizeof(str),pp);
	if(str==NULL)
	{
		pclose(pp);
		return -1;
	}
	long flow=atol(str);
	pclose(pp);
	return flow;
}

void MetricDao::clear(vector<Metric*>& metrics, int metricType)
{
	for(vector<Metric*>::iterator it = metrics.begin(); it != metrics.end(); ++it)
        {
        	switch(metricType)
                {
                 	case metric_db_mem:
                        {
                      		delete (DbMemMetric *) *it;
                                *it=NULL;
                                break;
                        }
                        case metric_db:
                        {
                                delete (DbMetric *) * it;
                                *it=NULL;
                                break;
                        }
                        case metric_db_tablespace:
                        {
                        	delete (DbTablespaceMetric *) *it;
                                *it=NULL;
                                break;
                        }
                        case metric_db_io:
                        {
                                delete (DbIoMetric *) *it;
                                *it=NULL;
                                break;
                        }
			case metric_db_long_session:
			{
				   delete (DbIoMetric *) *it;
				   *it=NULL;
				   break;
			}
			case metric_db_info:
			{
				delete (DbInfoMetric *) * it;
				*it = NULL;
				break;
			}
			case metric_db_session:
			{
				delete (DbSessionMetric *) * it;
				*it = NULL;
				break;
			}
			case metric_db_trx:
			{
				delete (DbTrxMetric *) *it;
				*it = NULL;
				break;
			}
			/*
			case metric_db_sql:
			{
                                delete (DbSqlMetric *) *it;
                                *it=NULL;
                                break;
			}*/
                        default:
                        {
                                //      return ;
                        }
      		}
	}
        metrics.clear();
        vector<Metric *>(metrics).swap(metrics);
}



float getCpuUsed(int pid)
{
	//string cmd = top -b -n 1 -p $pid | awk '{print $9}' | tail -2 | awk '{if(NR==1) print}';
	string cmd = "top -b -n 1 -p " + MyUtil::itos(pid) + "| awk '{print $9}' | tail -2 | awk '{if(NR==1) print}'";
	FILE *pp=NULL;
	pp=popen(cmd.c_str(),"r");
	if(pp==NULL)
	{
		pclose(pp);
		cout<<"cmd error"<<endl;
		return -1;
	}
	char str[1024]="";
	memset(str,0,sizeof(str));
	fgets(str,sizeof(str),pp);
	if(str==NULL)
	{
		pclose(pp);
	        return -1;
	}
	float cpuUse=MyUtil::stof(str);
	pclose(pp);
	return cpuUse;
}
