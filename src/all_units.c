#include <pebble.h>
#include "units.h"
    
float miles_to_kilometres(float input) {
    return input * 1.6093472;
}   

float kilometres_to_miles(float input) {
    return input * 0.621;
}

static uint16_t std_steps[] = { 50, 10, 1 };

void load_units(void) {
    uint8_t std_steps_size = 3;
    uint16_t std_min = 0;
    uint16_t std_max = 500;
    units[MILES] = ucunit_create(MILES, "Miles", "mi", miles_to_kilometres, KILOMETRES, std_min, std_max, &std_steps[0], std_steps_size);
    units[KILOMETRES] = ucunit_create(KILOMETRES, "Kilometres", "km", kilometres_to_miles, MILES, std_min, std_max, &std_steps[0], std_steps_size);
}