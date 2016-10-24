#include "radio_package_obj.h"

static radio_package_buffer[PACKAGE_LENGTH + UUID_LENGTH]
static package_buffer_index =UUID_LENGTH;



void package_add_uuid()
{
	radio_package_buffer[UUID_LENGTH - 1] = 0x01;
}

void package_add_byte(uint8_t byte)
{
	
}


