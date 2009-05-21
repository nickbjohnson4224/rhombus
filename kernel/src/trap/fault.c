// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>

image_t *fault_generic(image_t *image) {
	if (image->cs & 0x3 == 0) panic("unknown exception");
	return ksignal(curr_pid, S_GEN, image->num, image->err, 0);
}

image_t *fault_page(image_t *image) {
	if (image->cs & 0x3 == 0) panic("page fault exception");
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	return ksignal(curr_pid, S_PAG, cr2, image->err, 0);
}

image_t *fault_float(image_t *image) {
	if (image->cs & 0x3 == 0) panic("floating point exception");
	return ksignal(curr_pid, S_FPE, image->eip, 0, 0);
}

image_t *fault_double(image_t *image) {
	panic("double fault exception");
	return NULL;
}

__attribute__ ((section(".ttext"))) 
void init_fault() {

	register_int(0,  fault_float);
	register_int(1,  fault_generic);
	register_int(2,  fault_generic);
	register_int(3,  fault_generic);
	register_int(4,  fault_generic);
	register_int(5,  fault_generic);
	register_int(6,  fault_generic);
	register_int(7,  fault_float);
	register_int(8,  fault_double);
	register_int(9,  fault_float);
	register_int(10, fault_generic);
	register_int(11, fault_generic);
	register_int(12, fault_generic);
	register_int(13, fault_generic);
	register_int(14, fault_page);
	register_int(15, fault_generic);
	register_int(16, fault_float);
	register_int(17, fault_generic);
	register_int(18, fault_generic);

}
