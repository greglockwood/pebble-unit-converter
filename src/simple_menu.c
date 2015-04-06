#include <pebble.h>
#include "simple_menu.h"

SimpleMenuSection *simplemenusection_create(const char *title, SimpleMenuItem *items, uint16_t items_count) {
    SimpleMenuSection *section = (SimpleMenuSection *)malloc(sizeof(SimpleMenuSection));

    section->title = title;
    section->items = items;
    section->num_items = items_count;
    
    return section;
}

void simplemenusection_destroy(const SimpleMenuSection *section) {
    if (section == NULL) return;
    for (uint16_t i = 0; i < section->num_items; i++) {
        simplemenuitem_destroy(&(section->items[i]));
    }
    free((void *)section);
}

SimpleMenuItem *simplemenuitem_create(const char *title, const char *subtitle, SimpleMenuLayerSelectCallback select_callback) {
    SimpleMenuItem *item = (SimpleMenuItem *)malloc(sizeof(SimpleMenuItem));
    
    item->title = title;
    item->subtitle = subtitle;
    item->icon = NULL;
    item->callback = select_callback;
    
    return item;
}

void simplemenuitem_destroy(const SimpleMenuItem *item) {
    free((void *)item);
}