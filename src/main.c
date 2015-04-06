#include <pebble.h>
#include "simple_menu.h"
#include "units.h"
    
Window *main_window;
static SimpleMenuSection main_window_sections[NUM_MAIN_WINDOW_SECTIONS];
static SimpleMenuItem top_section_items[NUM_UNIT_PAIRS];
static SimpleMenuItem bottom_section_items[NUM_UNIT_PAIRS];
static SimpleMenuLayer *main_window_menu_layer;

static void main_menu_select_callback(int index, void *ctx) {
    _uc_info("Selected an item");
}
    
void init_main_menu_items() {    
    int top_item_count = 0, bottom_item_count = 0;
    	    
    for (int u = 0; u < NUM_UNITS; u++) {
        int section_index = u % 2;
        char * subtitle = (char *)malloc(30 * sizeof(char));
        snprintf(subtitle, 30, "to %s", units[units[u]->inverse_id]->full_name);

        if (section_index == 0) {
            top_section_items[top_item_count++] = (SimpleMenuItem) {
                .title = units[u]->full_name,
                .subtitle = subtitle,
                .callback = main_menu_select_callback
            };
        } else {
            bottom_section_items[bottom_item_count++] = (SimpleMenuItem) {
                .title = units[u]->full_name,
                .subtitle = subtitle,
                .callback = main_menu_select_callback,
            };
        }
    }
    
    int section_count = 0;
    for (int s = 0; s < NUM_MAIN_WINDOW_SECTIONS; s++) {
        main_window_sections[section_count++] = (SimpleMenuSection) {
            .title = (s == 0 ? "Imperial to Metric" : "Metric to Imperial"),
            .items = (s == 0 ? top_section_items : bottom_section_items),
            .num_items = NUM_UNIT_PAIRS,
        };
    }
}

void main_window_load(Window *w) {
    Layer *window_layer = window_get_root_layer(w);
    GRect bounds = layer_get_frame(window_layer);

    main_window_menu_layer = simple_menu_layer_create(bounds, w, main_window_sections, NUM_MAIN_WINDOW_SECTIONS, NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(main_window_menu_layer));
}

void main_window_unload(Window *w) {
    simple_menu_layer_destroy(main_window_menu_layer);
} 

void init_main_window() {
    main_window = window_create();
    
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
    window_stack_push(main_window, false /* animated */);
}

static void init() {
    load_units();
    init_main_menu_items();
    init_main_window();    
}   

static void deinit() {
    _uc_info("deinit() started");
    window_destroy(main_window);
    for (int u = 0; u < NUM_UNITS; u++) {
        ucunit_destroy(units[u]);
    }
    _uc_info("deinit() finished");
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}