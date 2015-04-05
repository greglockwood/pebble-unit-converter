#pragma once
    
#define NUM_UNITS 2
#define NUM_UNIT_PAIRS 1
    
#define MILES 0
#define KILOMETRES 1

    
/**
 * Whether diagnostic information of unit events should be logged.
 * You may toggle this in the c file.
 */
extern const bool T3_LOGGING;

/**
 * The function signature for a conversion function that returns a float given the input value.
 */
typedef float (*UCConvertHandler)(float input);

/**
 * The UCUnit type.
 *
 * This is the element that will be created and should be passed to the
 * interface functions.
 */
typedef struct _uc_Unit {
    uint8_t id;
    const char *full_name;
    const char *abbrev;
    UCConvertHandler convert;
    uint8_t inverse_id;
    uint16_t min;
    uint16_t max;
    uint8_t steps_size;
    uint16_t *steps;
} UCUnit;

UCUnit * ucunit_create(uint8_t id, 
						const char *full_name,
                        const char *abbrev,
						 UCConvertHandler convert,
                      uint8_t inverse_id,
                      uint16_t min,
                       uint16_t max,
                      uint16_t *steps,
                      uint8_t steps_size);

void ucunit_destroy(UCUnit * unit);

extern UCUnit *units[NUM_UNITS];
extern uint8_t menuLevel;
extern uint16_t menuMin;
extern uint16_t menuMax;
extern uint8_t currentUnit;
extern void load_units(void);