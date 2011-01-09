#ifndef FLUX_WMANAGER_H
#define FLUX_WMANAGER_H

#include <stdint.h>

#define WMANAGER_RETURN_OK 0
#define WMANAGER_RETURN_INVALID_BITMAP_ID 1
#define WMANAGER_RETURN_INVALID_WINDOW_ID 2
#define WMANAGER_RETURN_INVALID_SIZE 3
#define WMANAGER_PORT_SET_BITMAP 32
#define WMANAGER_PORT_ADD_WINDOW (WMANAGER_PORT_SET_BITMAP + 1)
#define WMANAGER_PORT_SET_WINDOW (WMANAGER_PORT_SET_BITMAP + 2)
#define WMANAGER_PORT_DESTROY_WINDOW (WMANAGER_PORT_SET_BITMAP + 3)

int wm_init(void);

int wm_set_bitmap(uint8_t id, uint8_t *addr, size_t size);
int wm_add_window(uint8_t id, size_t width, size_t height, uint8_t bitmap);
int wm_set_window(uint8_t id, size_t width, size_t height, uint8_t bitmap);
int wm_destroy_window(uint8_t id);

#endif
