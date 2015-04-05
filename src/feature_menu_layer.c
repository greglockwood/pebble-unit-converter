#include "pebble.h"
#include "units.h"

#define NUM_MENU_SECTIONS 2

static Window *window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

static UCUnit * curr_unit() {
    return units[currentUnit];
}

static uint16_t curr_step() {
    return curr_unit()->steps[menuLevel-1];
}


// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    if (menuLevel == 0) {
        return NUM_MENU_SECTIONS;
    } else {
        return 1;
    }
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return menuLevel == 0 ? NUM_UNIT_PAIRS : ((menuMax - menuMin) / curr_step());
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  //  APP_LOG(APP_LOG_LEVEL_INFO, "Unit 0 is: %s", units[0]->full_name);
    char *heading;
            heading = (char *)malloc(30 * sizeof(char));
        if (heading == NULL) {
            // TODO: Handle out of memory
            return;
        }
    if (menuLevel == 0) {
        if (section_index == 0) {
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
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
    char *heading = "";
    char *subHeading = (char *)malloc(30 * sizeof(char));
    uint8_t unitId = cell_index->row * NUM_MENU_SECTIONS + cell_index->section;
    if (subHeading == NULL) {
        return;
    }
    if (menuLevel == 0) {
        heading = (char *)units[unitId]->full_name;
        snprintf(subHeading, 30, "to %s", units[units[unitId]->inverse_id]->full_name);
    } else {
        // TODO: Handle menu items beyond the first level
        uint16_t min = cell_index->row * curr_step() + menuMin;
        uint16_t max = min + curr_step() - 1;
        snprintf(heading, 30, "%d to %d %s", min, max, curr_unit()->abbrev);
        snprintf(subHeading, 30, "%0.2f to %0.2f %s", curr_unit()->convert((float)min), curr_unit()->convert((float)max), units[curr_unit()->inverse_id]->abbrev);
    }
    menu_cell_basic_draw(ctx, cell_layer, heading, subHeading, NULL);
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    if (menuLevel == 0) {
        currentUnit = cell_index->row * NUM_MENU_SECTIONS + cell_index->section;
        menuMin = curr_unit()->min;
        menuMax = curr_unit()->max;
        menuLevel++;
        
                APP_LOG(APP_LOG_LEVEL_INFO, "cell_index = s %d r %d curr unit %d", cell_index->section, cell_index->row, currentUnit);
    } else {
            menuLevel++;
    
        menuMin = curr_step() * cell_index->row + menuMin;
        menuMax = menuMin + curr_step() - 1;
    }
    if (menuLevel >= curr_unit()->steps_size) {
        return;
    }
    // After changing the icon, mark the layer to have it updated
    //layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

// This initializes the menu upon window load
void window_load(Window *window) {
  load_units();

  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

int main(void) {
  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, false /* Animated */);

  app_event_loop();

  window_destroy(window);
}
