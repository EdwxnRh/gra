#pragma once
#include <stdbool.h>
#define _debug(text, ...) if (DEBUG) {printf("[DEBUG] " text, ##__VA_ARGS__);}

extern bool DEBUG;