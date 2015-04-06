#include <pebble.h>
#pragma once

SimpleMenuSection *simplemenusection_create(const char *title, SimpleMenuItem *items, uint16_t items_count);
void simplemenusection_destroy(const SimpleMenuSection *section);
SimpleMenuItem *simplemenuitem_create(const char *title, const char *subtitle, SimpleMenuLayerSelectCallback select_callback);
void simplemenuitem_destroy(const SimpleMenuItem *item);