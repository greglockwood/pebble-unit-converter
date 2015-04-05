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

UCUnit * curr_unit() {
    return units[currentUnit];
}

uint16_t curr_step() {
    UCUnit *unit = curr_unit();
    if (unit == NULL) {
        return -1;
    } else {
        //APP_LOG(APP_LOG_LEVEL_INFO, "curr_step(): unit abbrev: %s, steps_size: %d, steps[0-3]: [%d, %d, %d], steps[menuLevel-1] = steps[%d]: %d", unit->abbrev, unit->steps_size, unit->steps[0], unit->steps[1], unit->steps[2], menuLevel - 1, unit->steps[menuLevel - 1]);
        return unit->steps[menuLevel-1];
    }
}

MenuData * menudata_create(uint8_t index) {
	MenuData * md = (MenuData*)malloc(sizeof(MenuData));
	
	md->index = index;
	
	_uc_info("Initialised menu data");
	return md;
}

void menudata_destroy(MenuData * data) {
	_uc_info("Destroying menu data");
	free(data);
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

char* floatToString(char* buffer, uint8_t bufferSize, double number)
{
    char decimalBuffer[5];

    snprintf(buffer, bufferSize, "%d", (uint16_t)number);
    strcat(buffer, ".");

    snprintf(decimalBuffer, 5, "%02d", (uint16_t)((double)(number - (uint16_t)number) * (double)100));
    strcat(buffer, decimalBuffer);

    return buffer;
}
