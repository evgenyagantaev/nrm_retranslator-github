#include "logger_obj.h"

#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>


static FILE *log_file;
static char *application_log = "/root/app_logs/nrm_retranslator_log";
static time_t current_time;



void logger_init()
{
	time(&current_time);
	log_file = fopen(application_log, "w");                                                            	
	fprintf(log_file, "%ld start log\r\n", current_time);
	fclose(log_file);                                                                              	

}
void logger_add_line(char *log_line)
{
	time(&current_time);
	log_file = fopen(application_log, "a");                                                            	
	fprintf(log_file, "%ld %s\r\n", current_time, log_line);
	fclose(log_file);                                                                              	

}






