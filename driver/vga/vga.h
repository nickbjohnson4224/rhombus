#ifndef VGA_H
#define VGA_H

/*** Constants ***/

/* VGA controller registers */
#define AC_INDEX		0x3C0
#define AC_WRITE		0x3C0
#define AC_READ			0x3C1
#define MISC_WRITE		0x3C2
#define SEQ_INDEX		0x3C4
#define SEQ_DATA		0x3C5
#define DAC_READ_INDEX	0x3C7
#define DAC_WRITE_INDEX	0x3C8
#define DAC_DATA		0x3C9
#define MISC_READ		0x3CC
#define GC_INDEX		0x3CE
#define GC_DATA			0x3CF
#define CRTC_INDEX		0x3D4
#define CRTC_DATA		0x3D5
#define INSTAT_READ		0x3DA

#define NUM_SEQ_REGS	5
#define NUM_CRTC_REGS	25
#define NUM_GC_REGS		9
#define NUM_AC_REGS		21
#define NUM_REGS		(1+NUM_SEQ_REGS+NUM_CRTC_REGS+NUM_GC_REGS+NUM_AC_REGS)

/* VGA palette */
uint8_t get_color_true(uint32_t c);
uint8_t get_color_rgbi(uint32_t c);
uint8_t get_color_mono(uint32_t c);
uint8_t get_color_gray(uint32_t c);

void set_color_true(void);
void set_color_rgbi(void);
void set_color_mono(void);
void set_color_gray(void);

/* VGA modes */
extern struct vga_mode {
	/* register file */
	uint8_t misc;
	uint8_t seq	[5];
	uint8_t crtc[25];
	uint8_t gc	[9];
	uint8_t ac	[21];

	/* palette */
	uint8_t (*get_color)(uint32_t c);
	void (*set_color)(void);

	/* dimensions */
	size_t offset;
	size_t width;
	size_t height;
	size_t depth;

	/* drawing functions */
	void (*plot)(size_t x, size_t y, uint32_t c);
	void (*copy)(size_t x0, size_t y0, size_t x1, size_t y1, uint8_t *buffer);
	void (*fill)(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c);
	void (*line)(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c);
} modev[], *mode;

/* point plotting */
void vga_linear_plot(size_t x, size_t y, uint32_t c);
void vga_planar_plot(size_t x, size_t y, uint32_t c);

/* rectangle filling */
void vga_linear_fill(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c);
void vga_planar_fill(size_t x0, size_t y0, size_t x1, size_t y1, uint32_t c);

void vga_set_mode(int m);

#define MODE_80x25xT		0
#define MODE_640x480x2		1
#define MODE_640x480x16		2
#define MODE_320x200x256	3
#define MODE_320x240x256	4
#define MODE_320x240x64G	5

/* framebuffer */
uint8_t *vmem;

#endif/*VGA_H*/
