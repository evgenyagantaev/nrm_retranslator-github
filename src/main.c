#include <main.h>

#include "uart_obj.h"
#include "logger_obj.h"
#include "udp_pipe_obj.h"

#include "/root/uuid.h"   // uuid of this machine
//uint8_t uuid[16];
//uint8_t uuid1[16];




//*****************************   MAIN
//*****************************   MAIN
//*****************************   MAIN

int main()
{

	logger_init();
	logger_add_line("<< application started >>");

	uart_open_port();
	udp_pipe_init();
	package_add_uuid(uuid, uuid1);

	
	// create and start uart read thread 
	pthread_t uart_Data_Receiving_Thread;
	
	// start a thread which will receive data from uart
	pthread_create(&uart_Data_Receiving_Thread, NULL, uart_data_receiving_thread, NULL);
	
	while(1)
	{
		sleep(1);
	}
	
	
	return 0;

}// end main


//***************************** END  MAIN
//***************************** END  MAIN
//***************************** END  MAIN
