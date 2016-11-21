#include "udp_pipe_obj.h"
#include "logger_obj.h"



static nrm_t *nrm_data_package;

void package_add_uuid(uint8_t *uuid, uint8_t *uuid1)
{
	int i;

	// initialize pointer
	nrm_data_package = get_data_package_pointer();

	for(i=0;i<UUID_LENGTH;i++)
		nrm_data_package->concentrator_uuid[i] = uuid[i];
	for(i=0;i<UUID_LENGTH;i++)
		nrm_data_package->nrm_uuid[i] = uuid1[i];

}

void package_add_byte(uint8_t byte, int index)
{
	if(index < NRM_PACKAGE_LENGTH)
	{
		nrm_data_package->data[index] = byte;
	}
	else
		logger_add_line("Index out of range (package_add_byte in radio_package_obj.c)");
		
}


