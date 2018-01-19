#include "iostream"
#include "stdlib.h"
#include "RunTableSpaceShell.h"
#include "pthread.h"
using namespace std;
string cmd;
int runShell(string shell)
{
	cmd = shell;
	pthread_attr_t pth_attr_acc;
	pthread_attr_init(&pth_attr_acc);
	pthread_attr_setdetachstate(&pth_attr_acc,PTHREAD_CREATE_DETACHED);
	pthread_t pth_acc;
	int ret = pthread_create(&pth_acc,&pth_attr_acc,run,NULL);	
	if(ret != 0)	
	{
		cout<<"create thread error"<<endl;
	}
	return 1;
}


void * run(void *)
{
	system(cmd.c_str());
	return (void *) NULL;
}
/*
int main()
{
	string shell = "./../kdbDataMonitor/tablespaceCollect.sh";
	runShell(shell);
	return 1;
}
*/
