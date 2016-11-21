#include "udp_pipe_obj.h"

// udp frame struct
static struct 
{
	uint16_t hd_id;
	uint16_t ps;
	nrm_t nrm_data_package;
	uint16_t crc16;
} udp_frame;

static void *pointer = (void *)(&udp_frame);

nrm_t *get_data_package_pointer()
{
	return (nrm_t *)(&(udp_frame.nrm_data_package));
}


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

	udp_frame.hd_id = 0x5448;
	udp_frame.ps = sizeof(thermo_t);
	udp_frame.crc16 = 0x5555;

	
}




void udp_translate()
{
	
	sendto(sock, (char *)pointer, sizeof(udp_frame), 0, (struct sockaddr *)&addr, sizeof(addr));
	
}
