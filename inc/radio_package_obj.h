#include "time.h"
#include <stdint.h>


#define UUID_LENGTH 16
#define NRM_PACKAGE_LENGTH 3000

// nrm data struct
typedef struct  
{
	uint8_t concentrator_uuid[UUID_LENGTH];
	uint8_t nrm_uuid[UUID_LENGTH];
	time_t timestamp;
	uint32_t order_number;
	uint8_t data[NRM_PACKAGE_LENGTH];
} nrm_t;


void package_add_uuid(uint8_t *uuid, uint8_t *uuid1);
void package_add_byte(uint8_t byte, int index);

