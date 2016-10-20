#include <main.h>

#include "uart_obj.h"
#include "logger_obj.h"


//*****************************   MAIN
//*****************************   MAIN
//*****************************   MAIN

int main()
{

	logger_init();
	logger_add_line("<< application started >>");

	uart_open_port();
	
	// create and start uart read thread 
	
	return 0;

}// end main


//***************************** END  MAIN
//***************************** END  MAIN
//***************************** END  MAIN
