#include "uart_obj.h"

#include "logger_obj.h"

static uint8_t uart_data_buffer[DATA_BUFFER_LENGTH];
static int uart_buffer_index = 0;

static int uart_file_descriptor; // serial port file descriptor


void uart_open_port()
{
	
	char *portname = "/dev/ttyAMA0";
	
	uart_file_descriptor  = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (uart_file_descriptor < 0)
    {
			logger_add_line("uart opening error");
            return;
    }
	else
		logger_add_line("uart opening OK");
    
    set_interface_attribs (uart_file_descriptor, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (uart_file_descriptor, 1);                // set blocking                                 
}


void uart_push_data(uint8_t data)
{
	// save new data in buffer
	if(uart_buffer_index < DATA_BUFFER_LENGTH)
	{
		uart_data_buffer[uart_buffer_index] = data;
		uart_buffer_index++;
	}
}


uint8_t uart_pop_data()
{
	// get eldest data from buffer
	uint8_t return_value = uart_data_buffer[0];
	// shift data buffer
	int i;
	for(i=0;i<(uart_buffer_index - 1);i++)
		uart_data_buffer[i] = uart_data_buffer[i+1];
	uart_buffer_index--;

	return return_value;
}



uint8_t uart_read_byte()
{
	uint8_t input_byte;
	read (uart_file_descriptor, &input_byte, 1);  // read 1 byte (blocking call)

	return input_byte;
}


void *uart_data_receiving_thread(void *parameter)
{

	logger_add_line("working thread started;");

	while(1)
	{
		uint8_t incoming_byte;

		incoming_byte = uart_read_byte();
		uart_push_data(incoming_byte);
	}
}




int set_interface_attribs (int fd, int speed, int parity)
{
	struct termios tty;                                                               
    memset (&tty, 0, sizeof tty);                                                 
    if (tcgetattr (fd, &tty) != 0)                                                
    {                                                                             
            //error_message ("error %d from tcgetattr", errno);                     
            return -1;                                                            
    }                                                                             
                                                                              
    cfsetospeed (&tty, speed);                                                    
    cfsetispeed (&tty, speed);                                                    
                                                                              
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars                
    // disable IGNBRK for mismatched speed tests; otherwise receive break         
    // as \000 chars                                                              
    tty.c_iflag &= ~IGNBRK;         // disable break processing                   
    tty.c_lflag = 0;                // no signaling chars, no echo,               
                                    // no canonical processing                    
    tty.c_oflag = 0;                // no remapping, no delays                    
    tty.c_cc[VMIN]  = 0;            // read doesn't block                         
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout                   
                                                                              
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl             
                                                                              
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,                     
                                    // enable reading                             
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity                    
    tty.c_cflag |= parity;                                                        
    tty.c_cflag &= ~CSTOPB;                                                       
    tty.c_cflag &= ~CRTSCTS;                                                      
                                                                              
    if (tcsetattr (fd, TCSANOW, &tty) != 0)                                       
    {                                                                             
    	//error_message ("error %d from tcsetattr", errno);                     
        return -1;                                                            
    }                                                                             
    return 0;                                                                     
}



void set_blocking (int fd, int should_block)
{
	struct termios tty;                                                
    memset (&tty, 0, sizeof tty);                                         
    if (tcgetattr (fd, &tty) != 0)                                        
    {                                                                     
    	//error_message ("error %d from tggetattr", errno);             
        return;                                                       
    }                                                                     
                                                                      
    tty.c_cc[VMIN]  = should_block ? 1 : 0;                               
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout           
                                                                      
    if (tcsetattr (fd, TCSANOW, &tty) != 0)                               
	{
    	//error_message ("error %d setting term attributes", errno);    
	}
}







