#include "iostream"
#include "alarmConfig.h"
#include "Util.h"
using namespace std;
int AlarmConfig::getAlarmConfInfo()
{
	string Home =getD5000HomePath();
	char path[256]="";
	string ConfigFile=Home+"/conf/AlarmInfo.conf";
	strcpy(path,ConfigFile.c_str());
	FILE *pp;
	pp = fopen(path,"r");
	if(NULL == pp)
	{
		cout<<ConfigFile<<"open failed,check you path"<<endl;
		return -1;
	}
	string str_code;
	string str_content;
	char buf[128];
	string str = "";
	int index = 0;
	while(fgets(buf, 128, pp) != NULL)
	{
		str = buf;
		str = str.substr(0,str.length()-1);
		index = str.find_first_of(":", 0);
		if(index > 0)
		{
			str_code = str.substr(0, index);
			str_content = str.substr(index+1, str.length());
			if(str_code ==  "memAlarm" || str_code == "00020009")
			{
				//  s_sendinfo.m_ip_back = str_content.c_str();
				memUsedAlarm=atoi(str_content.c_str());
				//  cout<<"serverIpBak    :"<<endl;
			}
			else if(str_code ==  "sessionAlarm" || str_code == "00020008")
			{
				//  s_sendinfo.m_ip_back = str_content.c_str();
				sessionAlarm=atoi(str_content.c_str());
				//  cout<<"serverIpBak    :"<<endl;
			}
		}
	}
	fclose(pp);
	return 0;
}

int AlarmConfig::getConfInfo()
{

	string Home =getD5000HomePath();
	char path[256]="";
	string ConfigFile=Home+"/conf/auto_monitor.conf";
	strcpy(path,ConfigFile.c_str());
	FILE *pp;
	pp = fopen(path,"r");
	if(NULL == pp)
	{
		cout<<ConfigFile<<"open failed,check you path"<<endl;
		return -1;
	}
	string str_code;
	string str_content;
	char buf[128];
	string str = "";
	int index = 0;
	while(fgets(buf, 128, pp) != NULL)
	{
		str = buf;
		str = str.substr(0,str.length()-1);
		index = str.find_first_of(":", 0);
		if(index > 0)
		{
			str_code = str.substr(0, index);
			str_content = str.substr(index+1, str.length());
			if(str_code ==  "alarmport")
			{
				alarmMainPort=atoi(str_content.c_str());
			}
			else if(str_code=="alarmport_bak")
			{
				alarmPortBak=atoi(str_content.c_str());
			}
			else if(str_code=="alarmip")
			{
				alarmMainIp=string(str_content);
			}
			else if(str_code=="alarmip_bak")
			{
				alarmIpBak=string(str_content);
			}
		}
	}
	fclose(pp);
	return 0;
}
