#include "udp_pipe_obj.h"


void udp_pipe_init()
{
	int sock;                                                     
	struct sockaddr_in addr;

	i = 1;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		printf("\r\nsocket failure ******************\r\n");
		*((int *)param) = 1; 
		return (void *)param;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(51112);
	//addr.sin_addr.s_addr = inet_addr("192.168.173.2");
	//addr.sin_addr.s_addr = inet_addr("192.168.1.1");
	//addr.sin_addr.s_addr = inet_addr("192.168.1.3");
	addr.sin_addr.s_addr = inet_addr("192.168.0.89");
	//addr.sin_addr.s_addr = inet_addr("192.168.0.191");
	//addr.sin_addr.s_addr = inet_addr("192.168.43.139");


	
}




void udp_translate(uint8_t *buffer, int buffer_length)
{
	sendto(sock, (char *)buffer, buffer_length, 0, (struct sockaddr *)&addr, sizeof(addr));
	
}
