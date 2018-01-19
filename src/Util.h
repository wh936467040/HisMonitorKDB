#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <limits.h>
#include <string>

#ifdef WIN32
#define DIR_CHAR		'\\'
#else
#define DIR_CHAR		'/'
#endif

#ifdef WIN32
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
#elif ULONG_MAX > 4294967295UL
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef int int32_t;
typedef long int64_t;
#else
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef long long int64_t;
#endif

#define INT64_C(val) val##i64
#define EPOCH_DELTA INT64_C(11644473600000000)

using namespace std;

class Util
{
public:
	static void log(const char* msg);

	static void log(string& msg);

	static void sleep(unsigned int ms);

	static char* getTime(char* timeBuf);

	static char* getTime(char* timeBuf, long second);	

	static int getSysTime(char* timeBuf, int second);

	static long getTimeInSeconds();

	static int64_t getTimeInMills();

	static long math_round(double x);

	static double math_dot1(double x);

	static double math_dot2(double x);
	
	static int stricmp2(const char * str1, const char * str2);

	static void str_replace(string& s1, const string& s2, const string& s3);

	static void getIp(char* host);
};


string getHomePath();
string getD5000HomePath();
string itos(int i);
string ftos(float f);
string f2tos(float f);
string getSysTime();

#endif
