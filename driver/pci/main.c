/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>
#include <rdi/io.h>

struct pci_header {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t  revision_id;
	uint8_t  prog_if;
	uint8_t  subclass;
	uint8_t  class_code;
	uint8_t  cache_line_size;
	uint8_t  latency_timer;
	uint8_t  header_type;
	uint8_t  bist;
	uint32_t bar[5];
	uint32_t cardbus_cis_ptr;
	uint16_t subsystem_id;
	uint16_t subsystem_vendor_id;
	uint32_t rom_base_address;
	uint8_t  capabilities;
	uint8_t  reserved0[3];
	uint32_t reserved1;
	uint8_t  interrupt_line;
	uint8_t  interrupt_pin;
	uint8_t  min_grant;
	uint8_t  max_latency;
};

static void _pci_read(uint32_t bus, uint32_t slot, uint32_t func, struct pci_header *hdr) {
	uint32_t addr;
	uint32_t i;
	uint32_t *hdr_data = (void*) hdr;

	addr = (bus << 16) | (slot << 11) | (func << 8) | 0x80000000;

	for (i = 0; i < 15; i++) {
		outd(0xCF8, addr | (i << 2));
		hdr_data[i] = ind(0xCFC);
	}
}

static bool _pci_ping(uint32_t bus, uint32_t slot, uint32_t func) {
	uint32_t addr;
	uint32_t input;

	addr = (bus << 16) | (slot << 11) | (func << 8) | 0x80000000;

	input = ind(0xCFC);

	if ((input & 0xFFFF0000) == 0xFFFF0000 || (input & 0xFFFF0000) == 0) {
		return false;
	}
	
	return true;
}

static size_t pci_read(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	uint8_t *data;

	data = robject_data(self, "pci-header");

	if (!data) {
		return 0;
	}

	if (offset > sizeof(struct pci_header)) {
		return 0;
	}

	if (offset + size >= sizeof(struct pci_header)) {
		size = sizeof(struct pci_header) - offset;
	}

	memcpy(buffer, &data[offset], size);

	return size;
}

int main(int argc, char **argv) {
	uint32_t slot, func, bus;
	struct pci_header *hdr;
	struct robject *root;
	struct robject *device;
	char *name;

	rdi_init();

	root = rdi_dir_cons(1, ACCS_READ | ACCS_WRITE);

	for (bus = 0; bus < 256; bus++) {
		for (slot = 0; slot < 32; slot++) {
			for (func = 0; func < 8; func++) {
				if (_pci_ping(bus, slot, func)) {

					hdr = malloc(sizeof(struct pci_header));
					_pci_read(bus, slot, func, hdr);

					device = rdi_file_cons((bus << 16) | (slot << 11) | (func << 8) + 2, ACCS_READ);
					robject_set_data(device, "pci-header", hdr);
					name = saprintf("/b%2Xs%2Xf%1X", bus, slot, func);
					rdi_vfs_add(root, name, device);
					free(name);

					printf("%02x:%02x.%x :\n", bus, slot, func);
					printf("\tdevice ID: %X\n", hdr->device_id);
					printf("\tvendor ID: %X\n", hdr->vendor_id);
					printf("\tclass: %X %X\n", hdr->class_code, hdr->subclass);

					for (int i = 0; i < 5; i++) {
						printf("\tBAR%d: %X\n", i, hdr->bar[i]);
					}
				}
			}
		}
	}

	rdi_global_read_hook = pci_read;

	msendb(getppid(), ACTION_CHILD);
	_done();

	return 0;
}
