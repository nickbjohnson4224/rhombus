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

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>

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

static void pci_read(uint32_t bus, uint32_t slot, uint32_t func, struct pci_header *hdr) {
	uint32_t addr;
	uint32_t i;
	uint32_t *hdr_data = (void*) hdr;

	addr = (bus << 16) | (slot << 11) | (func << 8) | 0x80000000;

	for (i = 0; i < 15; i++) {
		outd(0xCF8, addr | (i << 2));
		hdr_data[i] = ind(0xCFC);
	}
}

static bool pci_ping(uint32_t bus, uint32_t slot, uint32_t func) {
	uint32_t addr;

	addr = (bus << 16) | (slot << 11) | (func << 8) | 0x80000000;

	if (ind(0xCFC) == 0xFFFFFFFF) return false;
	
	return true;
}

int main(int argc, char **argv) {
	uint32_t slot, func, bus;
	struct pci_header hdr;

/*	if (argc < 2) {
		fprintf(stderr, "%s: error: no bus number specified\n", getname_s());
		return 1;
	}

	bus = atoi(argv[1]);

	if (bus > 255) {
		fprintf(stderr, "%s: error: bus number out of range\n", getname_s());
		return 1;
	} */

	for (bus = 0; bus < 256; bus++) {

	if (!pci_ping(bus, 0, 0)) {
		continue;
	}
	
	for (slot = 0; slot < 32; slot++) {
		for (func = 0; func < 8; func++) {
			if (pci_ping(bus, slot, func)) {
				pci_read(bus, slot, func, &hdr);

				printf("%02x:%02x.%x :\n", bus, slot, func);
				printf("\tdevice ID: %X\n", hdr.device_id);
				printf("\tvendor ID: %X\n", hdr.vendor_id);
				printf("\tclass: %X %X\n", hdr.class_code, hdr.subclass);

				for (int i = 0; i < 5; i++) {
					printf("\tBAR%d: %X\n", i, hdr.bar[i]);
				}
			}
		}
	}

	}

	return 0;
}
