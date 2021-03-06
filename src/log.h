/********************************************************
function:log class
version:1.0
date:2013.4.19
modify:2014.12.21
*********************************************************/

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include "block_queue.h"
using namespace std;

class Log
{
	public:

		static Log* get_instance()
		{
			static Log instance;
			return &instance;
		}
		
		static void *flush_log_thread(void* args)
		{
			Log::get_instance()->async_write_log();
		}

		bool init(const char* file_name, int log_buf_size = 8192, int split_lines = 5000000, int max_queue_size = 0, char * mode = "day");

		void write_log(int level, const char* format, ...);
		
		//void write_syslog(char *);

		void flush(void);

	private:
		Log();
		virtual ~Log();
		string fullName;
		string getLogName();
		char timeMode[8];
		void *async_write_log()
		{
			string single_log;
			while(m_log_queue->pop(single_log))
			{
				pthread_mutex_lock(m_mutex);
				fputs(single_log.c_str(), m_fp);
				pthread_mutex_unlock(m_mutex);
			}
		}

	private:
		pthread_mutex_t *m_mutex;
		char dir_name[128];
		char log_name[128];
		int m_split_lines;
		int m_log_buf_size;
		long long  m_count;
		int m_today;
		FILE *m_fp;
		char *m_buf;
		block_queue<string> *m_log_queue;
		bool m_is_async;
};

#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0,"%s: line%ld :"format,__FILE__,__LINE__, __VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, "%s: line%ld :"format,__FILE__,__LINE__, __VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, "%s: line%ld :"format,__FILE__,__LINE__, __VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3,"%s: line%ld :"format,__FILE__,__LINE__, __VA_ARGS__)

#endif 

