#include "pebble.h"
#include "units.h"

#define NUM_MENU_SECTIONS 2
#define MAX_SUB_MENUS 6

static Window* menu_windows[MAX_SUB_MENUS];
static MenuLayer* menu_layers[MAX_SUB_MENUS];

static void show_next_level();

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer* menu_layer, void* data)
{
    _uc_info("menu_get_num_sections_callback()");
    if(menuLevel == 0) {
		return NUM_MENU_SECTIONS;
    } else {
		return 1;
    }
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer* menu_layer, uint16_t section_index, void* data)
{
    _uc_info("menu_get_num_rows_callback()");
    uint16_t num_rows = curr_step() > 0 ? ((menuMax - menuMin) / curr_step()) : 0;
    //APP_LOG(APP_LOG_LEVEL_INFO, "menuMax: %d, menuMin: %d, menuLevel: %d, curr_step(): %d, num_rows: %d", menuMax, menuMin, menuLevel, curr_step(), num_rows);
    
    return menuLevel == 0 ? NUM_UNIT_PAIRS : num_rows;
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer* menu_layer, uint16_t section_index, void* data)
{
    // This is a define provided in pebble.h that you may use for the default height
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer* cell_layer, uint16_t section_index, void* data)
{
    //  APP_LOG(APP_LOG_LEVEL_INFO, "Unit 0 is: %s", units[0]->full_name);
    _uc_info("menu_draw_header_callback(): start");
    char* heading;
    heading = (char*)malloc(30 * sizeof(char));
    if(heading == NULL) {
		// TODO: Handle out of memory
		return;
    }
    if(menuLevel == 0) {
		if(section_index == 0) {
			heading = "Imperial to Metric";
		} else {
			heading = "Metric to Imperial";
		}
    } else {
		snprintf(heading, 30, "%s to %s", units[currentUnit]->full_name, units[units[currentUnit]->inverse_id]->full_name);
    }
    menu_cell_basic_header_draw(ctx, cell_layer, heading);
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* data)
{
    _uc_info("menu_draw_row_callback(): start");
    // Determine which section we're going to draw in
    char* heading = "";
    char* subHeading = (char*)malloc(30 * sizeof(char));
    uint8_t unitId = cell_index->row * NUM_MENU_SECTIONS + cell_index->section;
    if(subHeading == NULL) {
		return;
    }
    if(menuLevel == 0) {
		heading = (char*)units[unitId]->full_name;
		snprintf(subHeading, 30, "to %s", units[units[unitId]->inverse_id]->full_name);
    } else {
		// TODO: Handle menu items beyond the first level
		uint16_t min = cell_index->row * curr_step() + menuMin;
		uint16_t max = min + curr_step() - 1;
		snprintf(heading, 30, "%d to %d %s", min, max, curr_unit()->abbrev);
        uint8_t num_length = 10;
        char* num1 = (char*)malloc(num_length * sizeof(char));
        char* num2 = (char*)malloc(num_length * sizeof(char));
		snprintf(subHeading,
				 30,
				 "%s to %s %s",
				 floatToString(num1, num_length, curr_unit()->convert((float)min)),
				 floatToString(num2, num_length, curr_unit()->convert((float)max)),
				 units[curr_unit()->inverse_id]->abbrev);
    }
    _uc_info("menu_draw_row_callback(): about to log text to draw");
    APP_LOG(APP_LOG_LEVEL_INFO, "menu_draw_row_callback(): about to draw basic menu cell with heading: %s", heading);
    APP_LOG(APP_LOG_LEVEL_INFO, "menu_draw_row_callback(): subheading: %s", subHeading);
    menu_cell_basic_draw(ctx, cell_layer, heading, subHeading, NULL);
    _uc_info("menu_draw_row_callback(): finish");
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* data)
{
    if(menuLevel == 0) {
		currentUnit = cell_index->row * NUM_MENU_SECTIONS + cell_index->section;
		menuMin = curr_unit()->min;
		menuMax = curr_unit()->max;
		menuLevel++;

		APP_LOG(APP_LOG_LEVEL_INFO,
				"cell_index = s %d r %d curr unit %d",
				cell_index->section,
				cell_index->row,
				currentUnit);
    } else {
		menuLevel++;

		menuMin = curr_step() * cell_index->row + menuMin;
		menuMax = menuMin + curr_step() - 1;
    }
    if(menuLevel >= curr_unit()->steps_size) {
		return;
    }
	show_next_level();
    // After changing the icon, mark the layer to have it updated
    // layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

// This initializes the menu upon window load
void window_load(Window* window)
{
    _uc_info("window_load(): start");
	MenuData *data = (MenuData *)window_get_user_data(window);
    // Now we prepare to initialize the menu layer
    // We need the bounds to specify the menu layer's viewport size
    // In this case, it'll be the same as the window's
    Layer* window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    // Create the menu layer
    menu_layers[data->index] = menu_layer_create(bounds);

    _uc_info("window_load(): setting menu layer callbacks");
    // Set all the callbacks for the menu layer
    menu_layer_set_callbacks(menu_layers[data->index],
                             data,
                             (MenuLayerCallbacks){
	                         .get_num_sections = menu_get_num_sections_callback,
	                         .get_num_rows = menu_get_num_rows_callback,
	                         .get_header_height = menu_get_header_height_callback,
	                         .draw_header = menu_draw_header_callback,
	                         .draw_row = menu_draw_row_callback,
	                         .select_click = menu_select_callback,
	                     });

    // Bind the menu layer's click config provider to the window for interactivity
    menu_layer_set_click_config_onto_window(menu_layers[data->index], window);

    _uc_info("window_load(): adding menu layer to window layer");
    // Add it to the window for display
    layer_add_child(window_layer, menu_layer_get_layer(menu_layers[data->index]));
    _uc_info("window_load(): added menu layer to window layer");
}

void window_unload(Window* window)
{
    _uc_info("Unloading window");
	MenuData *data = (MenuData *)window_get_user_data(window);
    // Destroy the menu layer
    menu_layer_destroy(menu_layers[data->index]);
}

static void show_next_level()
{
    _uc_info("show_next_level(): start");
	if (menu_windows[menuLevel] == NULL) {
        _uc_info("show_next_level(): creating window");
		menu_windows[menuLevel] = window_create();

		// Setup the window handlers
		window_set_window_handlers(menu_windows[menuLevel],
								   (WindowHandlers){
								   .load = window_load, .unload = window_unload,
							   });
							   
		MenuData *data = menudata_create(menuLevel);
		window_set_user_data(menu_windows[menuLevel], data);
	}

    _uc_info("show_next_level(): pushing window onto stack");
    window_stack_push(menu_windows[menuLevel], menuLevel > 0 /* Animated */);
}


