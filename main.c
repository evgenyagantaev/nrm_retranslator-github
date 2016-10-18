#include <bcm2835.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUMBER_OF_SENSORS 19  // number of sensors really connected
#define LENGTH_OF_FILTER_ARRAY 10 // length of filter array

/*
#define CHIP_SEL0 RPI_BPLUS_GPIO_J8_03		
#define CHIP_SEL1 RPI_BPLUS_GPIO_J8_05		
#define CHIP_SEL2 RPI_BPLUS_GPIO_J8_07		
#define CHIP_SEL3 RPI_BPLUS_GPIO_J8_11		
#define CHIP_SEL4 RPI_BPLUS_GPIO_J8_13
#define CHIP_SEL5 RPI_BPLUS_GPIO_J8_15
#define CHIP_SEL6 RPI_BPLUS_GPIO_J8_29
#define CHIP_SEL7 RPI_BPLUS_GPIO_J8_31
#define CHIP_SEL8 RPI_BPLUS_GPIO_J8_33
#define CHIP_SEL9 RPI_BPLUS_GPIO_J8_35
#define CHIP_SEL10 RPI_BPLUS_GPIO_J8_37
int CHIP_SEL;
static int chip_sels[NUMBER_OF_SENSORS] = {CHIP_SEL0, CHIP_SEL1, CHIP_SEL2, CHIP_SEL3, CHIP_SEL4, CHIP_SEL5, CHIP_SEL6, CHIP_SEL7, CHIP_SEL8, CHIP_SEL9, CHIP_SEL10};
*/



#define ADDR_A0 RPI_BPLUS_GPIO_J8_29
#define ADDR_A1 RPI_BPLUS_GPIO_J8_31
#define ADDR_A2 RPI_BPLUS_GPIO_J8_33
#define ADDR_A3 RPI_BPLUS_GPIO_J8_35
#define ADDR_A4 RPI_BPLUS_GPIO_J8_37
int addressA[] = {ADDR_A0, ADDR_A1, ADDR_A2, ADDR_A3, ADDR_A4};
uint8_t address_a = 0;

const int sleep_period = 1;

#define number_of_sensors 19

// variables *****************************************************************************************
#include "/root/uuid"   // uuid of this machine

int16_t dataToTransmit[9* number_of_sensors];
int16_t gyro_calibration[number_of_sensors][3];
int16_t magnet_calibration[number_of_sensors][3];

// vector zakreplyaet sootvetstvie chipselektov i datchikov:
// poziciya v massive - nomer datchika, chislo - nomer sootvetstvuyuschego chipselekta
// "-1" - ne naznachen 
int assignment[] = {0,1,2,   3,4,5,   6,7,8,   9,10,11,   12,13,14,   15,16,17,   18};  // po poryadku
//int assignment[] = {-1,-1,-1,   -1,-1,-1,   -1,-1,0,   0,0,-1,   -1,-1,-1,   -1,-1,-1};  // levaya ruka celikom
//int assignment[] = {13,14,15,   16,-1,-1,   -1,-1,-1,   -1,-1,-1,   -1,-1,-1,   -1,-1,-1};  // 13 - 16 (test)
//int assignment[] = {8,9,10,   11,12,13,   -1,-1,-1,   -1,-1,-1,   -1,-1,-1,   -1,-1,-1};  // 8 - 13 (test)

void *pointer;

int16_t sensorsData[number_of_sensors][9];
double norma[number_of_sensors];
int16_t filter_array[NUMBER_OF_SENSORS][9][LENGTH_OF_FILTER_ARRAY];

int udp_send_thread_parameter = 0;

const uint8_t MAGNET_I2C_ADDRESS = 0x0c;

int16_t data_4x_board[9];

int32_t thermo_data[34];


// thermo data struct
typedef struct  
{
	uint8_t concentrator_uuid[16];
	uint8_t sensors_uuid[16];
	time_t timestamp;
	uint32_t order_number;
	int32_t temperatures[16];
	int32_t fluxes[16];
	int32_t temp_oral;
	int32_t temp_rectal;
} thermo_t;

thermo_t thermo_data_package;

// udp frame struct
struct 
{
	uint16_t hd_id;
	uint16_t ps;
	thermo_t thermo_data_package;
	uint16_t crc16;
} udp_frame;



// functions ********************************************************

#include "udp_send_thread.c"
#include "metab_udp_thread.c"

void pause(void)
{
	int i;
	int x, y = 5;
	for(i=0; i<100; i++)
		x = y;
}

void one_usec_delay(void)
{
	volatile int i = 0;
	for(i=0; i<33; i++)
	{
		i++;
		i--;
	}
}

void chipselhigh()
{
	long i;

	// test delay
	for(i=0; i<10; i++)
	{
		i++;
		i--;
	}


	bcm2835_gpio_write(ADDR_A4, HIGH); 	
	bcm2835_gpio_write(ADDR_A0, HIGH); 	
	bcm2835_gpio_write(ADDR_A1, HIGH); 	
	bcm2835_gpio_write(ADDR_A2, HIGH); 	
	bcm2835_gpio_write(ADDR_A3, HIGH); 	
}

void set_address(uint8_t address)
{
	// u nas dve serii adresov: 0 - 14 i 16 - ...

	//if(address >= 15)
		//address += 1;

	int i;
	uint8_t mask = 0x01;
	for(i=0; i<5; i++)
	{
		if(address & mask)
			bcm2835_gpio_write(addressA[i], HIGH); 
		else
			bcm2835_gpio_write(addressA[i], LOW); 
		mask = mask << 1;

	}

}



uint8_t read_byte(int address)
{

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000;	// 

	uint8_t return_data;
	uint8_t data_out;

	chipselhigh();

	//bcm2835_gpio_write(CHIP_SEL, LOW); // open SPI communication
	set_address(address_a);
	nanosleep(&sleep_interval, NULL);
	data_out = (uint8_t)0x80 | (uint8_t)address;
	bcm2835_spi_transfer(data_out);// send command (read register)
	return_data = bcm2835_spi_transfer(data_out);// read register data
	//bcm2835_gpio_write(CHIP_SEL, HIGH); // close SPI communication	
	chipselhigh();
	nanosleep(&sleep_interval, NULL);

	return return_data;
}

void read_data_from_thermo_board()
{
	int i;

	struct timespec sleep_interval;
	struct timespec long_sleep_interval;
	struct timespec long1_sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000;	// 1 uSec
	long_sleep_interval.tv_sec = 0;
	long_sleep_interval.tv_nsec = 4000000;	// 4 mSec 
	long1_sleep_interval.tv_sec = 0;
	long1_sleep_interval.tv_nsec = 2000000;	// 50 uSec 

	int16_t return_data;
	uint8_t data_msb;
	uint8_t data_lsb;
	uint8_t data_out;
	uint16_t data_aux;

	chipselhigh();
	set_address(address_a);
	nanosleep(&sleep_interval, NULL);
	data_out = 0x21; // komanda chteniya
	bcm2835_spi_transfer(data_out);// send data
	//data_out = 0; // index v massive dannyh
	//bcm2835_spi_transfer(data_out);// send data
	chipselhigh();
	//set_address(address_a);
	nanosleep(&sleep_interval, NULL);  // wait for controller loads appropriate data

	for(i=0; i<(34*2); i++)
	{
		set_address(address_a);                                    	
	    data_out = 0x00;
	    data_msb = bcm2835_spi_transfer(data_out);// read data
	    data_lsb = bcm2835_spi_transfer(data_out);// read data
	    chipselhigh();
	    nanosleep(&sleep_interval, NULL);
                                                                   
	    data_aux = ((uint16_t)data_msb)<<8 + (uint16_t)data_lsb;
	    data_4x_board[i] = (int16_t)data_aux;                           
		//printf("%7d   ");	
		//nanosleep(&long1_sleep_interval, NULL);  // wait for controller loads appropriate data
	}
	//printf("\r\n");

}


void write_byte(int address, uint8_t data)
{

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000;	// 

	uint8_t data_out;

	chipselhigh();

	//bcm2835_gpio_write(CHIP_SEL, LOW); // open SPI communication
	set_address(address_a);
	nanosleep(&sleep_interval, NULL);
	data_out = (uint8_t)address;
	bcm2835_spi_transfer(data_out);// send command (write register)
	bcm2835_spi_transfer(data);// write register data
	//bcm2835_gpio_write(CHIP_SEL, HIGH); // close SPI communication	
	chipselhigh();
	nanosleep(&sleep_interval, NULL);
}

int16_t read_word(int high_address, int low_address)
{

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000;	// 

	uint8_t high_byte;
	uint8_t low_byte;
	uint8_t data_out;
	uint16_t buffer;
	int16_t result;

	chipselhigh();

	//bcm2835_gpio_write(CHIP_SEL, LOW); // open SPI communication
	set_address(address_a);
	nanosleep(&sleep_interval, NULL);
	data_out = (uint8_t)0x80 | (uint8_t)high_address;
	bcm2835_spi_transfer(data_out);// send command (read register)
	high_byte = bcm2835_spi_transfer(data_out);// read register data
	//bcm2835_gpio_write(CHIP_SEL, HIGH); // close SPI communication	
	chipselhigh();
	nanosleep(&sleep_interval, NULL);
	//bcm2835_gpio_write(CHIP_SEL, LOW); // open SPI communication
	set_address(address_a);
	nanosleep(&sleep_interval, NULL);
	data_out = (uint8_t)0x80 | (uint8_t)low_address;
	bcm2835_spi_transfer(data_out);// send command (read register)
	low_byte = bcm2835_spi_transfer(data_out);// read register data
	//bcm2835_gpio_write(CHIP_SEL, HIGH); // close SPI communication	
	chipselhigh();
	nanosleep(&sleep_interval, NULL);
	
	buffer = ((uint16_t)high_byte)<<8;
	buffer |= (uint16_t)low_byte;
	result = (int16_t)buffer;

	return result;
}



// returns 0 if everything ok, returns 1 if timeout
int write_i2c_byte(uint8_t address, uint8_t value)
{
	uint8_t reg52_value;
	const uint8_t x80 = 0x80;

	int i = 0;
	int doJob;

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000000;	// 10 uSec 
	
	write_byte(49, MAGNET_I2C_ADDRESS); // komanda zapis' v magnetometr
	write_byte(50, address); // adres registra magnetometra
	write_byte(51, value); // znachenie, kotoroe nado zapisat' v registr magnetometra
	write_byte(52, 0x80); // zapuskaem obmen

	// wait for a transfer completion
	doJob = 1;
	while((i<100) && doJob)
	{
		reg52_value = read_byte(52);

		if((reg52_value & x80) == 0) // transfer complete
			doJob = 0;
		else
		{
			i++;
			nanosleep(&sleep_interval, NULL);
		}

	}

	if(doJob) // timeout occured
		return 1;
	else	// everything ok
		return 0;

}


uint8_t read_i2c_byte(uint8_t address)
{
	uint8_t reg52_value;
	const uint8_t x80 = 0x80;
	uint8_t magnetometer_data = 0;

	int i = 0;
	int doJob;

	struct timespec sleep_interval;

	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 1000000;	// 10 uSec 
	
	write_byte(49, (uint8_t)0x80 | MAGNET_I2C_ADDRESS); // komanda chtenie iz magnetometra
	write_byte(50, address); // adres registra magnetometra
	write_byte(52, 0x80); // zapuskaem obmen

	// wait for a transfer completion
	doJob = 1;
	while((i<100) && doJob)
	{
		reg52_value = read_byte(52);

		if((reg52_value & x80) == 0) // transfer complete
			doJob = 0;
		else
		{
			i++;
			nanosleep(&sleep_interval, NULL);
		}

	}

	if(doJob) // timeout occured
		return 0xff;
	else	// everything ok
	{
		magnetometer_data = read_byte(53);
		return magnetometer_data;
	}

}

// reads magnetometer data; returns them through parameters
void read_magnetometer_data(int16_t *hx, int16_t *hy, int16_t *hz)
{
	uint8_t high_byte;
	uint8_t low_byte;
	uint16_t buffer;

	high_byte = read_i2c_byte(0x04);
	low_byte = read_i2c_byte(0x03);
	buffer = ((uint16_t)high_byte)<<8;
	buffer |= (uint16_t)low_byte;
	*hx = (int16_t)buffer;

	high_byte = read_i2c_byte(0x06);
	low_byte = read_i2c_byte(0x05);
	buffer = ((uint16_t)high_byte)<<8;
	buffer |= (uint16_t)low_byte;
	*hy = (int16_t)buffer;

	high_byte = read_i2c_byte(0x08);
	low_byte = read_i2c_byte(0x07);
	buffer = ((uint16_t)high_byte)<<8;
	buffer |= (uint16_t)low_byte;
	*hz = (int16_t)buffer;
	
	//printf("st2 = %x\r\n", read_i2c_byte(0x09)); // read status2 (finish data reading)
	read_i2c_byte(0x09); // read status2 (finish data reading)
}



// prints data stored in a specified mpu 9250m register
void print_register(uint8_t address)
{
	printf("reg %d = %x\r\n", address, read_byte(address));
}



// smoothes data from sensors (5 values 1min + 1max cut)
int16_t smoothed_data(int16_t *array, int16_t value)
{
	int i, j, k;

	int16_t aux;

	int16_t smoothed_value;
	int16_t sorted_array[5];

	//shift source  array
	for(i=1; i<5; i++)
		array[i-1] = array[i];
	//add value into array
	array[4] = value;

	// sort array ******************
	//copy source array into sorted one
	for(i=0; i<5; i++)
		sorted_array[i] = array[i];
	// buble sorting
	for(i=5; i>0; i--)
	{
		for(j=1; j<i; j++)
		{
			if(sorted_array[j] < sorted_array[j-1])
			{
				//exchange
				aux = sorted_array[j];
				sorted_array[j] = sorted_array[j-1];
				sorted_array[j-1] = aux;
			}

		}
	}

	// calculate filtered value
	smoothed_value = 0;
	for(i=1; i<4; i++)
		smoothed_value += sorted_array[i];
	smoothed_value /= 3;

	//smoothed_value = value; // bez filtracii

	return smoothed_value;
}// end smoothed_data()



/*
// smoothes data from sensors (10 values 3min + 3max cut)
int16_t smoothed_data(int16_t *array, int16_t value)
{
	int i, j, k;

	int16_t aux;

	int16_t smoothed_value;
	int16_t sorted_array[10];

	//shift source  array
	for(i=1; i<10; i++)
		array[i-1] = array[i];
	//add value into array
	array[9] = value;

	// sort array ******************
	//copy source array into sorted one
	for(i=0; i<10; i++)
		sorted_array[i] = array[i];
	// buble sorting
	for(i=10; i>0; i--)
	{
		for(j=1; j<i; j++)
		{
			if(sorted_array[j] < sorted_array[j-1])
			{
				//exchange
				aux = sorted_array[j];
				sorted_array[j] = sorted_array[j-1];
				sorted_array[j-1] = aux;
			}

		}
	}

	// calculate filtered value
	smoothed_value = 0;
	for(i=3; i<7; i++)
		smoothed_value += sorted_array[i];
	smoothed_value /= 4;

	//smoothed_value = value;

	return smoothed_value;
}// end smoothed_data()
//*/




// end functions ********************************************************


//*****************************
//*****************************
//*****************************
//*****************************
//*****************************

int main(int argc, char *argv)
{


	struct timespec sleep_interval, long_sleep_interval, short_sleep_interval;


	short_sleep_interval.tv_sec = 0;
	short_sleep_interval.tv_nsec = 500000;   // 
	sleep_interval.tv_sec = 0;
	sleep_interval.tv_nsec = 24000000;	// 25 mSec
	long_sleep_interval.tv_sec = 0;
	long_sleep_interval.tv_nsec = 100000000;	// 100 mSec


	pthread_t udpSendThread;
	int threadId;

	pointer = dataToTransmit;


// local variables
//****************************
	int i, j, k, l;


// end of local variables block
//************************************


	udp_frame.hd_id = 0x5448;
	udp_frame.ps = sizeof(thermo_t);
	udp_frame.crc16 = 0x5555;

	for(i=0;i<16;i++)
		thermo_data_package.concentrator_uuid[i] = uuid[i];
	for(i=0;i<16;i++)
		thermo_data_package.sensors_uuid[i] = uuid1[i];

	// Inicializiruem periferiyu
	if(!bcm2835_init())
	{
		printf("\nbcm2835 initialization failed\n");
		return 1;
	}
	else // Everything OK with periphery
	{
		printf("\nTest of raspberry pi SPI interface with thermo board.\n");

		// Inicializiruem MPU9250
		#include "init.c"


		// start a thread which will send sensors data via udp through specifyed interval
		pthread_create(&udpSendThread, NULL, udp_send_thread, (void *)(&udp_send_thread_parameter));

		uint16_t *aux_pointer = (uint16_t *)thermo_data;
		//pointer = (void *)thermo_data;
		pointer = (void *)(&udp_frame);
		
		address_a = 20;// address of thermo board
		chipselhigh();
		int doJob = 1;
		int counter = 0;
		while(doJob)
		{
			
			uint8_t data_msb;
			uint8_t data_lsb;
			uint8_t data_out;
			uint16_t data_aux;

			
			chipselhigh();
	        set_address(address_a);                                            	
			data_out = 0x21; // komanda chteniya                           	    
	        bcm2835_spi_transfer(data_out);// send data                            
	        one_usec_delay();                                                      
			one_usec_delay();                                                      
			chipselhigh();                                                         
	        one_usec_delay();                                                      
			one_usec_delay();                                                      
	                                                                                                          
			printf("request sent\n");																											  
	                                                                                                           
	        //read data from thermo board                                          
			for(i=0; i<(34*2); i++)                                                                                 
			{                                                                      
	        	set_address(address_a);                                            	
	            data_out = 0x55;                                                   
	            data_msb = bcm2835_spi_transfer(0x55);// read data                 
	            data_lsb = bcm2835_spi_transfer(0xaa);// read data                 
	        	one_usec_delay();                                                  
				one_usec_delay();                                                  
			    chipselhigh();                                                     
	        	one_usec_delay();                                                  
				one_usec_delay();                                                  
	                                                                               
                data_aux = (((uint16_t)data_msb)<<8) + (uint16_t)data_lsb;     
				aux_pointer[i] = data_aux;
				
	        }  

			for(i=0;i<34;i++)
				printf("%d\n", thermo_data[i]);

			counter++;
			printf("data received %d\n", counter);
                                                                                            
			sleep(5);	


		}// end while (main loop)

		
		// Vyklyuchaem SPI
		bcm2835_spi_end();
		// Deinicializiruem periferiyu
		bcm2835_close();

	
		return 0;
	}//end if(!bcm2835_init())
}// end main

//***************************************************
//***************************************************
//***************************************************
//***************************************************
//***************************************************

