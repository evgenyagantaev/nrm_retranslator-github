#include "radio_package_obj.h"
#include "udp_pipe_obj.h"

static radio_package_buffer[PACKAGE_LENGTH + UUID_LENGTH]
static package_buffer_index = 0;



void package_add_uuid()
{
	int i;
	for(i=0; i<(UUID_LENGTH-1); i++)
		radio_package_buffer[i] = 0;
	radio_package_buffer[UUID_LENGTH - 1] = 0x01;
}

void package_add_byte(uint8_t byte)
{
	radio_package_buffer[package_buffer_index + UUID_LENGTH];

	if(package_buffer_index >= PACKAGE_LENGTH)
	{
		package_buffer_index = 0;
		// send package via udp pipe
		udp_translate(&radio_package_buffer, PACKAGE_LENGTH + UUID_LENGTH);
	}
}


