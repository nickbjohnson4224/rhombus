#include <lib.h>

//
// vga mode switcher by Jonas Berlin -98 <jberlin@cc.hut.fi>
//

#define SZ(x) (sizeof(x)/sizeof(x[0]))

// misc out (3c2h) value for various modes

#define R_COM  0x63 // "common" bits

#define R_W256 0x00
#define R_W320 0x00
#define R_W360 0x04
#define R_W376 0x04
#define R_W400 0x04

#define R_H200 0x00
#define R_H224 0x80
#define R_H240 0x80
#define R_H256 0x80
#define R_H270 0x80
#define R_H300 0x80
#define R_H360 0x00
#define R_H400 0x00
#define R_H480 0x80
#define R_H564 0x80
#define R_H600 0x80

static u8int hor_regs [] = { 0x0,  0x1,  0x2,  0x3,  0x4, 0x5,  0x13 };

static u8int width_256[] = { 0x5f, 0x3f, 0x40, 0x82, 0x4a, 0x9a, 0x20 };
static u8int width_320[] = { 0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x28 };
static u8int width_360[] = { 0x6b, 0x59, 0x5a, 0x8e, 0x5e, 0x8a, 0x2d };
static u8int width_376[] = { 0x6e, 0x5d, 0x5e, 0x91, 0x62, 0x8f, 0x2f };
static u8int width_400[] = { 0x70, 0x63, 0x64, 0x92, 0x65, 0x82, 0x32 };

static u8int ver_regs  [] = { 0x6,  0x7,  0x9,  0x10, 0x11, 0x12, 0x15, 0x16 };

static u8int height_200[] = { 0xbf, 0x1f, 0x41, 0x9c, 0x8e, 0x8f, 0x96, 0xb9 };
static u8int height_224[] = { 0x0b, 0x3e, 0x41, 0xda, 0x9c, 0xbf, 0xc7, 0x04 };
static u8int height_240[] = { 0x0d, 0x3e, 0x41, 0xea, 0xac, 0xdf, 0xe7, 0x06 };
static u8int height_256[] = { 0x23, 0xb2, 0x61, 0x0a, 0xac, 0xff, 0x07, 0x1a };
static u8int height_270[] = { 0x30, 0xf0, 0x61, 0x20, 0xa9, 0x1b, 0x1f, 0x2f };
static u8int height_300[] = { 0x70, 0xf0, 0x61, 0x5b, 0x8c, 0x57, 0x58, 0x70 };
static u8int height_360[] = { 0xbf, 0x1f, 0x40, 0x88, 0x85, 0x67, 0x6d, 0xba };
static u8int height_400[] = { 0xbf, 0x1f, 0x40, 0x9c, 0x8e, 0x8f, 0x96, 0xb9 };
static u8int height_480[] = { 0x0d, 0x3e, 0x40, 0xea, 0xac, 0xdf, 0xe7, 0x06 };
static u8int height_564[] = { 0x62, 0xf0, 0x60, 0x37, 0x89, 0x33, 0x3c, 0x5c };
static u8int height_600[] = { 0x70, 0xf0, 0x60, 0x5b, 0x8c, 0x57, 0x58, 0x70 };

// the chain4 parameter should be 1 for normal 13h-type mode, but 
// only allows 320x200 256x200, 256x240 and 256x256 because you
// can only access the first 64kb

// if chain4 is 0, then plane mode is used (tweaked modes), and
// you'll need to switch planes to access the whole screen but
// that allows you using any resolution, up to 400x600

static u32int scrdim[2];

int vga_init(u32int width, u32int height, int chain4) {
	const u8int *w,*h;
	u8int val;
	unsigned int a;

	switch (width) {
		case 256: w=width_256; val=R_COM | R_W256; break;
		case 320: w=width_320; val=R_COM | R_W320; break;
		case 360: w=width_360; val=R_COM | R_W360; break;
		case 376: w=width_376; val=R_COM | R_W376; break;
		case 400: w=width_400; val=R_COM | R_W400; break;
		default: return 0; // fail
	}
	switch (height) {
		case 200: h = height_200; val |= R_H200; break;
		case 224: h = height_224; val |= R_H224; break;
		case 240: h = height_240; val |= R_H240; break;
		case 256: h = height_256; val |= R_H256; break;
		case 270: h = height_270; val |= R_H270; break;
		case 300: h = height_300; val |= R_H300; break;
		case 360: h = height_360; val |= R_H360; break;
		case 400: h = height_400; val |= R_H400; break;
		case 480: h = height_480; val |= R_H480; break;
		case 564: h = height_564; val |= R_H564; break;
		case 600: h = height_600; val |= R_H600; break;
		default: return 0; // fail
	}

	// chain4 not available if mode takes over 64k

	if (chain4 && width * height > 65536) return 0; 

	// here goes the actual modeswitch

	outb(0x3C2, val);
	outw(0x3D4, 0x0E11); // enable regs 0-7

	for (a = 0; a < SZ(hor_regs); ++a) 
		outw(0x3D4, (u16int)((w[a] << 8) + hor_regs[a]));

	for (a = 0; a < SZ(ver_regs); ++a)
		outw(0x3D4, (u16int)((h[a] << 8) + ver_regs[a]));

	outw(0x3D4, 0x0008); // vert.panning = 0

	if (chain4) outw(0x3D4, 0x4014);
	else outw(0x3D4, 0x0014);

	outw(0x3D4, 0xa317);
	outw(0x3C4, 0x0E04);

	outw(0x3C4, 0x0101);
	outw(0x3C4, 0x0F02); // enable writing to all planes
	outw(0x3CE, 0x4005); // 256color mode
	outw(0x3CE, 0x0506); // graph mode & A000-AFFF

	inb(0x3DA);
	outb(0x3C0, 0x30);
	outb(0x3C0, 0x41);
	outb(0x3C0, 0x33); 
	outb(0x3C0, 0x00);

	for (a = 0; a < 16; a++) {    // ega pal
		outb(0x3C0, (u8int) a); 
		outb(0x3C0, (u8int) a); 
	}

	// Create full 2 bit color palette at 0
	outb(0x3C6, 0xFF);
	for (a = 0; a < 64; a++) {
		outb(0x3C8, (u8int) a);
		outb(0x3C9, (u8int) (a & 0x30) << 0);
		outb(0x3C9, (u8int) (a & 0x0C) << 2);
		outb(0x3C9, (u8int) (a & 0x03) << 4);
	}

	// Create full 6 bit grayscale palette at 64
	for (a = 0; a < 64; a++) {
		outb(0x3C8, (u8int) a + 64);
		outb(0x3C9, (u8int) a & 0x3F);
		outb(0x3C9, (u8int) a & 0x3F);
		outb(0x3C9, (u8int) a & 0x3F);
	}

	const u8int hue_table[6][3] = {
		{ 0x3F, 0x00, 0x00 },
		{ 0x3F, 0x3F, 0x00 },
		{ 0x00, 0x3F, 0x00 },
		{ 0x00, 0x3F, 0x3F },
		{ 0x00, 0x00, 0x3F },
		{ 0x3F, 0x00, 0x3F }
	};

	// Create 6x7 bit hue palette at 128
	for (a = 0; a < 42; a++) {
		outb(0x3C8, (u8int) a + 128);
		u8int hue = a % 6;
		u8int value = (a >> 3) % 7;
		if (val == 3) {
			outb(0x3C9, hue_table[hue][0]);
			outb(0x3C9, hue_table[hue][1]);
			outb(0x3C9, hue_table[hue][2]);
		}
		else if (val > 3) {
			outb(0x3C9, (hue_table[hue][0]) ? hue_table[hue][0] : (value - 3) * 0x10);
			outb(0x3C9, (hue_table[hue][1]) ? hue_table[hue][1] : (value - 3) * 0x10);
			outb(0x3C9, (hue_table[hue][2]) ? hue_table[hue][2] : (value - 3) * 0x10);
		}
		else {
			outb(0x3C9, (hue_table[hue][0]) ? value * 0x10 : hue_table[hue][0]);
			outb(0x3C9, (hue_table[hue][1]) ? value * 0x10 : hue_table[hue][1]);
			outb(0x3C9, (hue_table[hue][2]) ? value * 0x10 : hue_table[hue][2]);
		}
	}

	outb(0x3C0, 0x20); // enable video

	scrdim[0] = width;
	scrdim[1] = height;

	return 1;
}

int vga_write(int x, int y, u32int buffer, u16int size) {
	u16int offset = x + (y * scrdim[0]);
	if (size + offset > scrdim[0] * scrdim[1]) return 1;

	u16int i;
	for (i = 0; i < size; i += 0x4000)
		push(0, 0xA0000 + offset + i, buffer, min(size - i, 0x4000));

	return 0;
}

int vga_flip(u32int buffer) {
	u32int i = 0;
	for (i = 0; i + 0x100 < scrdim[0] * scrdim[1]; i += 0x100)
		push(0, 0xA0000 + i, buffer + i, min((scrdim[0] * scrdim[1]) - i, 0x200));

	return 0;
}

int vga_plot(int x, int y, u8int color, u8int *buffer) {
	u16int offset = x + (y * scrdim[0]);
	if (offset > scrdim[0] * scrdim[1]) return 1;

	buffer[offset] = color;

	return 0;
}
