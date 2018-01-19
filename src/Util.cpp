#include <time.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#endif

#include <iostream> 

#include "Util.h"

using namespace std;

void Util::log(const char* msg)
{
	char buf[20] = {0};
	cout<<Util::getTime(buf)<<" "<<msg<<endl;
}

void Util::log(string& msg)
{
	char buf[20] = {0};
	cout<<Util::getTime(buf)<<" "<<msg<<endl;
}

void Util::sleep(unsigned int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	if (ms == 0)
	{
		sched_yield();
	}
	else
	{
		usleep(ms * 1000);
	}
#endif
}

char* Util::getTime(char* timeBuf)
{
	time_t raw_time;
#ifdef WIN32
	struct tm time_info;		
	time(&raw_time);
	localtime_s(&time_info, &raw_time);
	strftime(timeBuf, 64, "%Y-%m-%d %H:%M:%S", &time_info);		
#else
	struct tm* time_info;
	time(&raw_time);
	time_info = localtime(&raw_time);
	strftime(timeBuf, 64, "%Y-%m-%d %H:%M:%S", time_info);
#endif
	return timeBuf;
}

char* Util::getTime(char* timeBuf, long second)
{
	time_t raw_time;
#ifdef WIN32
	struct tm time_info;		
	raw_time = second;
	localtime_s(&time_info, &raw_time);
	strftime(timeBuf, 64, "%Y-%m-%d %H:%M:%S", &time_info);		
#else
	struct tm* time_info;
	raw_time = second;
	time_info = localtime(&raw_time);
	strftime(timeBuf, 64, "%Y-%m-%d %H:%M:%S", time_info);
#endif
	return timeBuf;
}

long Util::getTimeInSeconds()
{
#ifdef WIN32
	time_t raw_time;
	time(&raw_time);
	return (long)raw_time;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long)tv.tv_sec;
#endif
}

int64_t Util::getTimeInMills()
{
#ifdef WIN32
	int64_t time;

	SYSTEMTIME st;
	FILETIME ft;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
		
	time = ft.dwHighDateTime;
	time = time << 32;
	time |= ft.dwLowDateTime;
	time /= 10;
	time -= EPOCH_DELTA;

	return time / 1000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000000L) + tv.tv_usec) / 1000L;
#endif
}

long Util::math_round(double x)
{
	return ((long)(x + 0.5) > (long)x) ? ((long)x + 1) : (long)x;
}

double Util::math_dot1(double x)
{
	double t;
	long i;
	t = x * 10;
	i = Util::math_round(t);
	return i * 1.0 / 10;
}
	
double Util::math_dot2(double x)
{
	double t;
	int i;
	t = x * 100;
	i = Util::math_round(t);
	return i * 1.0 / 100;
}

int Util::stricmp2(const char * str1, const char * str2)
{
#ifdef WIN32
	return _stricmp(str1, str2);
#else
	return strcasecmp(str1, str2);
#endif
}

void Util::str_replace(string& s1, const string& s2, const string& s3)
{
	string::size_type pos = 0;
	string::size_type a = s2.size();
	string::size_type b = s3.size();
	while((pos = s1.find(s2, pos)) != string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}

void Util::getIp(char* host)
{
#ifndef WIN32
	struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct != NULL) {
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {
			char addressBuffer[INET_ADDRSTRLEN];
			int len;
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			len = strlen(addressBuffer);
			if (strlen(ifAddrStruct->ifa_name) == 0 || strncmp(ifAddrStruct->ifa_name, "eth", 3) != 0)
			{
				ifAddrStruct = ifAddrStruct->ifa_next;
				continue;
			}
			if(Util::stricmp2(addressBuffer, "127.0.0.1") == 0 || Util::stricmp2(addressBuffer, "localhost") == 0)
			{
				ifAddrStruct = ifAddrStruct->ifa_next;
				continue;
			}
			if(len< 7 || len > 20)
			{
				ifAddrStruct = ifAddrStruct->ifa_next;
				continue;
			}
			cout<<"ip:"<<addressBuffer<<endl;
            strcpy(host, addressBuffer);
			break;
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
#endif
}

string getHomePath()
{
	char path[128] = "";
	sprintf(path, "%s", getenv("HOME"));
	string result=string(path);
	return result;
}
				
string getD5000HomePath()
{
	char path[128] = "";
	sprintf(path, "%s", getenv("D5000_HOME"));
	string result=string(path);
	return result;
}  
							 
string itos(int i)
{
	char tmp[16]="";
	sprintf(tmp,"%d",i);
	return string(tmp);
}

string ftos(float f)
{
	char tmp[128]="";
	sprintf(tmp,"%f",f);
	return string(tmp);
}

string f2tos(float f)
{
	char tmp[128]="";
	sprintf(tmp,"%.2f",f);
	return tmp;
}
	
int Util::getSysTime(char* str, int stime)
{
	struct tm *ctm;   
	time_t ts;
	int year, mon, day, hour, min, sec;
	int index = 0;
	ts = time(NULL);
	ctm = localtime(&ts);
	year = ctm->tm_year + 1900;
	mon = ctm->tm_mon + 1;
	day = ctm->tm_mday;
	hour = ctm->tm_hour;
	min = ctm->tm_min;
	sec = ctm->tm_sec;
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
	if(sec < stime)
	{
		index = sec;
		sec = stime;
		sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
		cout << "sleep time :" << stime - index << endl;
		sleep((stime - index)*1000);
	}
	else
	{
		sleep((60 - sec)*1000);
		cout << "sleep time :" << 60 - sec << endl;
		ts = time(NULL);
		ctm = localtime(&ts);   
		year = ctm->tm_year + 1900;
		mon = ctm->tm_mon + 1;
		day = ctm->tm_mday;
		hour = ctm->tm_hour;
		min = ctm->tm_min;
		sec = ctm->tm_sec;
		sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
	}
	return 1;
}            
