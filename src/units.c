#include <pebble.h>
#include "units.h"

const bool UC_LOGGING = true;

UCUnit *units[NUM_UNITS];

uint8_t menuLevel = 0;
uint16_t menuMin = 0;
uint16_t menuMax = 0;
uint8_t currentUnit = -1;

void _uc_info(const char * message);
void _uc_warn(const char * message);
void _uc_error(const char * message);

UCUnit * ucunit_create(uint8_t id, 
						const char *full_name,
                        const char *abbrev,
						 UCConvertHandler convert,
                      uint8_t inverse_id,
                      uint16_t min,
                       uint16_t max,
                      uint16_t *steps,
                      uint8_t steps_size) {
	UCUnit * u = (UCUnit*)malloc(sizeof(UCUnit));
	
	u->id = id;
    u->full_name = full_name;
    u->abbrev = abbrev;
    u->convert = convert;
    u->inverse_id = inverse_id;
    u->min = min;
    u->max = max;
    u->steps = steps;
    u->steps_size = steps_size;
	
	_uc_info("Initialized Unit ");
	return u;
}

void ucunit_destroy(UCUnit * unit) {
	_uc_info("Destroying unit");
    free(unit->steps);
	free(unit);
}

void _uc_info(const char * message) {
	if(UC_LOGGING)
		APP_LOG(APP_LOG_LEVEL_INFO, message);
}

void _uc_warn(const char * message) {
	if(UC_LOGGING)
		APP_LOG(APP_LOG_LEVEL_WARNING, message);
}

void _uc_error(const char * message) {
	if(UC_LOGGING)
		APP_LOG(APP_LOG_LEVEL_ERROR, message);
}

