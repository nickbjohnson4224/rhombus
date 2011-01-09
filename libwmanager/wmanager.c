#include <wmanager.h>
#include <stdlib.h>
#include <natio.h>
#include <page.h>
#include <ipc.h>

static uint32_t wmanager_pid;

int wm_init(void) {
	uint64_t wmanager;

	wmanager = io_find("/sys/wmanager");

	if (!wmanager) {
		return 1;
	}

	wmanager_pid = RP_PID(wmanager);

	return 0;
}

int window_message(uint8_t port, uint8_t id, size_t width, size_t height, uint8_t bitmap) {
	struct msg *msg = malloc(sizeof(struct msg));
	msg->count = 1;
	msg->packet = aalloc(msg->count * PAGESZ, PAGESZ);
	((uint32_t*) msg->packet)[0] = id;
	((uint32_t*) msg->packet)[1] = width;
	((uint32_t*) msg->packet)[2] = height;
	((uint32_t*) msg->packet)[3] = bitmap;
	msend(port, wmanager_pid, msg);
	struct msg *reply = mwaits(PORT_REPLY, wmanager_pid);
	return *(int*)reply->packet;
}

int wm_set_bitmap(uint8_t id, uint8_t *addr, size_t size) {
	struct msg *msg = malloc(sizeof(struct msg));
	uint8_t first_byte = addr[0];
	addr[0] = id;
	msg->count = size / PAGESZ + 1;
	msg->packet = aalloc(size, PAGESZ);
	page_self(addr, msg->packet, size);
	page_prot(msg->packet, size, PROT_READ);
	msend(WMANAGER_PORT_SET_BITMAP, wmanager_pid, msg);
	struct msg *reply = mwaits(PORT_REPLY, wmanager_pid);
	addr[0] = first_byte;
	return *(int*)reply->packet;
}

int wm_add_window(uint8_t id, size_t width, size_t height, uint8_t bitmap) {
	return window_message(WMANAGER_PORT_ADD_WINDOW, id, width, height, bitmap);
}

int wm_set_window(uint8_t id, size_t width, size_t height, uint8_t bitmap) {
	return window_message(WMANAGER_PORT_SET_WINDOW, id, width, height, bitmap);
}

int wm_destroy_window(uint8_t id) {
	struct msg *msg = malloc(sizeof(struct msg));
	msg->count = 1;
	msg->packet = aalloc(msg->count * PAGESZ, PAGESZ);
	*(uint32_t*) msg->packet = id;
	msend(WMANAGER_PORT_DESTROY_WINDOW, wmanager_pid, msg);
	struct msg *reply = mwaits(PORT_REPLY, wmanager_pid);
	return *(int*)reply->packet;
}
