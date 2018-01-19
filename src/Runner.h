
#ifndef __RUNNER_H__
#define __RUNNER_H_
#include "iostream"
using namespace std;
class Runner {
public:
	string statTime;
	static string node;
	Runner(string time);
	int collectHisDbUseThread();
	int collectSqlUseThread();
	void * collectSql();

	void * collectStaticInfo ();
	void *  collectHisDb();

	static void * switchDb();
	static void * switchDbUseThread();
	static void * sendStartAlarm();
        static void * sendSwitchAlarm(string oldNode,string newNode);
};
#endif
