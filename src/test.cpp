#include "iostream"
#include "string"
#include "stdio.h"
using namespace std;
string getTime(int stime)
{

	cout << "**********************"<<endl;
	char str[128];
	int year, mon, day, hour, min, sec;
	struct tm *ctm;
	time_t ts;
	int index = 0;
	cout << "11111111111111111111"<< endl;
        ts = time(NULL);
	ctm = localtime(&ts);
	cout << "------------------" << endl;
        hour = ctm->tm_hour; 
	cout <<"********************"<<endl;
        min = ctm->tm_min;
        sec = ctm->tm_sec;
	cout << "-------------------"<< endl;
        sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
	cout << "22222222222222222222"<< endl;
        if(sec <= stime)
        {
                index = sec;
                sec = stime;
                sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
                cout << "sleep time :" <<stime<<" -  "<<index <<"  =  "<<stime - index << endl;
                sleep(stime - index);
        }
        else
        {
                sleep(60 - sec);
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
	cout << "88888888888888888" << endl;
        return string(str);
}

int main()
{
	cout << getTime(30)<< endl;
	return 1;
}
