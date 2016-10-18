void *udp_send_thread(void *param)
{
	int doJob = 1;
	int i,j,k;
	long counter_long;

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	//sleep_interval.tv_nsec = 15000000;	// 15 miliseconds
	sleep_interval.tv_nsec = 25000000;	// 


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

	printf("\r\%s\r\n", inet_ntoa(addr.sin_addr.s_addr));


	i = 0;
	counter_long = 0;
	while(doJob)
	{
		thermo_data_package.order_number = i;
		thermo_data_package.timestamp = time(NULL);
		for(j=0;j<16;j++)
			thermo_data_package.temperatures[j] = thermo_data[j]; 
		for(j=16;j<32;j++)
			thermo_data_package.fluxes[j-16] = thermo_data[j]; 
		thermo_data_package.temp_oral = thermo_data[32];
		thermo_data_package.temp_rectal = thermo_data[33];

		udp_frame.thermo_data_package = thermo_data_package;
		i++;

		// send data via udp socket ***************************************************
		//sendto(sock, (char *)pointer, 34*4, 0, (struct sockaddr *)&addr, sizeof(addr));
		sendto(sock, (char *)pointer, sizeof(udp_frame), 0, (struct sockaddr *)&addr, sizeof(addr));
		//*****************************************************************************

		//nanosleep(&sleep_interval, NULL);
		sleep(5);
	}
}
